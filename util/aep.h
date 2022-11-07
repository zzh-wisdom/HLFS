#ifndef HLFS_UTIL_AEP_H_
#define HLFS_UTIL_AEP_H_

#include <libpmem2.h>

#include <string>

#include "util/instr.h"
#include "util/log.h"

// #if defined(__x86_64__)
// #include <mmintrin.h>
// #include <x86intrin.h>
// #else
// # error "Not support"
// #endif

#ifndef __x86_64__
#error "Not support, should run on __x86_64__"
#endif

#define CACHELINE_SIZE ((uintptr_t)64)
#define CACHELINE_MASK (CACHELINE_SIZE - 1)
#define CACHELINE_UNMASK (~CACHELINE_MASK)

#define clflush(addr) asm volatile("clflush %0" : "+m"(*(volatile char *)(addr)))
#define clflushopt(addr) asm volatile(".byte 0x66; clflush %0" : "+m"(*(volatile char *)(addr)));
#define clwb(addr) asm volatile(".byte 0x66; xsaveopt %0" : "+m"(*(volatile char *)(addr)));
#define lfence() __asm__ __volatile__("lfence" : : : "memory")
#define sfence() __asm__ __volatile__("sfence" : : : "memory")
#define mfence() __asm__ __volatile__("mfence" : : : "memory")
#define barrier() __asm__ __volatile__("" : : : "memory")

static force_inline void cpy_less_16b_noflush(char *dest, const char *src, size_t len) {
    dlog_assert(len < 16);
    if(len == 0) return;
    if (len <= 8) goto le8;
    { /* 9..16 */
        uint64_t d80 = *(uint64_t *)src;
        uint64_t d81 = *(uint64_t *)(src + len - 8);

        *(uint64_t *)dest = d80;
        *(uint64_t *)(dest + len - 8) = d81;
        return;
    }
le8:
    if (len <= 2) goto le2;
    if (len > 4) {
        /* 5..8 */
        uint32_t d40 = *(uint32_t *)src;
        uint32_t d41 = *(uint32_t *)(src + len - 4);

        *(uint32_t *)dest = d40;
        *(uint32_t *)(dest + len - 4) = d41;
        return;
    }
    {
        /* 3..4 */
        uint16_t d20 = *(uint16_t *)src;
        uint16_t d21 = *(uint16_t *)(src + len - 2);

        *(uint16_t *)dest = d20;
        *(uint16_t *)(dest + len - 2) = d21;
        return;
    }

le2:
    if (len == 2) {
        *(uint16_t *)dest = *(uint16_t *)src;
        return;
    }
    *(uint8_t *)dest = *(uint8_t *)src;
}

/******************************************************
 * native
 *****************************************************/

static force_inline void cpy_native_noflush(char *dest, const char *src, size_t len) {
    memcpy(dest, src, len);
}

static force_inline void cpy_native_flush(char *dest, const char *src, size_t len) {
    size_t cnt = (uintptr_t)dest & CACHELINE_MASK;
    if (cnt > 0) {
        cnt = CACHELINE_SIZE - cnt;
        if (cnt > len) cnt = len;
        memcpy(dest, src, cnt);
        clwb((uintptr_t)dest & CACHELINE_UNMASK);
        dest += cnt;
        src += cnt;
        len -= cnt;
    }
    while (len >= CACHELINE_SIZE) {
        memcpy(dest, src, CACHELINE_SIZE);
        clwb((uintptr_t)dest);
        dest += CACHELINE_SIZE;
        src += CACHELINE_SIZE;
        len -= cnt;
    }
    if (len == 0) return;
    memcpy(dest, src, len);
    clwb((uintptr_t)dest);
}

/******************************************************
 * load_and_store
 *****************************************************/

// 测试等于0时的正确性
static force_inline void cpy_load_store_noflush(char *dest, const char *src, size_t len) {
    size_t cnt = (uintptr_t)dest & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        cpy_less_16b_noflush(dest, src, cnt);
        dest += cnt;
        src += cnt;
        len -= cnt;
    }
    if (((uintptr_t)dest & 16) && len >= 16) {
        cpy_16b_load_store_noflush(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }
    if (((uintptr_t)dest & 32) && len >= 32) {
        cpy_32b_load_store_noflush(dest, src);
        dest += 32;
        src += 32;
        len -= 32;
    }

    // while (len >= 32 * 32) {
    //     cpy_32x32b_load_store_noflush(dest, src);
    //     dest += 32 * 32;
    //     src += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        cpy_16x32b_load_store_noflush(dest, src);
        dest += 16 * 32;
        src += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        cpy_8x32b_load_store_noflush(dest, src);
        dest += 8 * 32;
        src += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        cpy_4x32b_load_store_noflush(dest, src);
        dest += 4 * 32;
        src += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        cpy_2x32b_load_store_noflush(dest, src);
        dest += 2 * 32;
        src += 2 * 32;
        len -= 2 * 32;
    }

    if (len >= 1 * 32) {
        cpy_32b_load_store_noflush(dest, src);
        dest += 1 * 32;
        src += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        cpy_16b_load_store_noflush(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }

    cpy_less_16b_noflush(dest, src, len);
}

static force_inline void cpy_load_store_flush(char *dest, const char *src, size_t len) {
    bool need_flush = (uintptr_t)dest & CACHELINE_MASK;
    size_t cnt = (uintptr_t)dest & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        cpy_less_16b_noflush(dest, src, cnt);
        dest += cnt;
        src += cnt;
        len -= cnt;
    }
    if (((uintptr_t)dest & 16) && len >= 16) {
        cpy_16b_load_store_noflush(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }
    if (((uintptr_t)dest & 32) && len >= 32) {
        cpy_32b_load_store_noflush(dest, src);
        dest += 32;
        src += 32;
        len -= 32;
    }
    if (need_flush) {
        clwb((uintptr_t)dest - CACHELINE_SIZE);
    }

    // while (len >= 32 * 32) {
    //     cpy_32x32b_load_store_flush(dest, src);
    //     dest += 32 * 32;
    //     src += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        cpy_16x32b_load_store_flush(dest, src);
        dest += 16 * 32;
        src += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        cpy_8x32b_load_store_flush(dest, src);
        dest += 8 * 32;
        src += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        cpy_4x32b_load_store_flush(dest, src);
        dest += 4 * 32;
        src += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        cpy_2x32b_load_store_flush(dest, src);
        dest += 2 * 32;
        src += 2 * 32;
        len -= 2 * 32;
    }

    need_flush = (len != 0);
    if (len >= 1 * 32) {
        cpy_32b_load_store_noflush(dest, src);
        dest += 1 * 32;
        src += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        cpy_16b_load_store_noflush(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }

    cpy_less_16b_noflush(dest, src, len);

    if (need_flush) {
        clwb((uintptr_t)dest & CACHELINE_UNMASK);
    }
}

/******************************************************
 * ntload_and_store
 *****************************************************/

static force_inline void cpy_ntload_store_noflush(char *dest, const char *src, size_t len) {
    size_t cnt = (uintptr_t)dest & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        cpy_less_16b_noflush(dest, src, cnt);
        dest += cnt;
        src += cnt;
        len -= cnt;
    }
    if (((uintptr_t)dest & 16) && len >= 16) {
        cpy_16b_ntload_store_noflush(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }
    if (((uintptr_t)dest & 32) && len >= 32) {
        cpy_32b_ntload_store_noflush(dest, src);
        dest += 32;
        src += 32;
        len -= 32;
    }

    // while (len >= 32 * 32) {
    //     cpy_32x32b_ntload_store_noflush(dest, src);
    //     dest += 32 * 32;
    //     src += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        cpy_16x32b_ntload_store_noflush(dest, src);
        dest += 16 * 32;
        src += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        cpy_8x32b_ntload_store_noflush(dest, src);
        dest += 8 * 32;
        src += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        cpy_4x32b_ntload_store_noflush(dest, src);
        dest += 4 * 32;
        src += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        cpy_2x32b_ntload_store_noflush(dest, src);
        dest += 2 * 32;
        src += 2 * 32;
        len -= 2 * 32;
    }

    if (len >= 1 * 32) {
        cpy_32b_ntload_store_noflush(dest, src);
        dest += 1 * 32;
        src += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        cpy_16b_ntload_store_noflush(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }

    cpy_less_16b_noflush(dest, src, len);
}

static force_inline void cpy_ntload_store_flush(char *dest, const char *src, size_t len) {
    bool need_flush = (uintptr_t)dest & CACHELINE_MASK;
    size_t cnt = (uintptr_t)dest & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        cpy_less_16b_noflush(dest, src, cnt);
        dest += cnt;
        src += cnt;
        len -= cnt;
    }
    if (((uintptr_t)dest & 16) && len >= 16) {
        cpy_16b_ntload_store_noflush(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }
    if (((uintptr_t)dest & 32) && len >= 32) {
        cpy_32b_ntload_store_noflush(dest, src);
        dest += 32;
        src += 32;
        len -= 32;
    }
    if (need_flush) {
        clwb((uintptr_t)dest - CACHELINE_SIZE);
    }

    // while (len >= 32 * 32) {
    //     cpy_32x32b_ntload_store_flush(dest, src);
    //     dest += 32 * 32;
    //     src += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        cpy_16x32b_ntload_store_flush(dest, src);
        dest += 16 * 32;
        src += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        cpy_8x32b_ntload_store_flush(dest, src);
        dest += 8 * 32;
        src += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        cpy_4x32b_ntload_store_flush(dest, src);
        dest += 4 * 32;
        src += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        cpy_2x32b_ntload_store_flush(dest, src);
        dest += 2 * 32;
        src += 2 * 32;
        len -= 2 * 32;
    }

    need_flush = (len != 0);
    if (len >= 1 * 32) {
        cpy_32b_ntload_store_noflush(dest, src);
        dest += 1 * 32;
        src += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        cpy_16b_ntload_store_noflush(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }

    cpy_less_16b_noflush(dest, src, len);

    if (need_flush) {
        clwb((uintptr_t)dest & CACHELINE_UNMASK);
    }
}

/******************************************************
 * load_and_ntstore
 *****************************************************/

static force_inline void cpy_load_ntstore(char *dest, const char *src, size_t len) {
    size_t cnt = (uintptr_t)dest & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        cpy_less_16b_noflush(dest, src, cnt);
        clwb((uintptr_t)dest & CACHELINE_UNMASK);
        dest += cnt;
        src += cnt;
        len -= cnt;
    }
    if (((uintptr_t)dest & 16) && len >= 16) {
        cpy_16b_load_ntstore(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }
    if (((uintptr_t)dest & 32) && len >= 32) {
        cpy_32b_load_ntstore(dest, src);
        dest += 32;
        src += 32;
        len -= 32;
    }

    // while (len >= 32 * 32) {
    //     cpy_32x32b_load_ntstore(dest, src);
    //     dest += 32 * 32;
    //     src += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        cpy_16x32b_load_ntstore(dest, src);
        dest += 16 * 32;
        src += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        cpy_8x32b_load_ntstore(dest, src);
        dest += 8 * 32;
        src += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        cpy_4x32b_load_ntstore(dest, src);
        dest += 4 * 32;
        src += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        cpy_2x32b_load_ntstore(dest, src);
        dest += 2 * 32;
        src += 2 * 32;
        len -= 2 * 32;
    }

    if (len >= 1 * 32) {
        cpy_32b_load_ntstore(dest, src);
        dest += 1 * 32;
        src += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        cpy_16b_load_ntstore(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }

    cpy_less_16b_noflush(dest, src, len);
    clwb((uintptr_t)dest & CACHELINE_UNMASK);
}

/******************************************************
 * ntload_and_ntstore
 *****************************************************/

static force_inline void cpy_ntload_ntstore(char *dest, const char *src, size_t len) {
    size_t cnt = (uintptr_t)dest & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        cpy_less_16b_noflush(dest, src, cnt);
        clwb((uintptr_t)dest & CACHELINE_UNMASK);
        dest += cnt;
        src += cnt;
        len -= cnt;
    }
    if (((uintptr_t)dest & 16) && len >= 16) {
        cpy_16b_ntload_ntstore(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }
    if (((uintptr_t)dest & 32) && len >= 32) {
        cpy_32b_ntload_ntstore(dest, src);
        dest += 32;
        src += 32;
        len -= 32;
    }

    // while (len >= 32 * 32) {
    //     cpy_32x32b_ntload_ntstore(dest, src);
    //     dest += 32 * 32;
    //     src += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        cpy_16x32b_ntload_ntstore(dest, src);
        dest += 16 * 32;
        src += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        cpy_8x32b_ntload_ntstore(dest, src);
        dest += 8 * 32;
        src += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        cpy_4x32b_ntload_ntstore(dest, src);
        dest += 4 * 32;
        src += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        cpy_2x32b_ntload_ntstore(dest, src);
        dest += 2 * 32;
        src += 2 * 32;
        len -= 2 * 32;
    }

    if (len >= 1 * 32) {
        cpy_32b_ntload_ntstore(dest, src);
        dest += 1 * 32;
        src += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        cpy_16b_ntload_ntstore(dest, src);
        dest += 16;
        src += 16;
        len -= 16;
    }

    cpy_less_16b_noflush(dest, src, len);
    clwb((uintptr_t)dest & CACHELINE_UNMASK);
}

struct pmem2_map *Pmem2Map(const std::string &dev_file);

struct pmem2_map *Pmem2MapAndTruncate(const std::string &file, uint64_t size);

void Pmem2UnMap(struct pmem2_map **map);

#endif  // HLFS_UTIL_AEP_H_
