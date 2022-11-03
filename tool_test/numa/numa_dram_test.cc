#include <iostream>
#include <assert.h>
#include <sys/time.h>
#include <numa.h>

using namespace std;

int buffer_size = 0;
int total_access = 0;
int64_t* buffer = nullptr;

static inline uint64_t get_now_micros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000000 + tv.tv_usec;
}

void numa_bind_on_socket_strict(int socket) {
    bitmask* node_mask = numa_get_mems_allowed();
    numa_bitmask_clearall(node_mask);
    numa_bitmask_setbit(node_mask, socket);
    numa_bind(node_mask);
    numa_bitmask_free(node_mask);
    numa_set_bind_policy(1);
    // numa_set_strict();
    // numa_run_on_node(socket);
    // numa_set_membind()
    node_mask = numa_get_membind();
    assert(numa_bitmask_weight(node_mask) == 1);
    assert(numa_bitmask_isbitset(node_mask, socket));
}

// 1: buffer大小，单位K
// 2: 读取总量，单位K
// numactl --cpubind=0 --membind=1 ./bin/numa_dram_test 1024 1024
int main(int argc, char *argv[]) {
    buffer_size = atoi(argv[1]) * 1024;
    total_access = atoi(argv[2]) * 1024;
    printf("buffer_size=%dk, total_access=%dk\n", buffer_size, total_access);

    numa_bind_on_socket_strict(1);

    buffer = (int64_t*)malloc(buffer_size);
    assert(buffer);
    for(int i = 0; i < buffer_size/sizeof(int64_t); ++i) {
        buffer[i] = 1;
    }

    uint64_t start_ts = 0;
    uint64_t end_ts = 0;
    const char* label = nullptr;
    uint64_t sum = 0;

    label = "seq write";
    start_ts = get_now_micros();
    for(int i = 0; i < total_access/sizeof(int64_t); ++i) {
        buffer[i] = 1;
    }
    end_ts = get_now_micros();
    printf("%s\t %lu us\n", label, end_ts - start_ts);

    label = "seq read";
    start_ts = get_now_micros();
    sum = 0;
    for(int i = 0; i < total_access/sizeof(int64_t); ++i) {
        sum += buffer[i];
    }
    end_ts = get_now_micros();
    printf("%s\t %lu us\n", label, end_ts - start_ts);
    assert(sum == total_access/sizeof(int64_t));

    ///
    int step = 32;

    label = "step32 write";
    start_ts = get_now_micros();
    for(int base = 0; base < step; ++base) {
        for(int i = base; i < total_access/sizeof(int64_t); i += step) {
            buffer[i] = 1;
        }
    }
    end_ts = get_now_micros();
    printf("%s\t %lu us\n", label, end_ts - start_ts);

    label = "step32 read";
    start_ts = get_now_micros();
    sum = 0;
    for(int base = 0; base < step; ++base) {
        for(int i = base; i < total_access/sizeof(int64_t); i += step) {
            sum += buffer[i];
        }
    }
    end_ts = get_now_micros();
    printf("%s\t %lu us\n", label, end_ts - start_ts);
    assert(sum == total_access/sizeof(int64_t));

    free(buffer);
    return 0;
}

// $ numactl --cpubind=1 --membind=0 ./bin/numa_dram_test 10240 10240
// buffer_size=10485760k, total_access=10485760k
// seq write        4504 us
// seq read         4346 us
// step32 write     18488 us
// step32 read      8919 us

// zzh@s88:~/HLFS/bgtest$ numactl --cpubind=1 --membind=1 ./bin/numa_dram_test 10240 10240
// buffer_size=10485760k, total_access=10485760k
// seq write        4489 us
// seq read         4345 us
// step32 write     18468 us
// step32 read      8948 us

/// O3优化后

// zzh@s88:~/HLFS/bgtest$ numactl --cpubind=1 --membind=0 ./bin/numa_dram_test 10240 10240
// buffer_size=10485760k, total_access=10485760k
// seq write        807 us
// seq read         760 us
// step32 write     11779 us
// step32 read      7241 us

// zzh@s88:~/HLFS/bgtest$ numactl --cpubind=1 --membind=1 ./bin/numa_dram_test 10240 10240
// buffer_size=10485760k, total_access=10485760k
// seq write        778 us
// seq read         753 us
// step32 write     11775 us
// step32 read      7243 us



// zzh@s88:~/HLFS/bgtest$ numactl --cpubind=1 --membind=0 ./bin/numa_dram_test 1024000 1024000
// buffer_size=1048576000k, total_access=1048576000k
// seq write        136277 us
// seq read         109877 us
// step32 write     2719413 us
// step32 read      1811980 us

// zzh@s88:~/HLFS/bgtest$ numactl --cpubind=1 --membind=1 ./bin/numa_dram_test 1024000 1024000
// buffer_size=1048576000k, total_access=1048576000k
// seq write        99234 us
// seq read         69183 us
// step32 write     1831839 us
// step32 read      1055288 us

