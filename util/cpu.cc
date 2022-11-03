#include "util/cpu.h"

#include <assert.h>
#include <unistd.h>

#include <iostream>

bool CoreBind(pthread_t t, uint32_t core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    int ret_error = pthread_setaffinity_np(t, sizeof(cpu_set_t), &cpuset);
    return (ret_error == 0);
}

std::vector<int> CoreBindQuery(pthread_t t) {
    // 查询绑核核优先级情况
    cpu_set_t cpuset;
    int ret_error = pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    assert(ret_error == 0);
    std::vector<int> ret;
    for (int i = 0; i < CPU_SETSIZE; i++) {
        if (CPU_ISSET(i, &cpuset)) {
            ret.push_back(i);
        }
    }
    return ret;
}

