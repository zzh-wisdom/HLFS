#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <libpmem2.h>

#include <immintrin.h>

#include <util/aep.h>

using namespace std;

char nstore_64byte_label[1024] = "0123456789012345678901234567890123456789012345678901234567890123456789"
        "0123456789012345678901234567890123456789";
char my_load_store_buf[1024] = "my_load_store_buf_987654321098765432109876543210987654321098765432109876543210";

const char* devdax_path = "/dev/dax1.0";

int main() {
    int fd;
	struct pmem2_config *cfg;
	struct pmem2_map *map;
	struct pmem2_source *src;

    if ((fd = open(devdax_path, O_RDWR)) < 0) {
		perror("open");
		exit(1);
	}

	if (pmem2_config_new(&cfg)) {
		pmem2_perror("pmem2_config_new");
		exit(1);
	}

	if (pmem2_source_from_fd(&src, fd)) {
		pmem2_perror("pmem2_source_from_fd");
		exit(1);
	}

	if (pmem2_config_set_required_store_granularity(cfg,
			PMEM2_GRANULARITY_PAGE)) {
		pmem2_perror("pmem2_config_set_required_store_granularity");
		exit(1);
	}

	if (pmem2_map_new(&map, cfg, src)) {
		pmem2_perror("pmem2_map");
		exit(1);
	}

    char *addr = (char*)pmem2_map_get_address(map);
    size_t size = pmem2_map_get_size(map);

    printf("init: \n%s", addr);

    auto memset_fn = pmem2_get_memset_fn(map);
    memset_fn(addr, 0, 2048, 0);
    printf("set before: \n%s", addr);

    const char* _mm_clflush_label = "_mm_clflush,_mm_clflush,_mm_clflush,_mm_clflush,_mm_clflush"
        ",_mm_clflush,_mm_clflush,_mm_clflush,_mm_clflush,_mm_clflush,_mm_clflush";
    const char* _mm_clflushopt_label = "_mm_clflushopt,_mm_clflushopt,_mm_clflushopt,_mm_clflushopt"
        ",_mm_clflushopt,_mm_clflushopt,_mm_clflushopt,_mm_clflushopt,_mm_clflushopt";
    const char* _mm_clwb_label = "_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb"
        ",_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb,_mm_clwb";
    memcpy(addr, _mm_clflush_label, 64);
    addr[63] = '\n';
    clflush(addr);
    memcpy(addr+64, _mm_clflushopt_label, 64);
    addr[63+64] = '\n';
    clflushopt(addr+64);
    memcpy(addr+64+64, _mm_clwb_label, 64);
    addr[63+64+64] = '\n';
    clwb(addr+64+64);
    barrier();
    // load_64byte_fence(nstore_64byte_label);
    // printf("load_64byte_fence\n");
    nstore_64byte_label[63] = '\n';
    cpy_2x32b_ntload_ntstore(addr+64+64+64, nstore_64byte_label);
    cpy_2x32b_load_ntstore(addr+64+64+64+64, nstore_64byte_label);
    my_load_store_buf[63] = '\n';
    cpy_2x32b_load_store_flush(  addr+64+64+64+64+64, my_load_store_buf);
    cpy_2x32b_load_ntstore(      addr+64+64+64+64+64+64, my_load_store_buf);
    cpy_2x32b_load_store_noflush(addr+64+64+64+64+64+64+64, my_load_store_buf);
    sfence();
    lfence();
    mfence();
    printf("set after: \n%s", addr);
    char buf[9];
    abort();
    return 0;
}