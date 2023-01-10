#include <stdio.h>

#include "util/log.h"
#include "util/aep.h"
#include "util/cpu.h"

const uint64_t BUFFER_SIZE = 512ul << 20; // 2 GB
int bs;

static inline void aep_native_store_dram(char* aep, const char* dram, size_t len) {
    uint64_t* dst = (uint64_t*)aep;
    const uint64_t* src = (const uint64_t*)dram;
    for(int i = 0; i < len; i += sizeof(uint64_t)) {
        dst[i] = src[i];
    }
    clwb_extent(aep, len);
    sfence();
}

int main(int argc, char* argv[]) {
    LogCfg log_cfg = {
        .is_log_to_stderr = true,
        .vlog_level = 3,
        .min_log_level = 0,
    };
    InitLog(argv[0], &log_cfg);
    CoreBind(pthread_self(), 1);

    log_assert(argc == 2);
    bs = atoi(argv[1]);
    uint64_t OP = BUFFER_SIZE / bs;
    printf("buffer size: %lu MB, bs: %d, op: %lu\n", BUFFER_SIZE >> 20, bs, OP);

    char* dst_buf = (char*)aligned_alloc(4096, BUFFER_SIZE);
    char* src_buf = (char*)aligned_alloc(4096, bs);
    uint64_t start_ns, end_ns, total_ns;

    start_ns = GetTsNsec();
    for(int i = 0; i < OP; ++i) {
        aep_native_store_dram(dst_buf + i * bs, src_buf, bs);
    }
    end_ns = GetTsNsec();
    total_ns = end_ns - start_ns;
    printf("lat: %0.2lf\n", total_ns*1.0 / OP);
    return 0;
}