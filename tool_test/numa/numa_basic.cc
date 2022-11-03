#include <numa.h>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <string>

int max_bitmask_size = std::max(numa_num_possible_nodes(), numa_num_possible_cpus());

void print_bitmask(bitmask* bitmap_ptr) {
    std::string str;
    for (int i = 0; i < max_bitmask_size; ++i) {
        if (numa_bitmask_isbitset(bitmap_ptr, i)) {
            str += std::to_string(i) + ",";
        }
    }
    if (str.empty())
        str = "null";
    else
        str.pop_back();
    printf("    %-30s -> %s\n", "", str.c_str());
}

int main() {
    bitmask* bitmap_ptr;
    const char* node_str;
    const char* cpu_str;

    // 环境查询
    printf("==========================================================================\n");
    printf("环境查询API:\n");
    int i1 = 1;
    int ret;
    ret = numa_available();
    if (ret) {
        printf("Your system does not support NUMA API\n");
        return -1;
    }
    printf("%2d. %-30s -> %-6d\t支持numa api\n", i1++, "numa_available()", ret);
    ret = numa_max_possible_node();
    printf("%2d. %-30s -> %-6d\t系统/内核支持的最大node编号\n", i1++, "numa_max_possible_node()",
           ret);
    ret = numa_num_possible_nodes();
    printf("%2d. %-30s -> %-6d\t系统/内核支持的最大node数量\n", i1++, "numa_num_possible_nodes()",
           ret);
    ret = numa_num_possible_cpus();
    printf("%2d. %-30s -> %-6d\t系统/内核支持的最大cpu(core)数量\n", i1++, "numa_num_possible_nodes()",
           ret);
    ret = numa_max_node();  // [0,ret]
    printf("%2d. %-30s -> %-6d\t系统当前可用的最大node编号\n", i1++, "numa_max_node()", ret);
    ret = numa_num_configured_nodes();  // numa_max_node() + 1
    printf("%2d. %-30s -> %-6d\t系统当前memory node数量, 包括禁用的节点\n", i1++,
           "numa_num_configured_nodes()", ret);
    ret = numa_num_configured_cpus();  // numa_max_node() + 1
    printf("%2d. %-30s -> %-6d\t系统当前cpu数(core总数), 包括禁用的cpu\n", i1++,
           "numa_num_configured_cpus()", ret);
    long free_size;
    long mem_size = numa_node_size(1, &free_size);
    printf("%2d. %-30s -> %ldM/%ldM\t系统当前cpu数(core总数), 包括禁用的cpu\n", i1++,
           "numa_node_size(1)", free_size/1024/1024, mem_size/1024/1024);
    long long ms,fs;
    ms = numa_node_size64(0, &fs);
    printf("%2d. %-30s -> %lldM/%lldM\t系统当前cpu数(core总数), 包括禁用的cpu\n", i1++,
           "numa_node_size64(0)", fs/1024/1024, ms/1024/1024);

    // 位操作相关
    printf("==========================================================================\n");
    printf("位操作相关API:\n");
    int i2 = 1;
    printf("%2d. %-30s -> %-6s\t包含所有当前任务可用的node\n", i2++,
           "numa_all_nodes_ptr var", "bitmask*");
    print_bitmask(numa_all_nodes_ptr);
    printf("%2d. %-30s -> %-6s\t包含空集node\n", i2++,
           "numa_no_nodes_ptr var", "bitmask*");
    print_bitmask(numa_no_nodes_ptr);
    printf("%2d. %-30s -> %-6s\t包含所有当前任务可用的cpu(core)\n", i2++,
           "numa_all_cpus_ptr var", "bitmask*");
    print_bitmask(numa_all_cpus_ptr);
    node_str = "0-1";
    // 只会解析当前系统可用的node编号
    bitmap_ptr = numa_parse_nodestring(node_str);
    printf("%2d. %-30s -> %-6s\t parse(%s)\n", i2++,
           "numa_parse_nodestring()", "bitmask*", node_str);
    print_bitmask(bitmap_ptr);
    numa_bitmask_free(bitmap_ptr);
    node_str = "!0";
    bitmap_ptr = numa_parse_nodestring(node_str);
    printf("%2d. %-30s -> %-6s\t parse(%s)\n", i2++,
           "numa_parse_nodestring()", "bitmask*", node_str);
    print_bitmask(bitmap_ptr);
    numa_bitmask_free(bitmap_ptr);
    node_str = "0,1";
    bitmap_ptr = numa_parse_nodestring(node_str);
    printf("%2d. %-30s -> %-6s\t parse(%s)\n", i2++,
           "numa_parse_nodestring()", "bitmask*", node_str);
    print_bitmask(bitmap_ptr);
    numa_bitmask_free(bitmap_ptr);
    // 解析所有node，目前看来行为和numa_parse_nodestring一样
    node_str = "!0";
    bitmap_ptr = numa_parse_nodestring_all(node_str);
    printf("%2d. %-30s -> %-6s\t parse(%s)\n", i2++,
           "numa_parse_nodestring_all()", "bitmask*", node_str);
    print_bitmask(bitmap_ptr);
    numa_bitmask_free(bitmap_ptr);
    // 解析cpu core编号
    // 好像-!之间不能混用
    cpu_str = "0-9,+12-15";
    bitmap_ptr = numa_parse_cpustring(cpu_str);
    printf("%2d. %-30s -> %-6s\t parse(%s)\n", i2++,
           "numa_parse_cpustring()", "bitmask*", cpu_str);
    print_bitmask(bitmap_ptr);
    numa_bitmask_free(bitmap_ptr);
    // 不能有多个! 如!20-39
    cpu_str = "!0-39";
    bitmap_ptr = numa_parse_cpustring(cpu_str);
    printf("%2d. %-30s -> %-6s\t parse(%s)\n", i2++,
           "numa_parse_cpustring()", "bitmask*", cpu_str);
    print_bitmask(bitmap_ptr);
    numa_bitmask_free(bitmap_ptr);
    cpu_str = "+0-39";
    bitmap_ptr = numa_parse_cpustring(cpu_str);
    printf("%2d. %-30s -> %-6s\t parse(%s)\n", i2++,
           "numa_parse_cpustring()", "bitmask*", cpu_str);
    print_bitmask(bitmap_ptr);
    numa_bitmask_free(bitmap_ptr);
    cpu_str = "!0-39";
    bitmap_ptr = numa_parse_cpustring_all(cpu_str);
    printf("%2d. %-30s -> %-6s\t parse(%s)\n", i2++,
           "numa_parse_cpustring_all()", "bitmask*", cpu_str);
    print_bitmask(bitmap_ptr);
    numa_bitmask_free(bitmap_ptr);

    // 进程线程相关
    printf("==========================================================================\n");
    printf("进程线程相关API:\n");
    int i3 = 1;
    bitmap_ptr = numa_get_mems_allowed();
    printf("%2d. %-30s -> %-6s\t当前进程允许分配内存的node\n", i3++, "numa_get_mems_allowed()",
           "bitmask*");
    print_bitmask(bitmap_ptr);
    numa_bitmask_free(bitmap_ptr);
    // numactl --cpubind=0 --membind=1 时返回值会发生变化
    ret = numa_num_task_cpus();
    printf("%2d. %-30s -> %-6d\t当前任务可用的cpu个数(core)\n", i3++,
           "numa_num_task_cpus()", ret);
    ret = numa_num_task_nodes();
    printf("%2d. %-30s -> %-6d\t当前任务可用的node个数\n", i3++,
           "numa_num_task_nodes()", ret);

    // mask
    nodemask_t mask;  // 类似cpuset
    nodemask_zero(&mask);
    // nodemask_set设置，用nodemask_clr清除。nodemask_equal比较两个节点。nodemask_isset测试是否在nodemask中设置了位。
    // 有两个预定义的节点 : numa_all_nodes 表示系统中的所有节点，numa_no_nodes 是空集。

    return 0;
}