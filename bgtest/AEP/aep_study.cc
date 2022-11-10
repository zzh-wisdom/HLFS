#include "aep_study.h"

#include <assert.h>
#include <gflags/gflags.h>
#include <numa.h>
#include <time.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <unordered_map>

#include "util/bitops.h"
#include "util/file.h"
#include "util/log.h"

using namespace std;

enum RwType {
    kRead = 0,
    kWrite = 1,
    kCpy = 2,
    kMemset = 3,
};
#define RW_TYPE_READ "read"
#define RW_TYPE_WRITE "write"
#define RW_TYPE_CPY "cpy"
#define RW_TYPE_MEMSET "memset"
static unordered_map<RwType, string> rw_type_2_str = {
    {kRead, RW_TYPE_READ},
    {kWrite, RW_TYPE_WRITE},
    {kCpy, RW_TYPE_CPY},
    {kMemset, RW_TYPE_MEMSET},
};
static unordered_map<string, RwType> str_2_rw_type = {
    {RW_TYPE_READ, kRead},
    {RW_TYPE_WRITE, kWrite},
    {RW_TYPE_CPY, kCpy},
    {RW_TYPE_MEMSET, kMemset},
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

const int MAX_THREAD_NUM = 32;
const int MAX_CORE_ID = 79;

const uint64_t AEP_SIZE = 64UL * 1024 * 1024 * 1024;
string aep_dev_path = "/dev/dax1.0";
uint32_t thread_num = 1;
uint64_t buffer_size = 1024;
RwType rw_type = kRead;
AccessMode access_mode = kSeq;
uint32_t access_size = 16;
uint32_t access_size_bits = 4;
// runtime 弃用
uint32_t runtime = 5;
uint32_t iters = 10;
uint32_t socket = 1;
uint32_t core_ids[MAX_THREAD_NUM];
uint32_t core_num = 1;

string rep_dir_path = "/mnt/pmem2/aep_test";
const string rep_internal_dir = "internal";
const string rep_report_dir = "report";
string rep_file_prefix = "aep_test";
string rep_data_suffix = "data.dat";
string rep_report_suffix = "report.txt";

uint64_t buf_slots = 0;
uint32_t buf_slots_mask = 0;
uint32_t seed;
struct pmem2_map* aep_map = nullptr;
char* aep_buf;
uint64_t aep_buf_size;
// char* analyze_buf = nullptr;

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
    printf("\taep_dev_path     = %s\n", aep_dev_path.c_str());
    printf("\tthread_num       = %u\n", thread_num);
    printf("\tbuffer_size      = %luB,%luKB,%luMB\n", buffer_size, buffer_size / 1024,
           buffer_size / 1024 / 1024);
    printf("\trw_type          = %s\n", RwType2Str(rw_type).c_str());
    printf("\taccess_mode      = %s\n", AccessMode2Str(access_mode).c_str());
    printf("\taccess_size      = %uB,%uKB,%uMB\n", access_size, access_size / 1024,
           access_size / 1024 / 1024);
    printf("\truntime          = %us\n", runtime);
    printf("\titers            = %u\n", iters);
    printf("\tsocket           = %u\n", socket);
    printf("\tcore_id          = %u", core_ids[0]);
    for (int i = 1; i < thread_num; ++i) {
        printf(",%u", core_ids[i]);
    }
    printf("\n");
    printf("\trep_dir_path     = %s\n", rep_dir_path.c_str());
    printf("\taep_buf_size     = %s\n", Size2HumanStr(aep_buf_size).c_str());
    printf("\tseed             = %u\n", seed);
    printf("\taccess_size_bits = %u\n", access_size_bits);

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

void CleanRepDir() { RemoveDirRecursive(rep_dir_path); }

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
DEFINE_bool(report_to_stdout, false, "...");
DEFINE_bool(vary_access_size, false, "...");
DEFINE_bool(vary_thread_num, false, "...");

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
    if (value > 0) return true;
    printf("Invalid value for --%s: %u\n", flag_name, value);
    return false;
}

void ParseAndValidateFlags(int* argc, char*** argv) {
    gflags::SetUsageMessage("usage msg");
    gflags::RegisterFlagValidator(&FLAGS_thread_num, ValidateThreadNum);
    gflags::RegisterFlagValidator(&FLAGS_rw_type, ValidateRwType);
    gflags::RegisterFlagValidator(&FLAGS_access_mode, ValidateAccessMode);
    gflags::RegisterFlagValidator(&FLAGS_core_ids, ValidateCoreIds);
    gflags::RegisterFlagValidator(&FLAGS_iters, ValidateIters);
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
    log_assert(aep_buf_size >= AEP_SIZE);
    log_assert(buffer_size * thread_num <= AEP_SIZE / 2);
    int buf_size_bits = __fls(buffer_size);
    log_assert((1UL << buf_size_bits) == buffer_size);
    log_assert((1UL << access_size_bits) == access_size);

    log_assert(buf_slots > 0);
    uint32_t buf_slots_bits = __fls(buf_slots);
    CHECK_EQ(1UL << buf_slots_bits, buf_slots);
}

void InitGlobalCfg(bool first_time) {
    if (first_time) {
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

        log_assert(!aep_map);
        aep_map = Pmem2Map(aep_dev_path);
        log_assert(aep_map);
        aep_buf = (char*)pmem2_map_get_address(aep_map);
        log_assert(aep_buf);
        aep_buf_size = pmem2_map_get_size(aep_map);

        pmdk_memcpy = pmem2_get_memcpy_fn(aep_map);
        pmdk_memset = pmem2_get_memset_fn(aep_map);
        pmdk_drain = pmem2_get_drain_fn(aep_map);
    }

    buf_slots = buffer_size / access_size;
    uint32_t buf_slots_bits = __fls(buf_slots);
    buf_slots_mask = (1UL << buf_slots_bits) - 1;
    access_size_bits = __fls(access_size);
    seed = time(nullptr);

    // if(analyze_buf) free(analyze_buf);
    // analyze_buf = (char*)malloc(sizeof(uint32_t) * iters);
    // log_assert(analyze_buf);
}

void DestroyGlobalCfg() {
    Pmem2UnMap(&aep_map);
    aep_map = nullptr;
    aep_buf = nullptr;
    // free(analyze_buf);
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
const uint64_t AEP_TAIL_READ_SIZE = 2ul * 1024 * 1024;

int GetSubOpNum(RwType rw_type) {
    switch (rw_type) {
        case kRead:
            return AEP_READ_FUNC_NUM;
            break;
        case kWrite:
            return AEP_WRITE_FUNC_NUM;
            break;
        case kCpy:
            return AEP_CPY_FUNC_NUM;
            break;
        case kMemset:
            return AEP_MEMSET_FUNC_NUM;
            break;
        default:
            fatal << "error rw_type_:" << rw_type;
            break;
    }
}

aep_op_fn GetOpFn(RwType rw_type, int sub_op) {
    log_assert(sub_op < GetSubOpNum(rw_type));
    switch (rw_type) {
        case kRead:
            return aep_read_funcs[sub_op];
            break;
        case kWrite:
            return aep_write_funcs[sub_op];
            break;
        case kCpy:
            return aep_cpy_funcs[sub_op];
            break;
        case kMemset:
            return aep_memset_funcs[sub_op];
            break;
        default:
            fatal << "error rw_type_:" << rw_type;
            break;
    }
}

const char* GetOpLabel(RwType rw_type, int sub_op) {
    log_assert(sub_op < GetSubOpNum(rw_type));
    switch (rw_type) {
        case kRead:
            return aep_read_func_labels[sub_op];
            break;
        case kWrite:
            return aep_write_func_labels[sub_op];
            break;
        case kCpy:
            return aep_cpy_func_labels[sub_op];
            break;
        case kMemset:
            return aep_memset_func_labels[sub_op];
            break;
        default:
            fatal << "error rw_type_:" << rw_type;
            break;
    }
}

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

    uint32_t access_size_bits_;
    uint32_t* idx_;
    static const uint64_t mem_set_value = 0x3f3f3f3f3f3f3f3fUL;
    char* mem_cpy_buf_;

    int sub_op_ = 0;
    aep_op_fn fn_ = nullptr;

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

        access_size_bits_ = __fls(access_size);
        // 保持在main线程中生成，这样当使用同样的seed，生成的随机数是一样的
        this->idx_ = (uint32_t*)malloc(MAX_IDX_SIZE * sizeof(uint32_t));
        log_assert(this->idx_);
        for (int i = MAX_IDX_SIZE - 1; i >= 0; --i) {
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

    void SetSubOp(int sub_op) {
        sub_op_ = sub_op;
        fn_ = GetOpFn(rw_type_, sub_op_);
        log_assert(fn_);
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

struct RunData {
    struct pmem2_map* aep_data_map_;
    uint32_t* data_;

    RunData(Task* task, const string& time, uint32_t* data) {
        char buf[256];
        // buf size, rw_type_, AccessMode, access_size, run-time, thread_idx
        sprintf(buf, "%s-%s-%s-%s-%d", Size2HumanStr(task->buf_size_).c_str(),
                RwType2Str(task->rw_type_).c_str(), AccessMode2Str(task->access_mode_).c_str(),
                Size2HumanStr(task->access_size_).c_str(), task->thread_idx_);

        string file_common_part = time + "-" + string(buf);
        string data_path = rep_dir_path + "/" + rep_internal_dir + "/" + rep_file_prefix + "-" +
                           file_common_part + "-" + rep_data_suffix;
        info << "open file: " << data_path;
        uint64_t data_size = task->iters_ * sizeof(uint32_t);
        int data_fd_ = OpenAndAllocAtSize(data_path, data_size);
        log_assert(data_fd_ > 0);
        aep_data_map_ = Pmem2MapFromFd(data_fd_);
        log_assert(aep_data_map_);
        data_ = (uint32_t*)pmem2_map_get_address(aep_data_map_);
        Close(data_fd_);
        if (data) {
            data_ = data;
        }
    }
    ~RunData() {
        Pmem2UnMap(&aep_data_map_);
        aep_data_map_ = nullptr;
    }
};

struct AnalyzeResult {
    // 单线程：带宽、平均时延、min、max、P50、P99、P99.9、P99.99
    // 多线程：带宽
    uint32_t total_time;        // ns
    uint32_t total_cal;         // ns
    double bandwidth;           // MB/s
    uint32_t avg_lat;           // ns
    uint32_t avg_lat_from_cal;  // ns
    uint32_t min_lat;
    uint32_t max_lat;
    uint32_t p50;
    uint32_t p99;
    uint32_t p99_9;
    uint32_t p99_99;
    uint32_t p99_999;
};

void Analyze(uint32_t* data, uint64_t run_total_time, int thread_num, AnalyzeResult* result) {
    memset(result, 0, sizeof(AnalyzeResult));
    uint64_t sum_time = 0;
    for (int i = 0; i < iters; ++i) {
        sum_time += data[i];
    }

    result->total_time = run_total_time;
    result->total_cal = sum_time;
    result->bandwidth =
        (double)access_size * iters * thread_num / 1024 / 1024 * 1000000000 / run_total_time;

    if (thread_num == 1) {
        result->avg_lat = run_total_time / iters;
        result->avg_lat_from_cal = sum_time / iters;
        sort(data, data + iters);
        result->min_lat = data[0];
        result->max_lat = data[iters - 1];
        result->p50 = data[iters / 2];
        result->p99 = data[(uint64_t)((double)iters * 0.99)];
        result->p99_9 = data[(uint64_t)((double)iters * 0.999)];
        result->p99_99 = data[(uint64_t)((double)iters * 0.9999)];
        result->p99_999 = data[(uint64_t)((double)iters * 0.99999)];
    }
}

struct Report {
    int report_fd_ = 0;
    FILE* report_fp_ = nullptr;
    Report(const string& file_time) {
        if (file_time.empty()) goto use_std;

        {
            char buf[256];
            // buf size, rw_type_, AccessMode, access_size, run-time, thread_idx
            sprintf(buf, "%s-%s-%s-%s", Size2HumanStr(buffer_size).c_str(),
                    RwType2Str(rw_type).c_str(), AccessMode2Str(access_mode).c_str(),
                    Size2HumanStr(access_size).c_str());
            string file_common_part = file_time + "-" + string(buf);
            string report_path = rep_dir_path + "/" + rep_report_dir + "/" + rep_file_prefix + "-" +
                                 file_common_part + "-" + rep_report_suffix;
            info << "open file: " << report_path;
            report_fd_ = OpenAndAllocAtSize(report_path, 0);
            if (report_fd_ < 0) {
                perror(report_path.c_str());
                exit(-1);
            }
            report_fp_ = fdopen(report_fd_, "w");
            fprintf(report_fp_, "time: %s\n", file_time.c_str());
            fprintf(report_fp_, "buffer_size: %s\n", Size2HumanStr(buffer_size).c_str());
            fprintf(report_fp_, "rw_type: %s\n", RwType2Str(rw_type).c_str());
            fprintf(report_fp_, "access_mode: %s\n", AccessMode2Str(access_mode).c_str());
            fprintf(report_fp_, "access_size: %s\n", Size2HumanStr(access_size).c_str());
            fprintf(report_fp_, "access_size_bits: %u\n", access_size_bits);
            fprintf(report_fp_, "iters: %u\n", iters);
            fprintf(report_fp_, "thread_num: %u\n", thread_num);
            fprintf(report_fp_, "socket: %u\n", socket);
            fprintf(report_fp_, "aep_buf_size: %s\n", Size2HumanStr(aep_buf_size).c_str());
            fprintf(report_fp_, "seed: %u\n", seed);
            fprintf(report_fp_, "--------------------------------------------\n");
        }

    use_std:
        report_fp_ = stdout;
    }
    ~Report() {
        if (report_fp_) fclose(report_fp_);
        if (report_fp_ && report_fp_ != stdout) Close(report_fd_);
    }
    void ReportSubOp(int thread_num, Task* task, AnalyzeResult* result) {
        fprintf(report_fp_, "%-50s ", GetOpLabel(task->rw_type_, task->sub_op_));
        fprintf(report_fp_, "total_time:%-12u ", result->total_time);
        fprintf(report_fp_, "total_cal:%-12u ", result->total_cal);
        fprintf(report_fp_, "bandwidth:%-8.2f ", result->bandwidth);
        if (thread_num == 1) {
            fprintf(report_fp_, "avg_lat:%-6u ", result->avg_lat);
            fprintf(report_fp_, "avg_lat_from_cal:%-6u ", result->avg_lat_from_cal);
            fprintf(report_fp_, "min_lat:%-6u ", result->min_lat);
            fprintf(report_fp_, "max_lat:%-6u ", result->max_lat);
            fprintf(report_fp_, "p50:%-6u ", result->p50);
            fprintf(report_fp_, "p99:%-6u ", result->p99);
            fprintf(report_fp_, "p99_9:%-6u ", result->p99_9);
            fprintf(report_fp_, "p99_99:%-6u ", result->p99_99);
            fprintf(report_fp_, "p99_999:%-6u ", result->p99_999);
        }
        fprintf(report_fp_, "\n");
    }
};

struct ThreadContext {
    struct Task* task_;
    RunData* data_;
};

ThreadContext thread_contexts[MAX_THREAD_NUM];
pthread_t threads[MAX_THREAD_NUM];

void InitThreadContext(const string& file_time, uint32_t* data) {
    for (int i = 0; i < thread_num; ++i) {
        ThreadContext& ctx = thread_contexts[i];
        ctx.task_ = new Task(i, aep_buf + i * buffer_size);
        log_assert(ctx.task_);
        ctx.data_ = new RunData(ctx.task_, file_time, data);
        log_assert(ctx.data_);
    }
}

void ThreadContextSetSubOp(int sub_op) {
    for (int i = 0; i < thread_num; ++i) {
        ThreadContext& ctx = thread_contexts[i];
        ctx.task_->SetSubOp(sub_op);
    }
}

void DestroyThreadContext() {
    for (int i = 0; i < thread_num; ++i) {
        ThreadContext& ctx = thread_contexts[i];
        delete ctx.task_;
        delete ctx.data_;
    }
}

pmem2_memcpy_fn pmdk_memcpy;
pmem2_memset_fn pmdk_memset;
pmem2_drain_fn pmdk_drain;

void PrepareJob(ThreadContext* ctx) {
    uint64_t hash = 0;
    // 加载aep高地址2MB，防止aep内部cache的影响
    // _just_load(aep_buf + aep_buf_size - AEP_TAIL_READ_SIZE, AEP_TAIL_READ_SIZE);
    _just_ntload(ctx->task_->buf_, AEP_TAIL_READ_SIZE);
    _just_ntload(ctx->task_->buf_ + AEP_SIZE / 2, AEP_TAIL_READ_SIZE);
    // 加载buf
    for (int i = 0; i < ctx->task_->access_size_ / sizeof(uint64_t); ++i) {
        hash ^= ((uint64_t*)(ctx->task_->mem_cpy_buf_))[i];
    }
    // 从高到低加载idx
    for (int i = MAX_IDX_SIZE / 2; i >= 0; --i) {
        hash ^= ((uint64_t*)(ctx->task_->idx_))[i];
    }
    // just for cacheline test
    if (ctx->task_->buf_size_ <= 64 * 1024 * 1024) {
        for (int i = 0; i < ctx->task_->buf_size_ / sizeof(uint64_t); ++i) {
            hash ^= ((uint64_t*)(ctx->task_->buf_))[i];
        }
    }
}

void* ThreadJob(void* arg) {
    bool ret = 0;
    ThreadContext* ctx = (ThreadContext*)arg;
    ret = CoreBind(pthread_self(), ctx->task_->core_id_);
    assert(ret);
    std::vector<int> core_binds = CoreBindQuery(pthread_self());
    assert(core_binds.size() == 1 && core_binds[0] == ctx->task_->core_id_);
    r_info("idx[%d] thread[%ld] bind => core [%u]", ctx->task_->thread_idx_, pthread_self(),
           ctx->task_->core_id_);

    PrepareJob(ctx);

    Task* task = ctx->task_;
    char* aep_addr = ctx->task_->buf_;
    char* dram_addr = ctx->task_->mem_cpy_buf_;
    char c = 1;
    for (int i = 0; i < task->iters_; ++i) {
        uint64_t idx = i & buf_slots_mask;
        if (task->access_mode_ == kRandom) {
            idx = task->idx_[i % MAX_IDX_SIZE];
        }
        char* addr1 = nullptr;
        char* addr2 = nullptr;
        if (task->rw_type_ == kRead) {
            addr1 = dram_addr;
            addr2 = aep_addr + (idx << task->access_size_bits_);
        } else if (task->rw_type_ == kWrite) {
            addr1 = aep_addr + (idx << task->access_size_bits_);
            addr2 = dram_addr;
        } else if (task->rw_type_ == kMemset) {
            addr1 = aep_addr + (idx << task->access_size_bits_);
            addr2 = &c;
        } else {
            addr1 = aep_addr + (idx << task->access_size_bits_);
            addr2 = addr1 + AEP_SIZE / 2;
        }
        uint64_t t = task->fn_(addr1, addr2, task->access_size_);
        ctx->data_->data_[i] = t;

        dlog_assert(t > 0 && t < INT32_MAX);
        // printf("%d:%luns, idx:%lu, buf_slots_mask:%u\n",
        //     i, t, idx << task->access_size_bits_, buf_slots_mask);
    }

    return nullptr;
}

uint32_t gen_iters(uint32_t size) {
    if (size >= 32 * 1024) {
        return 500000;
    }
    if (size >= 8 * 1024) {
        return 500000 * 2;
    }
    if (size >= 4 * 1024) {
        return 500000 * 2 * 2;
    }
    if (size >= 1 * 1024) {
        return 4000000;
    }

    return 8000000;
}

void ReCfgWhenThreadNumChange() {
    buffer_size = AEP_SIZE / 2 / thread_num;
    iters = buffer_size / access_size;
    if (iters > 2500000)
        iters = 2500000;
    else if (iters < 500000)
        iters = 500000;
}

void AnalyzeForCachelineFlush(uint32_t* data, uint64_t num, int thread_num, AnalyzeResult* result) {
    memset(result, 0, sizeof(AnalyzeResult));
    uint64_t sum_time = 0;
    for (int i = 0; i < num; ++i) {
        sum_time += data[i];
    }

    result->total_time = 0;
    result->total_cal = sum_time;
    result->bandwidth = 0;

    if (thread_num == 1) {
        result->avg_lat = 0;
        result->avg_lat_from_cal = sum_time / num;

        sort(data, data + num);
        result->min_lat = data[0];
        result->max_lat = data[num - 1];
        result->p50 = data[num / 2];
        result->p99 = data[(uint64_t)((double)num * 0.99)];
        result->p99_9 = data[(uint64_t)((double)num * 0.999)];
        result->p99_99 = data[(uint64_t)((double)num * 0.9999)];
        result->p99_999 = data[(uint64_t)((double)num * 0.99999)];
    }
}

void MainJobVaryThreadNum(Report& rep, const string& file_time) {
    int ret = 0;
    buffer_size = AEP_SIZE / 2;
    core_num = 0;
    for (int i = 20; i <= 39; ++i) {
        core_ids[core_num++] = i;
    }
    for (int i = 60; i < 60 + MAX_THREAD_NUM - 20; ++i) {
        core_ids[core_num++] = i;
    }

    for (thread_num = 1; thread_num <= MAX_THREAD_NUM; thread_num <<= 1) {
        InitGlobalCfg(false);
        ReCfgWhenThreadNumChange();
        printf("=========================== thread_num: %u, iters: %u, buffer_size: %s\n",
               thread_num, iters, Size2HumanStr(buffer_size).c_str());

        InitThreadContext(file_time, nullptr);
        int sub_op_num = GetSubOpNum(rw_type);
        for (int sub_op = 0; sub_op < sub_op_num; ++sub_op) {
            ThreadContextSetSubOp(sub_op);
            r_info("sub_op:%d, %s", sub_op, GetOpLabel(rw_type, sub_op));

            uint64_t start = GetTsNsec();
            for (int i = 0; i < thread_num; i++) {
                ret = pthread_create(&threads[i], nullptr, ThreadJob, (void*)(&thread_contexts[i]));
                assert(ret == 0);
            }
            for (int i = 0; i < thread_num; i++) {
                pthread_join(threads[i], NULL);
            }
            uint64_t end = GetTsNsec();

            uint64_t run_total_time = end - start;
            AnalyzeResult result;
            if (buffer_size <= 64 * 1024 * 1024) {
                log_assert(iters > 5000000);
                AnalyzeForCachelineFlush(thread_contexts[0].data_->data_ + 1000000, iters - 2000000,
                                         thread_num, &result);
            } else {
                Analyze(thread_contexts[0].data_->data_, run_total_time, thread_num, &result);
            }
            rep.ReportSubOp(thread_num, thread_contexts[0].task_, &result);
        }
        DestroyThreadContext();
    }
}

void MainJobVaryAccessSize(Report& rep, const string& file_time) {
    int ret = 0;
    for (access_size = 16; access_size <= 64 * 1024; access_size <<= 1) {
        InitGlobalCfg(false);
        if (thread_num == 1) {
            iters = gen_iters(access_size);
        } else {
            ReCfgWhenThreadNumChange();
        }
        printf("=========================== access_size: %u, iters: %u\n", access_size, iters);

        InitThreadContext(file_time, nullptr);
        int sub_op_num = GetSubOpNum(rw_type);
        for (int sub_op = 0; sub_op < sub_op_num; ++sub_op) {
            ThreadContextSetSubOp(sub_op);
            r_info("sub_op:%d, %s", sub_op, GetOpLabel(rw_type, sub_op));

            uint64_t start = GetTsNsec();
            for (int i = 0; i < thread_num; i++) {
                ret = pthread_create(&threads[i], nullptr, ThreadJob, (void*)(&thread_contexts[i]));
                assert(ret == 0);
            }
            for (int i = 0; i < thread_num; i++) {
                pthread_join(threads[i], NULL);
            }
            uint64_t end = GetTsNsec();

            uint64_t run_total_time = end - start;
            AnalyzeResult result;
            Analyze(thread_contexts[0].data_->data_, run_total_time, thread_num, &result);
            rep.ReportSubOp(thread_num, thread_contexts[0].task_, &result);
        }
        DestroyThreadContext();
    }
}

void MainJobNormal(Report& rep, const string& file_time) {
    int ret = 0;
    InitThreadContext(file_time, nullptr);
    int sub_op_num = GetSubOpNum(rw_type);
    for (int sub_op = 0; sub_op < sub_op_num; ++sub_op) {
        ThreadContextSetSubOp(sub_op);
        r_info("sub_op:%d, %s", sub_op, GetOpLabel(rw_type, sub_op));

        uint64_t start = GetTsNsec();
        for (int i = 0; i < thread_num; i++) {
            ret = pthread_create(&threads[i], nullptr, ThreadJob, (void*)(&thread_contexts[i]));
            assert(ret == 0);
        }
        for (int i = 0; i < thread_num; i++) {
            pthread_join(threads[i], NULL);
        }
        uint64_t end = GetTsNsec();

        uint64_t run_total_time = end - start;
        AnalyzeResult result;
        Analyze(thread_contexts[0].data_->data_, run_total_time, thread_num, &result);
        rep.ReportSubOp(thread_num, thread_contexts[0].task_, &result);
    }
    DestroyThreadContext();
}

// sudo ./aep_study --flagfile=../config/aep_test_flags
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

    CleanRepDir();

    InitGlobalCfg(true);
    PrintGlobalCfg();
    CheckGlobalCfg();
    SetEnv();

    string file_time = GetLocalTime();
    string tmp_str = file_time;
    if (FLAGS_report_to_stdout) {
        tmp_str = "";
    }
    Report rep(file_time);

    if (FLAGS_vary_access_size) {
        MainJobVaryAccessSize(rep, file_time);
    } else if (FLAGS_vary_thread_num) {
        MainJobVaryThreadNum(rep, file_time);
    } else {
        MainJobNormal(rep, file_time);
    }

    DestroyGlobalCfg();
    return 0;
}