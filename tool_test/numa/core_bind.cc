#include <assert.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <iostream>

using namespace std;

#define EAX_IDX 0
#define EBX_IDX 1
#define ECX_IDX 2
#define EDX_IDX 3

#define __cpuid_count(level, count, a, b, c, d)				\
  __asm__ __volatile__ ("cpuid\n\t"					\
			: "=a" (a), "=b" (b), "=c" (c), "=d" (d)	\
			: "0" (level), "2" (count))

static inline void
cpuid(unsigned func, unsigned subfunc, unsigned cpuinfo[4])
{
	__cpuid_count(func, subfunc, cpuinfo[EAX_IDX], cpuinfo[EBX_IDX],
			cpuinfo[ECX_IDX], cpuinfo[EDX_IDX]);
}

static int
is_cpu_feature_present(unsigned func, unsigned reg, unsigned bit)
{
	unsigned cpuinfo[4] = { 0 };

	/* check CPUID level first */
	cpuid(0x0, 0x0, cpuinfo);
	if (cpuinfo[EAX_IDX] < func)
		return 0;

	cpuid(func, 0x0, cpuinfo);
	return (cpuinfo[reg] & bit) != 0;
}

#define bit_MOVDIR64B	(1 << 28)

#define cpuid(in, a, b, c, d) asm("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(in))

static int getcpuid(char *id, size_t max) {
    unsigned long maxi, eax, ebx, ecx, edx, unused;
    cpuid(0, maxi, unused, unused, unused);
    maxi &= 0xffff;
    if (maxi < 3) {
        return -1;
    }

    cpuid(3, eax, ebx, ecx, edx);
    snprintf(id, max, "%08lx %08lx %08lx %08lx", eax, ebx, ecx, edx);
    fprintf(stdout, "get cpu id: %s\n", id);
    return 0;
}

int main() {

    // 检测相关指令是否支持
    int ret = is_cpu_feature_present(0x7, ECX_IDX, bit_MOVDIR64B);
    printf("MOVDIR64B support? : %d\n", ret);

    int total_core = sysconf(_SC_NPROCESSORS_CONF);
    assert(get_nprocs_conf() == total_core);
    int core_available = sysconf(_SC_NPROCESSORS_ONLN);
    assert(get_nprocs() == core_available);
    printf("total_core=%d core_available=%d\n", total_core, core_available);

    cpu_set_t cpuset;
    struct sched_param sp;
    const int expected_policy = SCHED_RR;
    int policy = expected_policy;
    int priority = (sched_get_priority_max(policy) + sched_get_priority_min(policy)) / 2;

    for (int i = 0; i < core_available; ++i) {
        // 绑核
        CPU_ZERO(&cpuset);
        CPU_SET(i, &cpuset);
        int ret_error = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        assert(ret_error == 0);
        // 设置调度策略和优先级
        memset(&sp, 0, sizeof(sp));
        sp.sched_priority = priority;
        ret_error = pthread_setschedparam(pthread_self(), policy, &sp);
        assert(ret_error == 0);

        // 查询绑核核优先级情况
        int core_num = 0;
        int core_id = -1;
        ret_error = pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
        assert(ret_error == 0);
        for (int i = 0; i < CPU_SETSIZE; i++)
            if (CPU_ISSET(i, &cpuset)) {
                ++core_num;
                core_id = i;
            }
        assert(core_num == 1);
        assert(core_id == i);
        policy = 0;
        memset(&sp, 0, sizeof(sp));
        ret_error = pthread_getschedparam(pthread_self(), &policy, &sp);
        assert(ret_error == 0);
        assert(policy == expected_policy);
        assert(sp.sched_priority == priority);
        fprintf(stderr, "thread[%ld] bind => [%d], policy:[%d], sched_priority:[%d]\n",
                pthread_self(), core_id, policy, sp.sched_priority);
        int mul = 2;
        for (int j = 0; j < 4e8; ++j) {
            mul = mul * mul;
        }
        char id[128];
        printf("return: %d\n", getcpuid(id, 128));
        printf("getcpuid:%s\n", id);
        printf("sched_getcpu:%d\n", sched_getcpu());
    }
    return 0;
}
