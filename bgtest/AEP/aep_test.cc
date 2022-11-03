#include <assert.h>
#include <gflags/gflags.h>
#include <numa.h>

#include <iostream>
#include <cstdint>
#include <unordered_map>

#include "util/cpu.h"

using namespace std;

enum RwType {
    kRead = 0,
    kWrite = 1,
};
#define RW_TYPE_READ "read"
#define RW_TYPE_WRITE "write"
static unordered_map<RwType, string> rw_type_2_str = {
    {kRead, RW_TYPE_READ},
    {kWrite, RW_TYPE_WRITE},
};
static unordered_map<string, RwType> str_2_rw_type = {
    {RW_TYPE_READ, kRead},
    {RW_TYPE_WRITE, kWrite},
};

inline const string& RwType2Str(RwType rw_type) { return rw_type_2_str[rw_type]; }
inline RwType Str2RwType(const string& type) { return str_2_rw_type[type]; };

enum AccessMode {
    kSeq = 0,
    kRandom = 1,
};
#define ACCESS_MODE_SEQ "seq"
#define ACCESS_MODE_RANDOM "random"
static unordered_map<AccessMode, string> access_mode_2_str = {
    {kSeq, ACCESS_MODE_SEQ},
    {kRandom, ACCESS_MODE_RANDOM},
};
static unordered_map<string, AccessMode> str_2_access_mode = {
    {ACCESS_MODE_SEQ, kSeq},
    {ACCESS_MODE_RANDOM, kRandom},
};

inline const string& AccessMode2Str(AccessMode mode) { return access_mode_2_str[mode]; }
inline AccessMode Str2AccessMode(const string& mode) { return str_2_access_mode[mode]; };

const int MAX_THREAD_NUM = 64;
const int MAX_CORE_ID = 79;

string aep_dev_path = "/dev/dax1.0";
uint32_t thread_num = 1;
uint64_t buffer_size = 1024;
RwType rw_type = kRead;
AccessMode access_mode = kSeq;
uint32_t access_size= 8;
uint32_t runtime = 5;
uint32_t socket = 1;
uint32_t core_ids[MAX_THREAD_NUM];
uint32_t core_num = 1;

DEFINE_string(aep_dev_path, "/dev/dax1.0", "AEP char device path");
DEFINE_uint32(thread_num, 1, "thread number <= 64");
DEFINE_uint64(buffer_size, 1024, "buffer size for each thread, unit B");
DEFINE_string(rw_type, RW_TYPE_READ, "read/write");
DEFINE_string(access_mode, ACCESS_MODE_SEQ, "seq/random");
DEFINE_uint32(access_size, 8, "access size, unit 8 B");
DEFINE_uint32(runtime, 5, "runtime, unit s");
DEFINE_uint32(socket, 1, "cpu socket id, 0/1/2...");
DEFINE_string(core_ids, "20", "core id(<80) to bind each thread, should in the same socket with aep");

static bool ValidateThreadNum(const char* flag_name, const uint32_t value) {
    if(value < MAX_THREAD_NUM) return true;
    printf("Invalid value for --%s: %u\n", flag_name, value);
    return false;
}
static bool ValidateRwType(const char* flag_name, const string& value) {
    auto it = str_2_rw_type.find(value);
    if(it != str_2_rw_type.end()) return true;
    printf("Invalid value for --%s: %s\n", flag_name, value.c_str());
    return false;
}
static bool ValidateAccessMode(const char* flag_name, const string& value) {
    auto it = str_2_access_mode.find(value);
    if(it != str_2_access_mode.end()) return true;
    printf("Invalid value for --%s: %s\n", flag_name, value.c_str());
    return false;
}
static bool ValidateCoreIds(const char* flag_name, const string& value) {
    core_num = 0;
    // string cur;
    // for(int i = 0; i < value.size(); ++i) {
    //     if(value[i] == ',') {
    //         if(cur.empty()) goto err;
    //         core_ids[core_num++] = stoi(cur.c_str());
    //         cur.clear();
    //     } else if(value[i] >= '0' && value[i] <= '9') {
    //         cur.push_back(value[i]);
    //     } else {
    //         goto err;
    //     }
    // }
    // if(!cur.empty()) {
    //     core_ids[core_num++] = stoi(cur.c_str());
    // }
    int max_cpus = numa_num_possible_cpus();
    bitmask* mask = numa_parse_cpustring(value.c_str());
    if(!mask) goto err;
    for (int i = 0; i < max_cpus; ++i) {
        if (numa_bitmask_isbitset(mask, i)) {
            if(i > MAX_CORE_ID) goto err;
            core_ids[core_num++] = i;
        }
    }
    numa_bitmask_free(mask);
    if(core_num == 0) goto err;
    return true;

err:
    printf("Invalid value for --%s: %s\n", flag_name, value.c_str());
    return false;
}


void ParseAndValidateFlags(int *argc, char ***argv) {
    gflags::SetUsageMessage("usage msg");
    gflags::RegisterFlagValidator(&FLAGS_thread_num, ValidateThreadNum);
    gflags::RegisterFlagValidator(&FLAGS_rw_type, ValidateRwType);
    gflags::RegisterFlagValidator(&FLAGS_access_mode, ValidateAccessMode);
    gflags::RegisterFlagValidator(&FLAGS_core_ids, ValidateCoreIds);
    gflags::ParseCommandLineFlags(argc, argv, true);
}

void CheckCfg() {
    int max_node = numa_max_node();
    if(socket > max_node) {
        printf("socket %u should between [0-%d]\n", socket, max_node);
        exit(-1);
    }
    if(core_num != thread_num) {
        printf("core_num %u is not equal to thread_num %u\n", core_num, thread_num);
        exit(-1);
    }
    for(int i = 0; i < thread_num; ++i) {
        if(numa_node_of_cpu(core_ids[i]) != socket) {
            printf("cpu core %u is not on socket %u\n", core_ids[i], socket);
            exit(-1);
        }
    }
}

void SetEnv() {
    // numa set
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

void SetAndPrintCfg() {
    aep_dev_path = FLAGS_aep_dev_path;
    thread_num = FLAGS_thread_num;
    buffer_size = FLAGS_buffer_size;
    rw_type = Str2RwType(FLAGS_rw_type);
    access_mode = Str2AccessMode(FLAGS_access_mode);
    access_size = FLAGS_access_size;
    runtime = FLAGS_runtime;
    socket = FLAGS_socket;

    CheckCfg();
    SetEnv();

    printf("%-80s\n", "========================================================");
    printf("\taep_dev_path  = %s\n", aep_dev_path.c_str());
    printf("\tthread_num    = %u\n", thread_num);
    printf("\tbuffer_size   = %luB,%luKB,%luMB\n", buffer_size, buffer_size/1024, buffer_size/1024/1024);
    printf("\trw_type       = %s\n", RwType2Str(rw_type).c_str());
    printf("\taccess_mode   = %s\n", AccessMode2Str(access_mode).c_str());
    printf("\taccess_size   = %uB,%uKB,%uMB\n", access_size, access_size/1024, access_size/1024/1024);
    printf("\truntime       = %us\n", runtime);
    printf("\tsocket        = %u\n", socket);
    printf("\tcore_id       = %u", core_ids[0]);
    for(int i = 1; i < thread_num; ++i) {
        printf(",%u", core_ids[i]);
    }
    printf("\n");
    printf("%-80s\n", "--------------------------------------------------------");

}

struct ThreadContext {
    int thread_idx_;

    char* buf_;
    uint64_t buf_size_;
    uint32_t core_id_;
    uint32_t socket_;
    RwType rw_type_;
    AccessMode access_mode_;
    uint32_t access_size;
    uint32_t runtime;
};

ThreadContext thread_contexts[MAX_THREAD_NUM];
pthread_t threads[MAX_THREAD_NUM];

void ThreadContextInit() {
    for(int i = 0; i < thread_num; ++i) {
        ThreadContext& ctx = thread_contexts[i];
        ctx.thread_idx_ = i;
        ctx.buf_ = nullptr;
        ctx.buf_size_ = buffer_size;
        ctx.core_id_ = core_ids[i];
        ctx.socket_ = socket;
        ctx.rw_type_ = rw_type;
        ctx.access_mode_ = access_mode;
        ctx.access_size = access_size;
        ctx.runtime = runtime;
    }
}

void* ThreadJob(void *arg) {
    bool ret = 0;
    ThreadContext* ctx = (ThreadContext*)arg;
    ret= CoreBind(pthread_self(), ctx->core_id_);
    assert(ret);
    std::vector<int> core_binds = CoreBindQuery(pthread_self());
    assert(core_binds.size() == 1 && core_binds[0] == ctx->core_id_);
    printf("idx[%d] thread[%ld] bind => core [%u]\n",
        ctx->thread_idx_, pthread_self(), ctx->core_id_);
    while(1){
        ++ret;
    }
    return nullptr;
}

// ./aep_test --flagfile=../config/aep_test_flags
int main(int argc, char *argv[]) {
    ParseAndValidateFlags(&argc, &argv);
    SetAndPrintCfg();
    int ret;

    ThreadContextInit();
    for (int i = 0; i < thread_num; i++) {
        ret = pthread_create(&threads[i], nullptr, ThreadJob, (void *)(&thread_contexts[i]));
        assert(ret == 0);
    }
    for (int i = 0; i < thread_num; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
