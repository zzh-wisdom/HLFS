#ifndef HLFS_UTIL_INSTR_JUST_H_
#define HLFS_UTIL_INSTR_JUST_H_

#include "util/instr.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
/// for test

/******************************************************
 * just load
 *****************************************************/

static force_inline void _just_16b_load(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        LOAD_INSTR" 0*16(%%rsi), %%xmm0 \n"
        :
        : [src] "r" (src)
        : "rsi", "xmm0"
    );
}

static force_inline void _just_32b_load(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        :
        : [src] "r" (src)
        : "rsi", YMM_1_REG
    );
}

static force_inline void _just_2x32b_load(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        :
        : [src] "r" (src)
        : "rsi", YMM_2_REG
    );
}

static force_inline void _just_4x32b_load(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        LOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        LOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        :
        : [src] "r" (src)
        : "rsi", YMM_4_REG
    );
}

static force_inline void _just_8x32b_load(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        LOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        LOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        LOAD_INSTR" 4*32(%%rsi), %%ymm4 \n"
        LOAD_INSTR" 5*32(%%rsi), %%ymm5 \n"
        LOAD_INSTR" 6*32(%%rsi), %%ymm6 \n"
        LOAD_INSTR" 7*32(%%rsi), %%ymm7 \n"
        :
        : [src] "r" (src)
        : "rsi", YMM_8_REG
    );
}

static force_inline void _just_16x32b_load(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        LOAD_INSTR" 0*32(%%rsi),   %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi),   %%ymm1 \n"
        LOAD_INSTR" 2*32(%%rsi),   %%ymm2 \n"
        LOAD_INSTR" 3*32(%%rsi),   %%ymm3 \n"
        LOAD_INSTR" 4*32(%%rsi),   %%ymm4 \n"
        LOAD_INSTR" 5*32(%%rsi),   %%ymm5 \n"
        LOAD_INSTR" 6*32(%%rsi),   %%ymm6 \n"
        LOAD_INSTR" 7*32(%%rsi),   %%ymm7 \n"
        LOAD_INSTR" 8*32(%%rsi),   %%ymm8 \n"
        LOAD_INSTR" 9*32(%%rsi),   %%ymm9 \n"
        LOAD_INSTR" 10*32(%%rsi), %%ymm10 \n"
        LOAD_INSTR" 11*32(%%rsi), %%ymm11 \n"
        LOAD_INSTR" 12*32(%%rsi), %%ymm12 \n"
        LOAD_INSTR" 13*32(%%rsi), %%ymm13 \n"
        LOAD_INSTR" 14*32(%%rsi), %%ymm14 \n"
        LOAD_INSTR" 15*32(%%rsi), %%ymm15 \n"
        :
        : [src] "r" (src)
        : "rsi", YMM_16_REG
    );
}

// static force_inline void _just_32x32b_load(const char* src) {
//     asm volatile(
//         "mov %[src], %%rsi \n"
//         LOAD_INSTR" 0*32(%%rsi),   %%ymm0 \n"
//         LOAD_INSTR" 1*32(%%rsi),   %%ymm1 \n"
//         LOAD_INSTR" 2*32(%%rsi),   %%ymm2 \n"
//         LOAD_INSTR" 3*32(%%rsi),   %%ymm3 \n"
//         LOAD_INSTR" 4*32(%%rsi),   %%ymm4 \n"
//         LOAD_INSTR" 5*32(%%rsi),   %%ymm5 \n"
//         LOAD_INSTR" 6*32(%%rsi),   %%ymm6 \n"
//         LOAD_INSTR" 7*32(%%rsi),   %%ymm7 \n"
//         LOAD_INSTR" 8*32(%%rsi),   %%ymm8 \n"
//         LOAD_INSTR" 9*32(%%rsi),   %%ymm9 \n"
//         LOAD_INSTR" 10*32(%%rsi), %%ymm10 \n"
//         LOAD_INSTR" 11*32(%%rsi), %%ymm11 \n"
//         LOAD_INSTR" 12*32(%%rsi), %%ymm12 \n"
//         LOAD_INSTR" 13*32(%%rsi), %%ymm13 \n"
//         LOAD_INSTR" 14*32(%%rsi), %%ymm14 \n"
//         LOAD_INSTR" 15*32(%%rsi), %%ymm15 \n"
//         LOAD_INSTR" 16*32(%%rsi), %%ymm16 \n"
//         LOAD_INSTR" 17*32(%%rsi), %%ymm17 \n"
//         LOAD_INSTR" 18*32(%%rsi), %%ymm18 \n"
//         LOAD_INSTR" 19*32(%%rsi), %%ymm19 \n"
//         LOAD_INSTR" 20*32(%%rsi), %%ymm20 \n"
//         LOAD_INSTR" 21*32(%%rsi), %%ymm21 \n"
//         LOAD_INSTR" 22*32(%%rsi), %%ymm22 \n"
//         LOAD_INSTR" 23*32(%%rsi), %%ymm23 \n"
//         LOAD_INSTR" 24*32(%%rsi), %%ymm24 \n"
//         LOAD_INSTR" 25*32(%%rsi), %%ymm25 \n"
//         LOAD_INSTR" 26*32(%%rsi), %%ymm26 \n"
//         LOAD_INSTR" 27*32(%%rsi), %%ymm27 \n"
//         LOAD_INSTR" 28*32(%%rsi), %%ymm28 \n"
//         LOAD_INSTR" 29*32(%%rsi), %%ymm29 \n"
//         LOAD_INSTR" 30*32(%%rsi), %%ymm30 \n"
//         LOAD_INSTR" 31*32(%%rsi), %%ymm31 \n"
//         :
//         : [src] "r" (src)
//         : "rsi", YMM_32_REG
//     );
// }

/******************************************************
 * just ntload
 *****************************************************/
static force_inline void _just_16b_ntload(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        NTLOAD_INSTR" 0*16(%%rsi), %%xmm0 \n"
        :
        : [src] "r" (src)
        : "rsi", "xmm0"
    );
}

static force_inline void _just_32b_ntload(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        :
        : [src] "r" (src)
        : "rsi", YMM_1_REG
    );
}

static force_inline void _just_2x32b_ntload(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        :
        : [src] "r" (src)
        : "rsi", YMM_2_REG
    );
}

static force_inline void _just_4x32b_ntload(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        NTLOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        NTLOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        :
        : [src] "r" (src)
        : "rsi", YMM_4_REG
    );
}

static force_inline void _just_8x32b_ntload(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        NTLOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        NTLOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        NTLOAD_INSTR" 4*32(%%rsi), %%ymm4 \n"
        NTLOAD_INSTR" 5*32(%%rsi), %%ymm5 \n"
        NTLOAD_INSTR" 6*32(%%rsi), %%ymm6 \n"
        NTLOAD_INSTR" 7*32(%%rsi), %%ymm7 \n"
        :
        : [src] "r" (src)
        : "rsi", YMM_8_REG
    );
}

static force_inline void _just_16x32b_ntload(const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        NTLOAD_INSTR" 0*32(%%rsi),   %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi),   %%ymm1 \n"
        NTLOAD_INSTR" 2*32(%%rsi),   %%ymm2 \n"
        NTLOAD_INSTR" 3*32(%%rsi),   %%ymm3 \n"
        NTLOAD_INSTR" 4*32(%%rsi),   %%ymm4 \n"
        NTLOAD_INSTR" 5*32(%%rsi),   %%ymm5 \n"
        NTLOAD_INSTR" 6*32(%%rsi),   %%ymm6 \n"
        NTLOAD_INSTR" 7*32(%%rsi),   %%ymm7 \n"
        NTLOAD_INSTR" 8*32(%%rsi),   %%ymm8 \n"
        NTLOAD_INSTR" 9*32(%%rsi),   %%ymm9 \n"
        NTLOAD_INSTR" 10*32(%%rsi), %%ymm10 \n"
        NTLOAD_INSTR" 11*32(%%rsi), %%ymm11 \n"
        NTLOAD_INSTR" 12*32(%%rsi), %%ymm12 \n"
        NTLOAD_INSTR" 13*32(%%rsi), %%ymm13 \n"
        NTLOAD_INSTR" 14*32(%%rsi), %%ymm14 \n"
        NTLOAD_INSTR" 15*32(%%rsi), %%ymm15 \n"
        :
        : [src] "r" (src)
        : "rsi", YMM_16_REG
    );
}

// static force_inline void _just_32x32b_ntload(const char* src) {
//     asm volatile(
//         "mov %[src], %%rsi \n"
//         NTLOAD_INSTR" 0*32(%%rsi),   %%ymm0 \n"
//         NTLOAD_INSTR" 1*32(%%rsi),   %%ymm1 \n"
//         NTLOAD_INSTR" 2*32(%%rsi),   %%ymm2 \n"
//         NTLOAD_INSTR" 3*32(%%rsi),   %%ymm3 \n"
//         NTLOAD_INSTR" 4*32(%%rsi),   %%ymm4 \n"
//         NTLOAD_INSTR" 5*32(%%rsi),   %%ymm5 \n"
//         NTLOAD_INSTR" 6*32(%%rsi),   %%ymm6 \n"
//         NTLOAD_INSTR" 7*32(%%rsi),   %%ymm7 \n"
//         NTLOAD_INSTR" 8*32(%%rsi),   %%ymm8 \n"
//         NTLOAD_INSTR" 9*32(%%rsi),   %%ymm9 \n"
//         NTLOAD_INSTR" 10*32(%%rsi), %%ymm10 \n"
//         NTLOAD_INSTR" 11*32(%%rsi), %%ymm11 \n"
//         NTLOAD_INSTR" 12*32(%%rsi), %%ymm12 \n"
//         NTLOAD_INSTR" 13*32(%%rsi), %%ymm13 \n"
//         NTLOAD_INSTR" 14*32(%%rsi), %%ymm14 \n"
//         NTLOAD_INSTR" 15*32(%%rsi), %%ymm15 \n"
//         NTLOAD_INSTR" 16*32(%%rsi), %%ymm16 \n"
//         NTLOAD_INSTR" 17*32(%%rsi), %%ymm17 \n"
//         NTLOAD_INSTR" 18*32(%%rsi), %%ymm18 \n"
//         NTLOAD_INSTR" 19*32(%%rsi), %%ymm19 \n"
//         NTLOAD_INSTR" 20*32(%%rsi), %%ymm20 \n"
//         NTLOAD_INSTR" 21*32(%%rsi), %%ymm21 \n"
//         NTLOAD_INSTR" 22*32(%%rsi), %%ymm22 \n"
//         NTLOAD_INSTR" 23*32(%%rsi), %%ymm23 \n"
//         NTLOAD_INSTR" 24*32(%%rsi), %%ymm24 \n"
//         NTLOAD_INSTR" 25*32(%%rsi), %%ymm25 \n"
//         NTLOAD_INSTR" 26*32(%%rsi), %%ymm26 \n"
//         NTLOAD_INSTR" 27*32(%%rsi), %%ymm27 \n"
//         NTLOAD_INSTR" 28*32(%%rsi), %%ymm28 \n"
//         NTLOAD_INSTR" 29*32(%%rsi), %%ymm29 \n"
//         NTLOAD_INSTR" 30*32(%%rsi), %%ymm30 \n"
//         NTLOAD_INSTR" 31*32(%%rsi), %%ymm31 \n"
//         :
//         : [src] "r" (src)
//         : "rsi", YMM_32_REG
//     );
// }

/******************************************************
 * just store
 *****************************************************/

static force_inline void _just_16b_store_noflush(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        STORE_INSTR" %%xmm0, 0*16(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", "xmm0"
    );
}

static force_inline void _just_32b_store_noflush(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_1_REG
    );
}

static force_inline void _just_2x32b_store_noflush(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_2_REG
    );
}

static force_inline void _just_4x32b_store_noflush(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        STORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_4_REG
    );
}

static force_inline void _just_8x32b_store_noflush(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        STORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        STORE_INSTR" %%ymm4, 4*32(%%rdi) \n"
        STORE_INSTR" %%ymm5, 5*32(%%rdi) \n"
        STORE_INSTR" %%ymm6, 6*32(%%rdi) \n"
        STORE_INSTR" %%ymm7, 7*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_8_REG
    );
}

static force_inline void _just_16x32b_store_noflush(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        STORE_INSTR" %%ymm0,   0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1,   1*32(%%rdi) \n"
        STORE_INSTR" %%ymm2,   2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3,   3*32(%%rdi) \n"
        STORE_INSTR" %%ymm4,   4*32(%%rdi) \n"
        STORE_INSTR" %%ymm5,   5*32(%%rdi) \n"
        STORE_INSTR" %%ymm6,   6*32(%%rdi) \n"
        STORE_INSTR" %%ymm7,   7*32(%%rdi) \n"
        STORE_INSTR" %%ymm8,   8*32(%%rdi) \n"
        STORE_INSTR" %%ymm9,   9*32(%%rdi) \n"
        STORE_INSTR" %%ymm10, 10*32(%%rdi) \n"
        STORE_INSTR" %%ymm11, 11*32(%%rdi) \n"
        STORE_INSTR" %%ymm12, 12*32(%%rdi) \n"
        STORE_INSTR" %%ymm13, 13*32(%%rdi) \n"
        STORE_INSTR" %%ymm14, 14*32(%%rdi) \n"
        STORE_INSTR" %%ymm15, 15*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_16_REG
    );
}

// static force_inline void _just_32x32b_store_noflush(char* dest) {
//     asm volatile(
//         "mov %[dest], %%rdi \n"
//         STORE_INSTR" %%ymm0,   0*32(%%rdi) \n"
//         STORE_INSTR" %%ymm1,   1*32(%%rdi) \n"
//         STORE_INSTR" %%ymm2,   2*32(%%rdi) \n"
//         STORE_INSTR" %%ymm3,   3*32(%%rdi) \n"
//         STORE_INSTR" %%ymm4,   4*32(%%rdi) \n"
//         STORE_INSTR" %%ymm5,   5*32(%%rdi) \n"
//         STORE_INSTR" %%ymm6,   6*32(%%rdi) \n"
//         STORE_INSTR" %%ymm7,   7*32(%%rdi) \n"
//         STORE_INSTR" %%ymm8,   8*32(%%rdi) \n"
//         STORE_INSTR" %%ymm9,   9*32(%%rdi) \n"
//         STORE_INSTR" %%ymm10, 10*32(%%rdi) \n"
//         STORE_INSTR" %%ymm11, 11*32(%%rdi) \n"
//         STORE_INSTR" %%ymm12, 12*32(%%rdi) \n"
//         STORE_INSTR" %%ymm13, 13*32(%%rdi) \n"
//         STORE_INSTR" %%ymm14, 14*32(%%rdi) \n"
//         STORE_INSTR" %%ymm15, 15*32(%%rdi) \n"
//         STORE_INSTR" %%ymm16, 16*32(%%rdi) \n"
//         STORE_INSTR" %%ymm17, 17*32(%%rdi) \n"
//         STORE_INSTR" %%ymm18, 18*32(%%rdi) \n"
//         STORE_INSTR" %%ymm19, 19*32(%%rdi) \n"
//         STORE_INSTR" %%ymm20, 20*32(%%rdi) \n"
//         STORE_INSTR" %%ymm21, 21*32(%%rdi) \n"
//         STORE_INSTR" %%ymm22, 22*32(%%rdi) \n"
//         STORE_INSTR" %%ymm23, 23*32(%%rdi) \n"
//         STORE_INSTR" %%ymm24, 24*32(%%rdi) \n"
//         STORE_INSTR" %%ymm25, 25*32(%%rdi) \n"
//         STORE_INSTR" %%ymm26, 26*32(%%rdi) \n"
//         STORE_INSTR" %%ymm27, 27*32(%%rdi) \n"
//         STORE_INSTR" %%ymm28, 28*32(%%rdi) \n"
//         STORE_INSTR" %%ymm29, 29*32(%%rdi) \n"
//         STORE_INSTR" %%ymm30, 30*32(%%rdi) \n"
//         STORE_INSTR" %%ymm31, 31*32(%%rdi) \n"
//         :
//         : [dest] "r" (dest)
//         : "rdi", YMM_32_REG
//     );
// }

static force_inline void _just_2x32b_store_flush(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        "clwb 0*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_2_REG
    );
}

static force_inline void _just_4x32b_store_flush(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        "clwb 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        "clwb 2*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_4_REG
    );
}

static force_inline void _just_8x32b_store_flush(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        "clwb 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        "clwb 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm4, 4*32(%%rdi) \n"
        STORE_INSTR" %%ymm5, 5*32(%%rdi) \n"
        "clwb 4*32(%%rdi) \n"
        STORE_INSTR" %%ymm6, 6*32(%%rdi) \n"
        STORE_INSTR" %%ymm7, 7*32(%%rdi) \n"
        "clwb 6*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_8_REG
    );
}

static force_inline void _just_16x32b_store_flush(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        STORE_INSTR" %%ymm0,   0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1,   1*32(%%rdi) \n"
        "clwb 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm2,   2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3,   3*32(%%rdi) \n"
        "clwb 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm4,   4*32(%%rdi) \n"
        STORE_INSTR" %%ymm5,   5*32(%%rdi) \n"
        "clwb 4*32(%%rdi) \n"
        STORE_INSTR" %%ymm6,   6*32(%%rdi) \n"
        STORE_INSTR" %%ymm7,   7*32(%%rdi) \n"
        "clwb 6*32(%%rdi) \n"
        STORE_INSTR" %%ymm8,   8*32(%%rdi) \n"
        STORE_INSTR" %%ymm9,   9*32(%%rdi) \n"
        "clwb 8*32(%%rdi) \n"
        STORE_INSTR" %%ymm10, 10*32(%%rdi) \n"
        STORE_INSTR" %%ymm11, 11*32(%%rdi) \n"
        "clwb 10*32(%%rdi) \n"
        STORE_INSTR" %%ymm12, 12*32(%%rdi) \n"
        STORE_INSTR" %%ymm13, 13*32(%%rdi) \n"
        "clwb 12*32(%%rdi) \n"
        STORE_INSTR" %%ymm14, 14*32(%%rdi) \n"
        STORE_INSTR" %%ymm15, 15*32(%%rdi) \n"
        "clwb 14*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_16_REG
    );
}

// static force_inline void _just_32x32b_store_flush(char* dest) {
//     asm volatile(
//         "mov %[dest], %%rdi \n"
//         STORE_INSTR" %%ymm0,   0*32(%%rdi) \n"
//         STORE_INSTR" %%ymm1,   1*32(%%rdi) \n"
//         "clwb 0*32(%%rdi) \n"
//         STORE_INSTR" %%ymm2,   2*32(%%rdi) \n"
//         STORE_INSTR" %%ymm3,   3*32(%%rdi) \n"
//         "clwb 2*32(%%rdi) \n"
//         STORE_INSTR" %%ymm4,   4*32(%%rdi) \n"
//         STORE_INSTR" %%ymm5,   5*32(%%rdi) \n"
//         "clwb 4*32(%%rdi) \n"
//         STORE_INSTR" %%ymm6,   6*32(%%rdi) \n"
//         STORE_INSTR" %%ymm7,   7*32(%%rdi) \n"
//         "clwb 6*32(%%rdi) \n"
//         STORE_INSTR" %%ymm8,   8*32(%%rdi) \n"
//         STORE_INSTR" %%ymm9,   9*32(%%rdi) \n"
//         "clwb 8*32(%%rdi) \n"
//         STORE_INSTR" %%ymm10, 10*32(%%rdi) \n"
//         STORE_INSTR" %%ymm11, 11*32(%%rdi) \n"
//         "clwb 10*32(%%rdi) \n"
//         STORE_INSTR" %%ymm12, 12*32(%%rdi) \n"
//         STORE_INSTR" %%ymm13, 13*32(%%rdi) \n"
//         "clwb 12*32(%%rdi) \n"
//         STORE_INSTR" %%ymm14, 14*32(%%rdi) \n"
//         STORE_INSTR" %%ymm15, 15*32(%%rdi) \n"
//         "clwb 14*32(%%rdi) \n"
//         STORE_INSTR" %%ymm16, 16*32(%%rdi) \n"
//         STORE_INSTR" %%ymm17, 17*32(%%rdi) \n"
//         "clwb 16*32(%%rdi) \n"
//         STORE_INSTR" %%ymm18, 18*32(%%rdi) \n"
//         STORE_INSTR" %%ymm19, 19*32(%%rdi) \n"
//         "clwb 18*32(%%rdi) \n"
//         STORE_INSTR" %%ymm20, 20*32(%%rdi) \n"
//         STORE_INSTR" %%ymm21, 21*32(%%rdi) \n"
//         "clwb 20*32(%%rdi) \n"
//         STORE_INSTR" %%ymm22, 22*32(%%rdi) \n"
//         STORE_INSTR" %%ymm23, 23*32(%%rdi) \n"
//         "clwb 22*32(%%rdi) \n"
//         STORE_INSTR" %%ymm24, 24*32(%%rdi) \n"
//         STORE_INSTR" %%ymm25, 25*32(%%rdi) \n"
//         "clwb 24*32(%%rdi) \n"
//         STORE_INSTR" %%ymm26, 26*32(%%rdi) \n"
//         STORE_INSTR" %%ymm27, 27*32(%%rdi) \n"
//         "clwb 26*32(%%rdi) \n"
//         STORE_INSTR" %%ymm28, 28*32(%%rdi) \n"
//         STORE_INSTR" %%ymm29, 29*32(%%rdi) \n"
//         "clwb 28*32(%%rdi) \n"
//         STORE_INSTR" %%ymm30, 30*32(%%rdi) \n"
//         STORE_INSTR" %%ymm31, 31*32(%%rdi) \n"
//         "clwb 30*32(%%rdi) \n"
//         :
//         : [dest] "r" (dest)
//         : "rdi", YMM_32_REG
//     );
// }

/******************************************************
 * just ntstore
 *****************************************************/

static force_inline void _just_16b_ntstore(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        NTSTORE_INSTR" %%xmm0, 0*16(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", "xmm0"
    );
}

static force_inline void _just_32b_ntstore(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_1_REG
    );
}

static force_inline void _just_2x32b_ntstore(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_2_REG
    );
}

static force_inline void _just_4x32b_ntstore(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_4_REG
    );
}

static force_inline void _just_8x32b_ntstore(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm4, 4*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm5, 5*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm6, 6*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm7, 7*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_8_REG
    );
}

static force_inline void _just_16x32b_ntstore(char* dest) {
    asm volatile(
        "mov %[dest], %%rdi \n"
        NTSTORE_INSTR" %%ymm0,   0*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm1,   1*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm2,   2*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm3,   3*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm4,   4*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm5,   5*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm6,   6*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm7,   7*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm8,   8*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm9,   9*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm10, 10*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm11, 11*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm12, 12*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm13, 13*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm14, 14*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm15, 15*32(%%rdi) \n"
        :
        : [dest] "r" (dest)
        : "rdi", YMM_16_REG
    );
}

// static force_inline void _just_32x32b_ntstore(char* dest) {
//     asm volatile(
//         "mov %[dest], %%rdi \n"
//         NTSTORE_INSTR" %%ymm0,   0*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm1,   1*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm2,   2*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm3,   3*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm4,   4*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm5,   5*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm6,   6*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm7,   7*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm8,   8*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm9,   9*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm10, 10*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm11, 11*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm12, 12*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm13, 13*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm14, 14*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm15, 15*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm16, 16*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm17, 17*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm18, 18*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm19, 19*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm20, 20*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm21, 21*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm22, 22*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm23, 23*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm24, 24*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm25, 25*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm26, 26*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm27, 27*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm28, 28*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm29, 29*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm30, 30*32(%%rdi) \n"
//         NTSTORE_INSTR" %%ymm31, 31*32(%%rdi) \n"
//         :
//         : [dest] "r" (dest)
//         : "rdi", YMM_32_REG
//     );
// }

#endif // HLFS_UTIL_INSTR_JUST_H_
