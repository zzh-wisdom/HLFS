#ifndef HLFS_UTIL_AEP_JUST_H_
#define HLFS_UTIL_AEP_JUST_H_

#include "util/aep.h"
#include "util/instr_just.h"

static force_inline void _just_less_16b_load(const char *src, size_t len) {
    dlog_assert(len < 16);
    if(len == 0) return;
    if (len <= 8) goto le8;
    asm volatile(
        "mov %[src1], %%rsi \n"
        "mov %[src2], %%rdi \n"
        "movq (%%rsi), %%r12 \n"
        "movq (%%rdi), %%r13 \n"
        :
        : [src1] "r"(src), [src2] "r"(src + len - 8)
        : "rsi", "rdi", "r12", "r13");
le8:
    if (len <= 2) goto le2;
    if (len > 4) {
        /* 5..8 */
        asm volatile(
            "mov %[src1], %%rsi \n"
            "mov %[src2], %%rdi \n"
            "mov (%%rsi), %%ecx \n"
            "mov (%%rdi), %%edx \n"
            :
            : [src1] "r"(src), [src2] "r"(src + len - 4)
            : "rsi", "rdi", "ecx", "edx");
        return;
    }
    {
        /* 3..4 */
        asm volatile(
            "mov %[src1], %%rsi \n"
            "mov %[src2], %%rdi \n"
            "mov (%%rsi), %%cx \n"
            "mov (%%rdi), %%dx \n"
            :
            : [src1] "r"(src), [src2] "r"(src + len - 2)
            : "rsi", "rdi", "cx", "dx");
        return;
    }

le2:
    if (len == 2) {
        asm volatile(
            "mov %[src1], %%rsi \n"
            "mov (%%rsi), %%cx \n"
            :
            : [src1] "r"(src)
            : "rsi", "rdi", "cx");
        return;
    }
    asm volatile(
        "mov %[src1], %%rsi \n"
        "mov (%%rsi), %%cl \n"
        :
        : [src1] "r"(src)
        : "rsi", "rdi", "cl");
}

static force_inline void _just_less_16b_store(char *dest, size_t len) {
    dlog_assert(len < 16);
    if(len == 0) return;
    uint64_t value = ~0ul;
    if (len <= 8) goto le8;
    { /* 9..16 */
        *(uint64_t *)dest = value;
        *(uint64_t *)(dest + len - 8) = value;
        return;
    }
le8:
    if (len <= 2) goto le2;
    if (len > 4) {
        /* 5..8 */
        *(uint32_t *)dest = value;
        *(uint32_t *)(dest + len - 4) = value;
        return;
    }
    {
        *(uint16_t *)dest = value;
        *(uint16_t *)(dest + len - 2) = value;
        return;
    }

le2:
    if (len == 2) {
        *(uint16_t *)dest = value;
        return;
    }
    *(uint8_t *)dest = value;
}

// load

static force_inline void _just_load(const char *src, size_t len) {
    size_t cnt = (uintptr_t)src & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        _just_less_16b_load(src, cnt);
        src += cnt;
        len -= cnt;
    }
    if (((uintptr_t)src & 16) && len >= 16) {
        _just_16b_load(src);
        src += 16;
        len -= 16;
    }
    if (((uintptr_t)src & 32) && len >= 32) {
        _just_32b_load(src);
        src += 32;
        len -= 32;
    }

    // while (len >= 32 * 32) {
    //     _just_32x32b_load(src);
    //     src += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        _just_16x32b_load(src);
        src += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        _just_8x32b_load(src);
        src += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        _just_4x32b_load(src);
        src += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        _just_2x32b_load(src);
        src += 2 * 32;
        len -= 2 * 32;
    }

    if (len >= 1 * 32) {
        _just_32b_load(src);
        src += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        _just_16b_load(src);
        src += 16;
        len -= 16;
    }

    _just_less_16b_load(src, cnt);
}

static force_inline void _just_ntload(const char *src, size_t len) {
    size_t cnt = (uintptr_t)src & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        _just_less_16b_load(src, cnt);
        src += cnt;
        len -= cnt;
    }
    if (((uintptr_t)src & 16) && len >= 16) {
        _just_16b_ntload(src);
        src += 16;
        len -= 16;
    }
    if (((uintptr_t)src & 32) && len >= 32) {
        _just_32b_ntload(src);
        src += 32;
        len -= 32;
    }

    // while (len >= 32 * 32) {
    //     _just_32x32b_ntload(src);
    //     src += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        _just_16x32b_ntload(src);
        src += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        _just_8x32b_ntload(src);
        src += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        _just_4x32b_ntload(src);
        src += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        _just_2x32b_ntload(src);
        src += 2 * 32;
        len -= 2 * 32;
    }

    if (len >= 1 * 32) {
        _just_32b_ntload(src);
        src += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        _just_16b_ntload(src);
        src += 16;
        len -= 16;
    }

    _just_less_16b_load(src, cnt);
}

// store

static force_inline void _just_store_noflush(char *dest, size_t len) {
    size_t cnt = (uintptr_t)dest & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        _just_less_16b_store(dest, cnt);
        dest += cnt;
        len -= cnt;
    }
    if (((uintptr_t)dest & 16) && len >= 16) {
        _just_16b_store_noflush(dest);
        dest += 16;
        len -= 16;
    }
    if (((uintptr_t)dest & 32) && len >= 32) {
        _just_32b_store_noflush(dest);
        dest += 32;
        len -= 32;
    }

    // while (len >= 32 * 32) {
    //     _just_32x32b_store_noflush(dest);
    //     dest += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        _just_16x32b_store_noflush(dest);
        dest += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        _just_8x32b_store_noflush(dest);
        dest += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        _just_4x32b_store_noflush(dest);
        dest += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        _just_2x32b_store_noflush(dest);
        dest += 2 * 32;
        len -= 2 * 32;
    }

    if (len >= 1 * 32) {
        _just_32b_store_noflush(dest);
        dest += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        _just_16b_store_noflush(dest);
        dest += 16;
        len -= 16;
    }

    _just_less_16b_store(dest, len);
}

static force_inline void _just_store_flush(char *dest, size_t len) {
    bool need_flush = (uintptr_t)dest & CACHELINE_MASK;
    size_t cnt = (uintptr_t)dest & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        _just_less_16b_store(dest, cnt);
        dest += cnt;
        len -= cnt;
    }
    if (((uintptr_t)dest & 16) && len >= 16) {
        _just_16b_store_noflush(dest);
        dest += 16;
        len -= 16;
    }
    if (((uintptr_t)dest & 32) && len >= 32) {
        _just_32b_store_noflush(dest);
        dest += 32;
        len -= 32;
    }
    if (need_flush) {
        clwb((uintptr_t)dest - CACHELINE_SIZE);
    }

    // while (len >= 32 * 32) {
    //     _just_32x32b_store_noflush(dest);
    //     dest += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        _just_16x32b_store_noflush(dest);
        dest += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        _just_8x32b_store_noflush(dest);
        dest += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        _just_4x32b_store_noflush(dest);
        dest += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        _just_2x32b_store_noflush(dest);
        dest += 2 * 32;
        len -= 2 * 32;
    }

    need_flush = (len != 0);
    if (len >= 1 * 32) {
        _just_32b_store_noflush(dest);
        dest += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        _just_16b_store_noflush(dest);
        dest += 16;
        len -= 16;
    }

    _just_less_16b_store(dest, len);

    if (need_flush) {
        clwb((uintptr_t)dest & CACHELINE_UNMASK);
    }
}

static force_inline void _just_ntstore(char *dest, size_t len) {
    size_t cnt = (uintptr_t)dest & 15;
    if (cnt > 0) {
        cnt = 16 - cnt;
        if (cnt > len) cnt = len;
        _just_less_16b_store(dest, cnt);
        clwb((uintptr_t)dest & CACHELINE_UNMASK);
        dest += cnt;
        len -= cnt;
    }
    if (((uintptr_t)dest & 16) && len >= 16) {
        _just_16b_ntstore(dest);
        dest += 16;
        len -= 16;
    }
    if (((uintptr_t)dest & 32) && len >= 32) {
        _just_32b_ntstore(dest);
        dest += 32;
        len -= 32;
    }

    // while (len >= 32 * 32) {
    //     _just_32x32b_ntstore(dest);
    //     dest += 32 * 32;
    //     len -= 32 * 32;
    // }

    while (len >= 16 * 32) {
        _just_16x32b_ntstore(dest);
        dest += 16 * 32;
        len -= 16 * 32;
    }

    if (len >= 8 * 32) {
        _just_8x32b_ntstore(dest);
        dest += 8 * 32;
        len -= 8 * 32;
    }

    if (len >= 4 * 32) {
        _just_4x32b_ntstore(dest);
        dest += 4 * 32;
        len -= 4 * 32;
    }

    if (len >= 2 * 32) {
        _just_2x32b_ntstore(dest);
        dest += 2 * 32;
        len -= 2 * 32;
    }

    if (len >= 1 * 32) {
        _just_32b_ntstore(dest);
        dest += 1 * 32;
        len -= 1 * 32;
    }

    if (len >= 16) {
        _just_16b_ntstore(dest);
        dest += 16;
        len -= 16;
    }

    _just_less_16b_store(dest, len);
    clwb((uintptr_t)dest & CACHELINE_UNMASK);
}

#endif  // HLFS_UTIL_AEP_JUST_H_
