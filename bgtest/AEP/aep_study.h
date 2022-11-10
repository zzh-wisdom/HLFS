#include "util/aep.h"
#include "util/aep_just.h"
#include "util/cpu.h"

// read
// write
// copy (pmem2)

typedef uint64_t (*aep_op_fn)(char *dest, const char *src, size_t len);

extern pmem2_memcpy_fn pmdk_memcpy;
extern pmem2_memset_fn pmdk_memset;
extern pmem2_drain_fn pmdk_drain;

/******************************************************
 * read
 *****************************************************/

static force_inline uint64_t aep_read_baseline(char* , const char* addr, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}
static force_inline uint64_t aep_just_load(char* , const char* addr, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    _just_load(addr, len);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}
static force_inline uint64_t aep_just_ntload(char* , const char* addr, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    _just_ntload(addr, len);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}
static force_inline uint64_t aep_load_to_dram(char* dram, const char* aep, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_load_store_noflush(dram, aep, len);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}
static force_inline uint64_t aep_ntload_to_dram(char* dram, const char* aep, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_ntload_store_noflush(dram, aep, len);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_native_cpy_to_dram(char* dram, const char* aep, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_native_noflush(dram, aep, len);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_pmem2_cpy_to_dram(char* dram, const char* aep, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    pmdk_memcpy(dram, aep, len, PMEM2_F_MEM_NOFLUSH);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

aep_op_fn aep_read_funcs[] = {
    aep_read_baseline,
    aep_just_load,
    aep_just_ntload,
    // aep_load_to_dram,  // avx不支持
    // aep_ntload_to_dram,

    // aep_native_cpy_to_dram,  // 一样的性能
    // aep_pmem2_cpy_to_dram,
};

const char* aep_read_func_labels[] = {
    "read_baseline",
    "just_load",
    "just_ntload",
    // "load_to_dram",
    // "ntload_to_dram",
    // "native_cpy_to_dram",
    // "pmem2_cpy_to_dram",
};

const int AEP_READ_FUNC_NUM = sizeof(aep_read_funcs) / sizeof(aep_op_fn);

/******************************************************
 * write
 *****************************************************/

static force_inline uint64_t aep_write_baseline(char* addr, const char* , size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}
static force_inline uint64_t aep_just_store_flush_by_all(char* addr, const char* , size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    _just_store_noflush(addr, len);
    clwb_extent(addr, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}
static force_inline uint64_t aep_just_store_flush_by_cacheline(char* addr, const char* , size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    _just_store_flush(addr, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}
static force_inline uint64_t aep_just_ntstore(char* addr, const char* , size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    _just_ntstore(addr, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}
static force_inline uint64_t aep_store_from_dram_flush_by_all(char* aep, const char* dram, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_load_store_noflush(aep, dram, len);
    clwb_extent(aep, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}
static force_inline uint64_t aep_store_from_dram_flush_by_cacheline(char* aep, const char* dram, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_load_store_flush(aep, dram, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}
static force_inline uint64_t aep_ntstore_from_dram(char* aep, const char* dram, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_load_ntstore(aep, dram, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_native_cpy_dram_by_all(char* aep_dest, const char* dram, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_native_noflush(aep_dest, dram, len);
    clwb_extent(aep_dest, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_native_cpy_dram_by_cacheline(char* aep_dest, const char* dram, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_native_flush(aep_dest, dram, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_pmem2_cpy_dram_com(char* aep, const char* dram, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    pmdk_memcpy(aep, dram, len, 0);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_pmem2_cpy_dram_t(char* aep, const char* dram, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    pmdk_memcpy(aep, dram, len, PMEM2_F_MEM_TEMPORAL);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_pmem2_cpy_dram_nt(char* aep, const char* dram, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    pmdk_memcpy(aep, dram, len, PMEM2_F_MEM_NONTEMPORAL);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

aep_op_fn aep_write_funcs[] = {
    aep_write_baseline,
    // aep_just_store_flush_by_all,      // 比不过pmdk
    // aep_just_store_flush_by_cacheline,
    // aep_just_ntstore,
    // aep_store_from_dram_flush_by_all, // malloc出来的内存，不能用向量指令。
    // aep_store_from_dram_flush_by_cacheline,
    // aep_ntstore_from_dram,
    // aep_native_cpy_dram_by_all,
    // aep_native_cpy_dram_by_cacheline,
    // aep_pmem2_cpy_dram_com,
    aep_pmem2_cpy_dram_t,
    aep_pmem2_cpy_dram_nt,
};

const char* aep_write_func_labels[] = {
    "write_baseline",
    // "just_store_flush_by_all",
    // "just_store_flush_by_cacheline",
    // "just_ntstore",

    // "store_from_dram_flush_by_all", // avx指令不支持malloc的内存
    // "store_from_dram_flush_by_cacheline",
    // "ntstore_from_dram",
    // "native_cpy_dram_by_all",
    // "native_cpy_dram_by_cacheline",
    // "pmem2_cpy_dram_com",
    "pmem2_cpy_dram_t",
    "pmem2_cpy_dram_nt",
};

const int AEP_WRITE_FUNC_NUM = sizeof(aep_write_funcs) / sizeof(aep_op_fn);

/******************************************************
 * copy aep to aep
 *****************************************************/

static force_inline uint64_t aep_cpy_baseline(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_cpy_native_by_all(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_native_noflush(aep_dest, aep_src, len);
    clwb_extent(aep_dest, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_cpy_native_by_cacheline(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_native_flush(aep_dest, aep_src, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_cpy_load_store_by_all(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_load_store_noflush(aep_dest, aep_src, len);
    clwb_extent(aep_dest, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_cpy_load_store_by_cacheline(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_load_store_flush(aep_dest, aep_src, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_cpy_load_ntstore(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_load_ntstore(aep_dest, aep_src, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_cpy_ntload_store_by_all(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_ntload_store_noflush(aep_dest, aep_src, len);
    clwb_extent(aep_dest, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_cpy_ntload_store_by_cacheline(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_ntload_store_flush(aep_dest, aep_src, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_cpy_ntload_ntstore(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    cpy_ntload_ntstore(aep_dest, aep_src, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

// 会根据阈值来选择 默认是256
static force_inline uint64_t aep_cpy_pmem2_memcpy_com(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    pmdk_memcpy(aep_dest, aep_src, len, 0);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_cpy_pmem2_memcpy_t(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    pmdk_memcpy(aep_dest, aep_src, len, PMEM2_F_MEM_TEMPORAL);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_cpy_pmem2_memcpy_nt(char* aep_dest, const char* aep_src, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    pmdk_memcpy(aep_dest, aep_src, len, PMEM2_F_MEM_NONTEMPORAL);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

aep_op_fn aep_cpy_funcs[] = {
    aep_cpy_baseline,
    // aep_cpy_native_by_all,
    // aep_cpy_native_by_cacheline,

    // aep_cpy_load_store_by_all,
    // aep_cpy_load_store_by_cacheline,
    // aep_cpy_load_ntstore,
    // aep_cpy_ntload_store_by_all,
    // aep_cpy_ntload_store_by_cacheline,
    // aep_cpy_ntload_ntstore,
    aep_cpy_pmem2_memcpy_com,
    aep_cpy_pmem2_memcpy_t,
    aep_cpy_pmem2_memcpy_nt,
};

const char* aep_cpy_func_labels[] = {
    "cpy_baseline",
    // "cpy_native_by_all",
    // "cpy_native_by_cacheline",
    // "cpy_load_store_by_all",
    // "cpy_load_store_by_cacheline",
    // "cpy_load_ntstore",
    // "cpy_ntload_store_by_all",
    // "cpy_ntload_store_by_cacheline",
    // "cpy_ntload_ntstore",
    "cpy_pmem2_memcpy_com",
    "cpy_pmem2_memcpy_t",
    "cpy_pmem2_memcpy_nt",
};

const int AEP_CPY_FUNC_NUM = sizeof(aep_cpy_funcs) / sizeof(aep_op_fn);

/******************************************************
 * memset aep
 *****************************************************/

static force_inline uint64_t aep_memset_baseline(char* aep_dest, const char* c, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_memset_native_by_all(char* aep_dest, const char* c, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    memset_native_noflush(aep_dest, *c, len);
    clwb_extent(aep_dest, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_memset_native_by_cacheline(char* aep_dest, const char* c, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    memset_native_flush(aep_dest, *c, len);
    sfence();

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_memset_pmem2_com(char* aep_dest, const char* c, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    pmdk_memset(aep_dest, *c, len, 0);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_memset_pmem2_t(char* aep_dest, const char* c, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    pmdk_memset(aep_dest, *c, len, PMEM2_F_MEM_TEMPORAL);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

static force_inline uint64_t aep_memset_pmem2_nt(char* aep_dest, const char* c, size_t len) {
    uint64_t start = GetTsNsec();
    barrier();

    pmdk_memset(aep_dest, *c, len, PMEM2_F_MEM_NONTEMPORAL);

    barrier();
    uint64_t end = GetTsNsec();
    return end - start;
}

aep_op_fn aep_memset_funcs[] = {
    aep_memset_baseline,
    // aep_memset_native_by_all,
    // aep_memset_native_by_cacheline,
    aep_memset_pmem2_com,
    aep_memset_pmem2_t,
    aep_memset_pmem2_nt,
};

const char* aep_memset_func_labels[] = {
    "aep_memset_baseline",
    // "aep_memset_native_by_all",
    // "aep_memset_native_by_cacheline",
    "aep_memset_pmem2_com",
    "aep_memset_pmem2_t",
    "aep_memset_pmem2_nt",
};

const int AEP_MEMSET_FUNC_NUM = sizeof(aep_memset_funcs) / sizeof(aep_op_fn);