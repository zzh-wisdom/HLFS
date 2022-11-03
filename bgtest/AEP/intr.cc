#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <libpmem2.h>

// #if defined(__x86_64__)
// #include <mmintrin.h>
// #include <x86intrin.h>
// #else
// # error "Not support"
// #endif

using namespace std;

#ifndef __x86_64__
# error "Not support, should run on __x86_64__"
#endif

#define clflush(addr)\
	asm volatile("clflush %0" : "+m" (*(volatile char *)(addr)))
#define clflushopt(addr)\
	asm volatile(".byte 0x66; clflush %0" :\
	"+m" (*(volatile char *)(addr)));
#define clwb(addr)\
	asm volatile(".byte 0x66; xsaveopt %0" :\
	"+m" (*(volatile char *)(addr)));
#define lfence() __asm__ __volatile__("lfence": : :"memory")
#define sfence() __asm__ __volatile__("sfence": : :"memory")
#define mfence() __asm__ __volatile__("mfence": : :"memory")
#define barrier() __asm__ __volatile__("": : :"memory")

void ntload_ntstore_64byte(const char *dst, const char *src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dst], %%rdi \n"
        "vmovntdqa 0*32(%%rsi), %%ymm0 \n"
        "vmovntdqa 1*32(%%rsi), %%ymm1 \n"
        "vmovntpd %%ymm0, 0*32(%%rdi) \n"
        "vmovntpd %%ymm1, 1*32(%%rdi) \n"
        :
        : [src] "r" (src), [dst] "r" (dst)
        : "rsi", "rdi", "ymm0", "ymm1"
    );
}

void load_ntstore_64byte(const char *dst, const char *src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dst], %%rdi \n"
        "vmovdqa 0*32(%%rsi), %%ymm0 \n"
        "vmovdqa 1*32(%%rsi), %%ymm1 \n"
        "vmovntpd %%ymm0, 0*32(%%rdi) \n"
        "vmovntpd %%ymm1, 1*32(%%rdi) \n"
        :
        : [src] "r" (src), [dst] "r" (dst)
        : "rsi", "rdi", "ymm0", "ymm1"
    );
}

void load_64byte_fence(const char *addr) {
    asm volatile(
        "mov %[memarea], %%rsi \n"
        // "mfence \n"
        "vmovdqa 0*32(%%rsi), %%ymm0 \n"
        "vmovdqa 1*32(%%rsi), %%ymm1 \n"
        :
        : [memarea] "r" (addr)
        : "rsi", "rdi", "ymm0", "ymm1");
}
char nstore_64byte_label[1024] = "0123456789012345678901234567890123456789012345678901234567890123456789"
        "0123456789012345678901234567890123456789";

const char* devdax_path = "/dev/dax1.0";

int main() {
    int fd;
	struct pmem2_config *cfg;
	struct pmem2_map *map;
	struct pmem2_source *src;

    if ((fd = open(devdax_path, O_RDWR)) < 0) {
		perror("open");
		exit(1);
	}

	if (pmem2_config_new(&cfg)) {
		pmem2_perror("pmem2_config_new");
		exit(1);
	}

	if (pmem2_source_from_fd(&src, fd)) {
		pmem2_perror("pmem2_source_from_fd");
		exit(1);
	}

	if (pmem2_config_set_required_store_granularity(cfg,
			PMEM2_GRANULARITY_PAGE)) {
		pmem2_perror("pmem2_config_set_required_store_granularity");
		exit(1);
	}

	if (pmem2_map_new(&map, cfg, src)) {
		pmem2_perror("pmem2_map");
		exit(1);
	}

    char *addr = (char*)pmem2_map_get_address(map);
    size_t size = pmem2_map_get_size(map);

    printf("set before: \n%s", addr);

    auto memset_fn = pmem2_get_memset_fn(map);
    memset_fn(addr, 0, 1024, 0);
    const char* _mm_clflush_label = "_mm_clflush,_mm_clflush,_mm_clflush,_mm_clflush,_mm_clflush"
        ",_mm_clflush,_mm_clflush,_mm_clflush,_mm_clflush,_mm_clflush,_mm_clflush";
    const char* _mm_clflushopt_label = "_mm_clflushopt,_mm_clflushopt,_mm_clflushopt,_mm_clflushopt"
        ",_mm_clflushopt,_mm_clflushopt,_mm_clflushopt,_mm_clflushopt,_mm_clflushopt";
    const char* _mm_clwb_label = "_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb"
        ",_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb";
    memcpy(addr, _mm_clflush_label, 64);
    addr[63] = '\n';
    clflush(addr);
    memcpy(addr+64, _mm_clflushopt_label, 64);
    addr[63+64] = '\n';
    clflushopt(addr+64);
    memcpy(addr+64+64, _mm_clwb_label, 64);
    addr[63+64+64] = '\n';
    clwb(addr+64+64);
    barrier();
    // load_64byte_fence(nstore_64byte_label);
    // printf("load_64byte_fence\n");
    nstore_64byte_label[63] = '\n';
    ntload_ntstore_64byte(addr+64+64+64, nstore_64byte_label);
    load_ntstore_64byte(addr+64+64+64+64, nstore_64byte_label);
    sfence();
    lfence();
    mfence();
    abort();
    return 0;
}