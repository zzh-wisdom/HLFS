#include <iostream>
#include <assert.h>
#include <gflags/gflags.h>

using namespace std;

DECLARE_string(devdax_path); // 声明,可以在其他文件中进行声明

static bool ValidateThreadNum(const char* flagname, google::int32 value) {
    if (value >= 0 && value < 40)   // value is ok
        return true;
    printf("Invalid value for --%s: %d\n", flagname, (int)value);
    return false;
}

static bool ValidateThreadNumOther(const char* flagname, google::int32 value) {
    if (value >= 0 && value < 40)   // value is ok
        return true;
    printf("Invalid value for --%s: %d\n", flagname, (int)value);
    return false;
}

static bool ValidateAccessSize(const char* flagname, google::int32 value) {
    if (value > 0 && value <= 1024*1024*1024)   // value is ok
        return true;
    printf("Invalid value for --%s: %d\n", flagname, (int)value);
    return false;
}

DEFINE_string(devdax_path, "/dev/dax1.0", "AEP devdax char device path");
DEFINE_int32(thread_num, 1, "The number of thread");
DEFINE_validator(thread_num, &ValidateThreadNum);
DEFINE_int32(access_size, 8, "access size per op, unit B");

// --novariable/--variable
// --variable=value
// --undefok # 抑制未定义flag的错误
// 以逗号分隔的标志名称列表，即使进程没有定义具有该名称的标志，也可以在命令行上指定
// -- 终止标志处理
// 如果多次指定一个标志，则仅使用最后一个规范;其他的将被忽略。
// 不允许在单个破折号后面“组合”标志，就像在ls -la中一样
int main(int argc, char** argv) {
    gflags::SetUsageMessage("usage msg");

    bool bool_ret;
    string str_ret;
    int int_ret;

    cout << "FLAGS_thread_num RegisterFlagValidator: " << thread_num_validator_registered << endl;
    // 成功，注册是同一个验证函数
    bool_ret = gflags::RegisterFlagValidator(&FLAGS_thread_num, ValidateThreadNum);
    assert(bool_ret);
    // 失败，并打印提示
    bool_ret = gflags::RegisterFlagValidator(&FLAGS_thread_num, ValidateThreadNumOther);
    assert(!bool_ret);
    bool_ret = gflags::RegisterFlagValidator(&FLAGS_access_size, ValidateAccessSize);
    assert(bool_ret);
    cout << endl;

    // 最后一个参数true: argv 将仅保存命令行参数，而不保存命令行标志
    // 如果命令行重复定义一个flag多次，包括flag文件，则最后一次定义的值被使用
    int_ret = gflags::ParseCommandLineFlags(&argc, &argv, true);
    printf("ParseCommandLineFlags ret=%d, argc=%d\n", int_ret, argc);

    str_ret = gflags::SetCommandLineOptionWithMode("thread_num", "5", google::SET_FLAGS_DEFAULT);
    cout << "# " << str_ret << "=>" << "FLAGS_thread_num=" << FLAGS_thread_num << endl;
    str_ret = gflags::SetCommandLineOptionWithMode("thread_num", "4", google::SET_FLAG_IF_DEFAULT);
    cout << "# " << str_ret << "=>" << "FLAGS_thread_num=" << FLAGS_thread_num << endl;
    cout << endl;

    str_ret = gflags::SetCommandLineOption("thread_num", "5"); // 返回值非空则设置成功
    cout << "# " << str_ret << "=>" << "FLAGS_thread_num=" << FLAGS_thread_num << endl;
    str_ret = gflags::SetCommandLineOption("thread_num", ""); // 返回值为空，设置失败保持原值
    cout << "# " << str_ret << "=>" << "FLAGS_thread_num=" << FLAGS_thread_num << endl;
    cout << endl;
    // 之前更新过了， SET_FLAG_IF_DEFAULT无效
    str_ret = gflags::SetCommandLineOptionWithMode("thread_num", "4", google::SET_FLAG_IF_DEFAULT);
    cout << "# " << str_ret << "=>" << "FLAGS_thread_num=" << FLAGS_thread_num << endl;
    //设置默认值
    str_ret = gflags::SetCommandLineOptionWithMode("thread_num", "5", google::SET_FLAGS_DEFAULT);
    cout << "# " << str_ret << "=>" << "FLAGS_thread_num=" << FLAGS_thread_num << endl;
    // 依然无效
    str_ret = gflags::SetCommandLineOptionWithMode("thread_num", "4", google::SET_FLAG_IF_DEFAULT);
    cout << "# " << str_ret << "=>" << "FLAGS_thread_num=" << FLAGS_thread_num << endl;

    printf("devdax_path=%s\n", FLAGS_devdax_path.c_str());
    printf("thread_num=%d\n", FLAGS_thread_num);

    return 0;
}