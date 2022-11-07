#ifndef HLFS_UTIL_COMMON_H_
#define HLFS_UTIL_COMMON_H_

#define force_inline __attribute__((always_inline)) inline

#define ATTR_CONSTRUCTOR __attribute__((constructor)) static
#define ATTR_DESTRUCTOR __attribute__((destructor)) static

#endif // HLFS_UTIL_COMMON_H_
