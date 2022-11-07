#include <assert.h>
#include <gflags/gflags.h>
#include <numa.h>
#include <time.h>

#include <cstdint>
#include <iostream>
#include <unordered_map>

#include "util/aep.h"
#include "util/cpu.h"
#include "util/file.h"
#include "util/log.h"
#include "util/bitops.h"

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
uint32_t access_size = 16;
// runtime 弃用
uint32_t runtime = 5;
uint32_t iters = 1000000;
uint32_t socket = 1;
uint32_t core_ids[MAX_THREAD_NUM];
uint32_t core_num = 1;

string rep_dir_path = "/mnt/pmem2/aep_test";
const string rep_internal_dir = "internal";
const string rep_report_dir = "report";
string rep_file_prefix = "ape_test";
string rep_label_suffix = "label.txt";
string rep_data_suffix = "data.dat";
string rep_report_suffix = "report.txt";

uint64_t buf_slots = 0;
uint32_t buf_slots_mask = 0;
uint32_t seed;
struct pmem2_map* aep_map = nullptr;
char* ape_buf;
uint64_t ape_buf_size;

string Size2HumanStr(uint64_t size) {
    if (size >= 1024 * 1024 * 1024) {
        return to_string(size / (1024 * 1024 * 1024)) + "G";
    } else if (size >= 1024 * 1024) {
        return to_string(size / (1024 * 1024)) + "M";
    } else if (size >= 1024) {
        return to_string(size / (1024)) + "K";
    } else {
        return to_string(size) + "B";
    }
}

void PrintGlobalCfg() {
    printf("%-80s\n", "========================================================");
    printf("\taep_dev_path  = %s\n", aep_dev_path.c_str());
    printf("\tthread_num    = %u\n", thread_num);
    printf("\tbuffer_size   = %luB,%luKB,%luMB\n", buffer_size, buffer_size / 1024,
           buffer_size / 1024 / 1024);
    printf("\trw_type       = %s\n", RwType2Str(rw_type).c_str());
    printf("\taccess_mode   = %s\n", AccessMode2Str(access_mode).c_str());
    printf("\taccess_size   = %uB,%uKB,%uMB\n", access_size, access_size / 1024,
           access_size / 1024 / 1024);
    printf("\truntime       = %us\n", runtime);
    printf("\titers         = %us\n", iters);
    printf("\tsocket        = %u\n", socket);
    printf("\tcore_id       = %u", core_ids[0]);
    for (int i = 1; i < thread_num; ++i) {
        printf(",%u", core_ids[i]);
    }
    printf("\n");
    printf("\trep_dir_path   = %s\n", rep_dir_path.c_str());
    printf("\tape_buf_size   = %s\n", Size2HumanStr(ape_buf_size).c_str());
    printf("\tseed           = %u\n", seed);

    printf("%-80s\n", "--------------------------------------------------------");
}

void InitRepDir() {
    string internal_dir = rep_dir_path + "/" + rep_internal_dir;
    string report_dir = rep_dir_path + "/" + rep_report_dir;
    int ret;
    if (!FileExists(rep_dir_path)) {
        ret = CreateDir(rep_dir_path);
        log_assert(!ret);
        ret = CreateDir(internal_dir);
        log_assert(!ret);
        ret = CreateDir(report_dir);
        log_assert(!ret);
    } else {
        if (!FileExists(internal_dir)) {
            ret = CreateDir(internal_dir);
            log_assert(!ret);
        }
        if (!FileExists(report_dir)) {
            ret = CreateDir(report_dir);
            log_assert(!ret);
        }
    }
}

void CleanRepDir() {
    int ret;
    ret = RemoveDirRecursive(rep_dir_path);
    log_assert(!ret);
}

DEFINE_string(aep_dev_path, "/dev/dax1.0", "AEP char device path");
DEFINE_uint32(thread_num, 1, "thread number <= 64");
DEFINE_uint64(buffer_size, 1024, "buffer size for each thread, unit B");
DEFINE_string(rw_type, RW_TYPE_READ, "read/write");
DEFINE_string(access_mode, ACCESS_MODE_SEQ, "seq/random");
DEFINE_uint32(access_size, access_size, "access size, unit 8 B");
DEFINE_uint32(runtime, 5, "runtime, unit s");
DEFINE_uint32(iters, iters, "op count");
DEFINE_uint32(socket, 1, "cpu socket id, 0/1/2...");
DEFINE_string(core_ids, "20",
              "core id(<80) to bind each thread, should in the same socket with aep");

DEFINE_string(rep_dir_path, rep_dir_path, "AEP rep fs dir path");
DEFINE_bool(clean_rep_dir, false, "if clean report dir");

static bool ValidateThreadNum(const char* flag_name, const uint32_t value) {
    if (value < MAX_THREAD_NUM) return true;
    printf("Invalid value for --%s: %u\n", flag_name, value);
    return false;
}
static bool ValidateRwType(const char* flag_name, const string& value) {
    auto it = str_2_rw_type.find(value);
    if (it != str_2_rw_type.end()) return true;
    printf("Invalid value for --%s: %s\n", flag_name, value.c_str());
    return false;
}
static bool ValidateAccessMode(const char* flag_name, const string& value) {
    auto it = str_2_access_mode.find(value);
    if (it != str_2_access_mode.end()) return true;
    printf("Invalid value for --%s: %s\n", flag_name, value.c_str());
    return false;
}
static bool ValidateCoreIds(const char* flag_name, const string& value) {
    core_num = 0;
    int max_cpus = numa_num_possible_cpus();
    bitmask* mask = numa_parse_cpustring(value.c_str());
    if (!mask) goto err;
    for (int i = 0; i < max_cpus; ++i) {
        if (numa_bitmask_isbitset(mask, i)) {
            if (i > MAX_CORE_ID) goto err;
            core_ids[core_num++] = i;
        }
    }
    numa_bitmask_free(mask);
    if (core_num == 0) goto err;
    return true;

err:
    printf("Invalid value for --%s: %s\n", flag_name, value.c_str());
    return false;
}

static bool ValidateIters(const char* flag_name, uint32_t value) {
    if(value > 0) return true;
    printf("Invalid value for --%s: %u\n", flag_name, value);
    return false;
}

void ParseAndValidateFlags(int* argc, char*** argv) {
    gflags::SetUsageMessage("usage msg");
    gflags::RegisterFlagValidator(&FLAGS_thread_num, ValidateThreadNum);
    gflags::RegisterFlagValidator(&FLAGS_rw_type, ValidateRwType);
    gflags::RegisterFlagValidator(&FLAGS_access_mode, ValidateAccessMode);
    gflags::RegisterFlagValidator(&FLAGS_core_ids, ValidateCoreIds);
    gflags::RegisterFlagValidator(&iters, ValidateIters);
    gflags::ParseCommandLineFlags(argc, argv, true);
}

void CheckGlobalCfg() {
    int max_node = numa_max_node();
    if (socket > max_node) {
        printf("socket %u should between [0-%d]\n", socket, max_node);
        exit(-1);
    }
    if (core_num != thread_num) {
        printf("core_num %u is not equal to thread_num %u\n", core_num, thread_num);
        exit(-1);
    }
    for (int i = 0; i < thread_num; ++i) {
        if (numa_node_of_cpu(core_ids[i]) != socket) {
            printf("cpu core %u is not on socket %u\n", core_ids[i], socket);
            exit(-1);
        }
    }

    // some check
    log_assert(buffer_size*thread_num <= ape_buf_size);
    int buf_size_bits = __fls(buffer_size);
    log_assert((1UL << buf_size_bits) == buffer_size);
    int access_size_bits = __fls(access_size);
    log_assert((1UL << access_size_bits) == access_size);

    log_assert(buf_slots > 0);
    uint32_t buf_slots_bits = __fls(buf_slots);
    printf("buf_slots_bits=%u, buf_slots=%lu\n", buf_slots_bits, buf_slots);
    CHECK_EQ(1UL << buf_slots_bits, buf_slots);
}

void InitGlobalCfg() {
    aep_dev_path = FLAGS_aep_dev_path;
    thread_num = FLAGS_thread_num;
    buffer_size = FLAGS_buffer_size;
    rw_type = Str2RwType(FLAGS_rw_type);
    access_mode = Str2AccessMode(FLAGS_access_mode);
    access_size = FLAGS_access_size;
    runtime = FLAGS_runtime;
    iters = FLAGS_iters;
    socket = FLAGS_socket;
    // core_ids
    // core_num

    rep_dir_path = FLAGS_rep_dir_path;

    buf_slots = buffer_size / access_size;
    uint32_t buf_slots_bits = __fls(buf_slots);
    buf_slots_mask = (1UL << buf_slots_bits) - 1;
    seed = time(nullptr);

    log_assert(!aep_map);
    aep_map = Pmem2Map(aep_dev_path);
    log_assert(aep_map);
    ape_buf = (char*)pmem2_map_get_address(aep_map);
    log_assert(ape_buf);
    ape_buf_size = pmem2_map_get_size(aep_map);
}

void DestroyGlobalCfg() {
    Pmem2UnMap(&aep_map);
    aep_map = nullptr;
    ape_buf = nullptr;
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
    // numa_set_membind();
    node_mask = numa_get_membind();
    assert(numa_bitmask_weight(node_mask) == 1);
    assert(numa_bitmask_isbitset(node_mask, socket));

    srand((unsigned int)seed);

    InitRepDir();
}

const uint64_t MAX_IDX_SIZE = (40ULL << 20 >> 2);
// 每次测试：
// 然后从高到低加载/生成idx
// 跑负载
// L2 Cache 40MB, 最多可以放 1 千万个索引，idx刚好可以刷新L2

struct Task {
    int thread_idx_;
    uint32_t core_id_;
    uint32_t socket_;

    char* buf_;

    uint64_t buf_size_;
    RwType rw_type_;
    AccessMode access_mode_;
    uint32_t access_size_;
    uint32_t runtime_;
    uint32_t iters_;

    uint32_t* idx_;
    static const uint64_t mem_set_value = 0x3f3f3f3f3f3f3f3fUL;
    char* mem_cpy_buf_;

    Task(int thread_idx, char* buf) {
        this->thread_idx_ = thread_idx;
        this->core_id_ = core_ids[thread_idx];
        this->socket_ = socket;

        this->buf_ = buf;

        this->buf_size_ = buffer_size;
        this->rw_type_ = rw_type;
        this->access_mode_ = access_mode;
        this->access_size_ = access_size;
        this->runtime_ = runtime;
        this->iters_ = iters;

        // 保持在main线程中生成，这样当使用同样的seed，生成的随机数是一样的
        this->idx_ = (uint32_t*)malloc(MAX_IDX_SIZE*sizeof(uint32_t));
        log_assert(this->idx_);
        printf("buf_slots = %lu\n", buf_slots);
        for(int i = MAX_IDX_SIZE - 1; i >= 0; --i) {
            idx_[i] = rand() & buf_slots_mask;
            // LOG_EVERY_N(INFO, 500000) << idx_[i];
        }
        mem_cpy_buf_ = (char*)malloc(access_size);
        log_assert(mem_cpy_buf_);
    }
    ~Task() {
        free(idx_);
        free(mem_cpy_buf_);
    }
};

string GetLocalTime() {
    char buf[256];
    time_t now_time = time(nullptr);
    tm* tm_ = localtime(&now_time);              // 将time_t格式转换为tm结构体
    int year, month, day, hour, minute, second;  // 定义时间的各个int临时变量。
    year =
        tm_->tm_year +
        1900;  // 临时变量，年，由于tm结构体存储的是从1900年开始的时间，所以临时变量int为tm_year加上1900。
    month = tm_->tm_mon +
            1;  // 临时变量，月，由于tm结构体的月份存储范围为0-11，所以临时变量int为tm_mon加上1。
    day = tm_->tm_mday;    // 临时变量，日。
    hour = tm_->tm_hour;   // 临时变量，时。
    minute = tm_->tm_min;  // 临时变量，分。
    second = tm_->tm_sec;  // 临时变量，秒。
    sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d", year, month, day, hour, minute,
            second);  // 将年月日时分秒合并。
    string str(buf);  // 定义string变量，并将总日期时间char*变量作为构造函数的参数传入。
    return str;  // 返回转换日期时间后的string变量。
}

class Report {
    int label_fd_;
    char* result_;

    void CreateLabelFile(Task* task) {
        char buf[256];
        // buf size, rw_type_, AccessMode, access_size, run-time, thread_idx
        sprintf(buf, "%s-%s-%s-%s-%d", Size2HumanStr(task->buf_size_).c_str(),
                RwType2Str(task->rw_type_).c_str(), AccessMode2Str(task->access_mode_).c_str(),
                Size2HumanStr(task->access_size_).c_str(), task->thread_idx_);
        string file_time = GetLocalTime();
        string file_common_part = file_time + "-" + string(buf);
        string label_path = rep_dir_path + "/" + rep_internal_dir + "/" + rep_file_prefix + "-" +
                            file_common_part + "-" + rep_label_suffix;
        info << "open file: " << label_path;
        label_fd_ = OpenAndAllocAtSize(label_path, 0);
        if (label_fd_ < 0) {
            perror(label_path.c_str());
            exit(-1);
        }
        FILE* label_fp = fdopen(label_fd_, "w");
        fprintf(label_fp, "time: %s\n", file_time.c_str());
        fprintf(label_fp, "buffer_size: %s\n", Size2HumanStr(task->buf_size_).c_str());
        fprintf(label_fp, "rw_type: %s\n", RwType2Str(task->rw_type_).c_str());
        fprintf(label_fp, "access_mode: %s\n", AccessMode2Str(task->access_mode_).c_str());
        fprintf(label_fp, "access_size: %s\n", Size2HumanStr(task->access_size_).c_str());
        fprintf(label_fp, "runtime: %us\n", task->runtime_);
        fprintf(label_fp, "iters: %us\n", task->iters_);
        fprintf(label_fp, "thread idx: %d\n", task->thread_idx_);
        fprintf(label_fp, "core id: %u\n", task->core_id_);
        fprintf(label_fp, "socket: %u\n", task->socket_);

        fprintf(label_fp, "ape_buf_size: %s\n", Size2HumanStr(ape_buf_size).c_str());
        fprintf(label_fp, "seed: %u\n", seed);
        fclose(label_fp);

        Close(label_fd_);
    }

   public:
    Report(Task* task) {
        CreateLabelFile(task);
        result_ = nullptr;
    }
    ~Report() {}
};

struct ThreadContext {
    struct Task* task_;
    Report* rep_;
};

ThreadContext thread_contexts[MAX_THREAD_NUM];
pthread_t threads[MAX_THREAD_NUM];

void InitThreadContext() {
    for (int i = 0; i < thread_num; ++i) {
        ThreadContext& ctx = thread_contexts[i];
        ctx.task_ = new Task(i, ape_buf+buffer_size);
        log_assert(ctx.task_);
        ctx.rep_ = new Report(ctx.task_);
        log_assert(ctx.rep_);
    }
}

void DestroyThreadContext() {
    for (int i = 0; i < thread_num; ++i) {
        ThreadContext& ctx = thread_contexts[i];
        delete ctx.task_;
        delete ctx.rep_;
    }
}

void* ThreadJob(void* arg) {
    bool ret = 0;
    ThreadContext* ctx = (ThreadContext*)arg;
    ret = CoreBind(pthread_self(), ctx->task_->core_id_);
    assert(ret);
    std::vector<int> core_binds = CoreBindQuery(pthread_self());
    assert(core_binds.size() == 1 && core_binds[0] == ctx->task_->core_id_);
    printf("idx[%d] thread[%ld] bind => core [%u]\n", ctx->task_->thread_idx_, pthread_self(),
           ctx->task_->core_id_);

    while (1) {
        ++ret;
    }
    return nullptr;
}

// sudo ./aep_test --flagfile=../config/aep_test_flags
int main(int argc, char* argv[]) {
    LogCfg log_cfg = {
        .is_log_to_stderr = true,
        .vlog_level = 3,
        .min_log_level = 0,
    };
    InitLog(argv[0], &log_cfg);
    ParseAndValidateFlags(&argc, &argv);
    if (FLAGS_clean_rep_dir) {
        printf("Sure to clean report dir: %s [0/1]:", rep_dir_path.c_str());
        int sure = 0;
        cin >> sure;
        if (sure) CleanRepDir();
        return 0;
    }

    InitGlobalCfg();
    PrintGlobalCfg();
    CheckGlobalCfg();
    SetEnv();

    int ret;
    InitThreadContext();
    for (int i = 0; i < thread_num; i++) {
        ret = pthread_create(&threads[i], nullptr, ThreadJob, (void*)(&thread_contexts[i]));
        assert(ret == 0);
    }
    for (int i = 0; i < thread_num; i++) {
        pthread_join(threads[i], NULL);
    }
    DestroyThreadContext();

    DestroyGlobalCfg();
    return 0;
}
