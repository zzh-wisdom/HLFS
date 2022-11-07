#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <cstdint>
#include <iostream>

using namespace std;

static __always_inline uint64_t GetTsNsec() {
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    return tp.tv_sec * 1000000000ULL + tp.tv_nsec;
}

static __always_inline uint64_t GetTsTick() {
    uint32_t lo, hi;
    uint64_t o;
    __asm__ __volatile__("rdtscp" : "=a"(lo), "=d"(hi) : : "%ecx");
    o = hi;
    o <<= 32;
    return (o | lo);
}

char* mem1;
char* mem2;

int main(int argc, char *argv[]) {
    assert(argc == 3);
    int mem_size = atoi(argv[1]);
    int count = atoi(argv[2]);
    mem1 = (char*)malloc(mem_size);
    mem2 = (char*)malloc(mem_size);
    memset(mem1, 0, mem_size);
    memset(mem1, 1, mem_size);

    for(int i = 0; i < count; ++i) {
        double tick_begin = GetTsTick();
        double ns_begin = GetTsNsec();
        memcpy(mem1, mem2, mem_size);
        double tick_end = GetTsTick();
        double ns_end = GetTsNsec();
        double cal = tick_end * ns_begin/tick_begin;
        printf("pass:%.2fns fact:%.2fns vs cal:%.2fns diff:%.2fns\n", ns_end - ns_begin, ns_end, cal, ns_end-cal);
    }

    free(mem1);
    free(mem2);
    return 0;
}
