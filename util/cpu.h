#ifndef HLFS_UTIL_CPU_H_
#define HLFS_UTIL_CPU_H_

#include <pthread.h>
#include <sys/time.h>
#include <time.h>

#include <cstdint>
#include <vector>

#include "util/common.h"

bool CoreBind(pthread_t t, uint32_t core_id);
std::vector<int> CoreBindQuery(pthread_t t);

static force_inline uint64_t GetTsUsec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000000 + tv.tv_usec;
}

static force_inline uint64_t GetTsNsec() {
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    return tp.tv_sec * 1000000000ULL + tp.tv_nsec;
}

// 由于cpu频率不定，这个很难转换成时间
static force_inline uint64_t GetTsTick() {
    uint32_t lo, hi;
    uint64_t o;
    __asm__ __volatile__("rdtscp" : "=a"(lo), "=d"(hi) : : "%ecx");
    o = hi;
    o <<= 32;
    return (o | lo);
}

// 注意：指令wbinvd需要在内核空间使用
/* since Skylake, Intel uses non-inclusive last-level cache, drop cache before testing */
static inline void DropCache(void)
{
	asm volatile("wbinvd": : :"memory");
	asm volatile("mfence": : :"memory");
}

#endif // HLFS_UTIL_CPU_H_
