#include <cstdint>
#include <vector>
#include <pthread.h>

bool CoreBind(pthread_t t, uint32_t core_id);
std::vector<int> CoreBindQuery(pthread_t t);
