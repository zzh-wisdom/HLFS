#include <iostream>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <assert.h>
#include <string.h>

using namespace std;

int main() {
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

    for(int i = 0; i < core_available; ++i) {
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
        for(int j = 0; j < 4e8; ++j) {
            mul = mul*mul;
        }
    }
    return 0;
}
