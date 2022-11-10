#include <gtest/gtest.h> // 需要放在第一位，一些定义和log.h中的冲突

#include "util/aep.h"
#include "util/aep_just.h"
#include "util/common.h"

const int BUF_LEN = 64 * 32;

char src[BUF_LEN];
char dest[BUF_LEN];

// char* src;
// char* dest;

ATTR_CONSTRUCTOR void init() {
    // src = (char*)malloc(BUF_LEN);
    // dest = (char*)malloc(BUF_LEN);
    uint32_t seq = 0;
    for(int i = 0; i < BUF_LEN; ++i) {
        src[i] = (seq++)%26 + 'a';
    }
    memset(dest, 0, BUF_LEN);
}

TEST(aep_test_cpy, cpy_native_noflush) {
    for(int len = 1; len <= 32*32; len += len) {
        cpy_native_noflush(dest+1, src+1, len);
        EXPECT_EQ(memcmp(dest+1, src+1, len), 0);
        memset(dest+1, 0, len);
    }
    cpy_native_noflush(dest+1, src+1, 63+32*32+63);
    EXPECT_EQ(memcmp(dest+1, src+1, 63+32*32+63), 0);
    memset(dest+1, 0, 63+32*32+63);
}

TEST(aep_test_cpy, cpy_native_flush) {
    for(int len = 1; len <= 32*32; len += len) {
        cpy_native_flush(dest+1, src+1, len);
        EXPECT_EQ(memcmp(dest+1, src+1, len), 0);
        memset(dest+1, 0, len);
    }
    cpy_native_flush(dest+1, src+1, 63+32*32+63);
    EXPECT_EQ(memcmp(dest+1, src+1, 63+32*32+63), 0);
    memset(dest+1, 0, 63+32*32+63);
}

TEST(aep_test_cpy, cpy_load_store_noflush) {
    for(int len = 1; len <= 32*32; len += len) {
        cpy_load_store_noflush(dest+1, src+1, len);
        EXPECT_EQ(memcmp(dest+1, src+1, len), 0);
        memset(dest+1, 0, len);
    }
    cpy_load_store_noflush(dest+1, src+1, 63+32*32+63);
    EXPECT_EQ(memcmp(dest+1, src+1, 63+32*32+63), 0);
    memset(dest+1, 0, 63+32*32+63);
}

TEST(aep_test_cpy, cpy_load_store_flush) {
    for(int len = 1; len <= 32*32; len += len) {
        cpy_load_store_flush(dest+1, src+1, len);
        EXPECT_EQ(memcmp(dest+1, src+1, len), 0);
        memset(dest+1, 0, len);
    }
    cpy_load_store_flush(dest+1, src+1, 63+32*32+63);
    EXPECT_EQ(memcmp(dest+1, src+1, 63+32*32+63), 0);
    memset(dest+1, 0, 63+32*32+63);
}

TEST(aep_test_cpy, cpy_load_ntstore) {
    for(int len = 1; len <= 32*32; len += len) {
        cpy_load_ntstore(dest+1, src+1, len);
        EXPECT_EQ(memcmp(dest+1, src+1, len), 0);
        memset(dest+1, 0, len);
    }
    cpy_load_ntstore(dest+1, src+1, 63+32*32+63);
    EXPECT_EQ(memcmp(dest+1, src+1, 63+32*32+63), 0);
    memset(dest+1, 0, 63+32*32+63);
}

TEST(aep_test_cpy, cpy_ntload_store_noflush) {
    for(int len = 1; len <= 32*32; len += len) {
        cpy_ntload_store_noflush(dest+1, src+1, len);
        EXPECT_EQ(memcmp(dest+1, src+1, len), 0);
        memset(dest+1, 0, len);
    }
    cpy_ntload_store_noflush(dest+1, src+1, 63+32*32+63);
    EXPECT_EQ(memcmp(dest+1, src+1, 63+32*32+63), 0);
    memset(dest+1, 0, 63+32*32+63);
}

TEST(aep_test_cpy, cpy_ntload_store_flush) {
    for(int len = 1; len <= 32*32; len += len) {
        cpy_ntload_store_flush(dest+1, src+1, len);
        EXPECT_EQ(memcmp(dest+1, src+1, len), 0);
        memset(dest+1, 0, len);
    }
    cpy_ntload_store_flush(dest+1, src+1, 63+32*32+63);
    EXPECT_EQ(memcmp(dest+1, src+1, 63+32*32+63), 0);
    memset(dest+1, 0, 63+32*32+63);
}

TEST(aep_test_cpy, cpy_ntload_ntstore) {
    for(int len = 1; len <= 32*32; len += len) {
        cpy_ntload_ntstore(dest+1, src+1, len);
        EXPECT_EQ(memcmp(dest+1, src+1, len), 0);
        memset(dest+1, 0, len);
    }
    cpy_ntload_ntstore(dest+1, src+1, 63+32*32+63);
    EXPECT_EQ(memcmp(dest+1, src+1, 63+32*32+63), 0);
    memset(dest+1, 0, 63+32*32+63);
}

TEST(aep_test_just, just_load_or_store) {
    for(int len = 1; len <= 32*32; len += len) {
        _just_load(src+1, len);
    }
    _just_load(src+1, 63+32*32+63);

    for(int len = 1; len <= 32*32; len += len) {
        _just_ntload(src+1, len);
    }
    _just_ntload(src+1, 63+32*32+63);

    for(int len = 1; len <= 32*32; len += len) {
        _just_store_noflush(dest+1, len);
    }
    _just_store_noflush(dest+1, 63+32*32+63);

    for(int len = 1; len <= 32*32; len += len) {
        _just_store_flush(dest+1, len);
    }
    _just_store_flush(dest+1, 63+32*32+63);

    for(int len = 1; len <= 32*32; len += len) {
        _just_ntstore(dest+1, len);
    }
    _just_ntstore(dest+1, 63+32*32+63);
}

int main(int argc, char **argv) {
    LogCfg log_cfg = {
        .is_log_to_stderr = true,
        .vlog_level = 3,
        .min_log_level = 0,
    };
    InitLog(argv[0], &log_cfg);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

