#ifndef HLFS_UTIL_INSTR_H_
#define HLFS_UTIL_INSTR_H_

#include "util/common.h"

#define YMM_1_REG "ymm0"
#define YMM_2_REG "ymm0", "ymm1"
#define YMM_4_REG "ymm0", "ymm1", "ymm2", "ymm3"
#define YMM_8_REG "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7"
#define YMM_16_REG "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", \
                  "ymm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13", "ymm14", "ymm15"
// #define YMM_32_REG  "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", \
//                     "ymm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13", "ymm14", "ymm15", \
//                     "ymm16", "ymm17", "ymm18", "ymm19", "ymm20", "ymm21", "ymm22", "ymm23", \
//                     "ymm24", "ymm25", "ymm26", "ymm27", "ymm28", "ymm29", "ymm30", "ymm31"

#define LOAD_INSTR "vmovdqa"
#define NTLOAD_INSTR "vmovntdqa"
#define STORE_INSTR "vmovdqa"
#define NTSTORE_INSTR "vmovntpd"

// movsb 一个字节-1b
// MOVSW 1字-2b
// MOVSD 2字-4b
// MOVNTQ 4字-8b
// MOVNTDQ 双四字-16b

/******************************************************
 * load_and_store
 *****************************************************/

static force_inline void cpy_16b_load_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*16(%%rsi), %%xmm0 \n"
        STORE_INSTR" %%xmm0, 0*16(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", "xmm0"
    );
}

static force_inline void cpy_32b_load_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_1_REG
    );
}

static force_inline void cpy_2x32b_load_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_2_REG
    );
}

static force_inline void cpy_4x32b_load_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        LOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        LOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        STORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_4_REG
    );
}

static force_inline void cpy_8x32b_load_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        LOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        LOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        LOAD_INSTR" 4*32(%%rsi), %%ymm4 \n"
        LOAD_INSTR" 5*32(%%rsi), %%ymm5 \n"
        LOAD_INSTR" 6*32(%%rsi), %%ymm6 \n"
        LOAD_INSTR" 7*32(%%rsi), %%ymm7 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        STORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        STORE_INSTR" %%ymm4, 4*32(%%rdi) \n"
        STORE_INSTR" %%ymm5, 5*32(%%rdi) \n"
        STORE_INSTR" %%ymm6, 6*32(%%rdi) \n"
        STORE_INSTR" %%ymm7, 7*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_8_REG
    );
}

static force_inline void cpy_16x32b_load_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
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
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_16_REG
    );
}

// static force_inline void cpy_32x32b_load_store_noflush(char* dest, const char* src) {
//     asm volatile(
//         "mov %[src], %%rsi \n"
//         "mov %[dest], %%rdi \n"
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
//         : [src] "r" (src), [dest] "r" (dest)
//         : "rsi", "rdi", YMM_32_REG
//     );
// }

static force_inline void cpy_2x32b_load_store_flush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        "clwb 0*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_2_REG
    );
}

static force_inline void cpy_4x32b_load_store_flush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        LOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        LOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        "clwb 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        "clwb 2*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_4_REG
    );
}

static force_inline void cpy_8x32b_load_store_flush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        LOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        LOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        LOAD_INSTR" 4*32(%%rsi), %%ymm4 \n"
        LOAD_INSTR" 5*32(%%rsi), %%ymm5 \n"
        LOAD_INSTR" 6*32(%%rsi), %%ymm6 \n"
        LOAD_INSTR" 7*32(%%rsi), %%ymm7 \n"
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
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_8_REG
    );
}

static force_inline void cpy_16x32b_load_store_flush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
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
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_16_REG
    );
}

// static force_inline void cpy_32x32b_load_store_flush(char* dest, const char* src) {
//     asm volatile(
//         "mov %[src], %%rsi \n"
//         "mov %[dest], %%rdi \n"
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
//         : [src] "r" (src), [dest] "r" (dest)
//         : "rsi", "rdi", YMM_32_REG
//     );
// }

/******************************************************
 * ntload_and_store
 *****************************************************/

static force_inline void cpy_16b_ntload_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*16(%%rsi), %%xmm0 \n"
        STORE_INSTR" %%xmm0, 0*16(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", "xmm0"
    );
}

static force_inline void cpy_32b_ntload_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_1_REG
    );
}

static force_inline void cpy_2x32b_ntload_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_2_REG
    );
}

static force_inline void cpy_4x32b_ntload_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        NTLOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        NTLOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        STORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_4_REG
    );
}

static force_inline void cpy_8x32b_ntload_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        NTLOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        NTLOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        NTLOAD_INSTR" 4*32(%%rsi), %%ymm4 \n"
        NTLOAD_INSTR" 5*32(%%rsi), %%ymm5 \n"
        NTLOAD_INSTR" 6*32(%%rsi), %%ymm6 \n"
        NTLOAD_INSTR" 7*32(%%rsi), %%ymm7 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        STORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        STORE_INSTR" %%ymm4, 4*32(%%rdi) \n"
        STORE_INSTR" %%ymm5, 5*32(%%rdi) \n"
        STORE_INSTR" %%ymm6, 6*32(%%rdi) \n"
        STORE_INSTR" %%ymm7, 7*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_8_REG
    );
}

static force_inline void cpy_16x32b_ntload_store_noflush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
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
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_16_REG
    );
}

// static force_inline void cpy_32x32b_ntload_store_noflush(char* dest, const char* src) {
//     asm volatile(
//         "mov %[src], %%rsi \n"
//         "mov %[dest], %%rdi \n"
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
//         : [src] "r" (src), [dest] "r" (dest)
//         : "rsi", "rdi", YMM_32_REG
//     );
// }

static force_inline void cpy_2x32b_ntload_store_flush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        "clwb 0*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_2_REG
    );
}

static force_inline void cpy_4x32b_ntload_store_flush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        NTLOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        NTLOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        STORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        "clwb 0*32(%%rdi) \n"
        STORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        STORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        "clwb 2*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_4_REG
    );
}

static force_inline void cpy_8x32b_ntload_store_flush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        NTLOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        NTLOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        NTLOAD_INSTR" 4*32(%%rsi), %%ymm4 \n"
        NTLOAD_INSTR" 5*32(%%rsi), %%ymm5 \n"
        NTLOAD_INSTR" 6*32(%%rsi), %%ymm6 \n"
        NTLOAD_INSTR" 7*32(%%rsi), %%ymm7 \n"
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
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_8_REG
    );
}

static force_inline void cpy_16x32b_ntload_store_flush(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
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
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_16_REG
    );
}

// static force_inline void cpy_32x32b_ntload_store_flush(char* dest, const char* src) {
//     asm volatile(
//         "mov %[src], %%rsi \n"
//         "mov %[dest], %%rdi \n"
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
//         : [src] "r" (src), [dest] "r" (dest)
//         : "rsi", "rdi", YMM_32_REG
//     );
// }

/******************************************************
 * load_and_ntstore
 *****************************************************/

static force_inline void cpy_16b_load_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*16(%%rsi), %%xmm0 \n"
        NTSTORE_INSTR" %%xmm0, 0*16(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", "xmm0"
    );
}

static force_inline void cpy_32b_load_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_1_REG
    );
}

static force_inline void cpy_2x32b_load_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_2_REG
    );
}

static force_inline void cpy_4x32b_load_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        LOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        LOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_4_REG
    );
}

static force_inline void cpy_8x32b_load_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        LOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        LOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        LOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        LOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        LOAD_INSTR" 4*32(%%rsi), %%ymm4 \n"
        LOAD_INSTR" 5*32(%%rsi), %%ymm5 \n"
        LOAD_INSTR" 6*32(%%rsi), %%ymm6 \n"
        LOAD_INSTR" 7*32(%%rsi), %%ymm7 \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm4, 4*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm5, 5*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm6, 6*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm7, 7*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_8_REG
    );
}

static force_inline void cpy_16x32b_load_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
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
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_16_REG
    );
}

// static force_inline void cpy_32x32b_load_ntstore(char* dest, const char* src) {
//     asm volatile(
//         "mov %[src], %%rsi \n"
//         "mov %[dest], %%rdi \n"
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
//         NTSTORE_INSTR" %%ymm16, 16*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm17, 17*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm18, 18*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm19, 19*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm20, 20*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm21, 21*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm22, 22*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm23, 23*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm24, 24*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm25, 25*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm26, 26*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm27, 27*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm28, 28*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm29, 29*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm30, 30*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm31, 31*32(%%rsi) \n"
//         :
//         : [src] "r" (src), [dest] "r" (dest)
//         : "rsi", "rdi", YMM_32_REG
//     );
// }

/******************************************************
 * ntload_and_ntstore
 *****************************************************/

static force_inline void cpy_16b_ntload_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*16(%%rsi), %%xmm0 \n"
        NTSTORE_INSTR" %%xmm0, 0*16(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", "xmm0"
    );
}

static force_inline void cpy_32b_ntload_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_1_REG
    );
}

static force_inline void cpy_2x32b_ntload_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_2_REG
    );
}

static force_inline void cpy_4x32b_ntload_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        NTLOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        NTLOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_4_REG
    );
}

static force_inline void cpy_8x32b_ntload_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
        NTLOAD_INSTR" 0*32(%%rsi), %%ymm0 \n"
        NTLOAD_INSTR" 1*32(%%rsi), %%ymm1 \n"
        NTLOAD_INSTR" 2*32(%%rsi), %%ymm2 \n"
        NTLOAD_INSTR" 3*32(%%rsi), %%ymm3 \n"
        NTLOAD_INSTR" 4*32(%%rsi), %%ymm4 \n"
        NTLOAD_INSTR" 5*32(%%rsi), %%ymm5 \n"
        NTLOAD_INSTR" 6*32(%%rsi), %%ymm6 \n"
        NTLOAD_INSTR" 7*32(%%rsi), %%ymm7 \n"
        NTSTORE_INSTR" %%ymm0, 0*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm1, 1*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm2, 2*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm3, 3*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm4, 4*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm5, 5*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm6, 6*32(%%rdi) \n"
        NTSTORE_INSTR" %%ymm7, 7*32(%%rdi) \n"
        :
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_8_REG
    );
}

static force_inline void cpy_16x32b_ntload_ntstore(char* dest, const char* src) {
    asm volatile(
        "mov %[src], %%rsi \n"
        "mov %[dest], %%rdi \n"
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
        : [src] "r" (src), [dest] "r" (dest)
        : "rsi", "rdi", YMM_16_REG
    );
}

// static force_inline void cpy_32x32b_ntload_ntstore(char* dest, const char* src) {
//     asm volatile(
//         "mov %[src], %%rsi \n"
//         "mov %[dest], %%rdi \n"
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
//         NTSTORE_INSTR" %%ymm16, 16*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm17, 17*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm18, 18*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm19, 19*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm20, 20*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm21, 21*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm22, 22*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm23, 23*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm24, 24*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm25, 25*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm26, 26*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm27, 27*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm28, 28*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm29, 29*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm30, 30*32(%%rsi) \n"
//         NTSTORE_INSTR" %%ymm31, 31*32(%%rsi) \n"
//         :
//         : [src] "r" (src), [dest] "r" (dest)
//         : "rsi", "rdi", YMM_32_REG
//     );
// }

#endif // HLFS_UTIL_INSTR_H_
