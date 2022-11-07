cmake_minimum_required (VERSION 3.0.2 FATAL_ERROR)

# ----------------------------------------------------------------------------
# package information
set (PACKAGE_NAME        "hlfs")
set (PACKAGE_VERSION     "1.0")
# set (PACKAGE_STRING      "${PACKAGE_NAME} ${PACKAGE_VERSION}")
# set (PACKAGE_TARNAME     "${PACKAGE_NAME}-${PACKAGE_VERSION}")
# set (PACKAGE_BUGREPORT   "https://github.com/gflags/gflags/issues")
# set (PACKAGE_DESCRIPTION "A commandline flags library that allows for distributed flags.")
# set (PACKAGE_URL         "http://gflags.github.io/gflags")

project (${PACKAGE_NAME} VERSION ${PACKAGE_VERSION} LANGUAGES CXX)

add_compile_options(-g)
# add_definitions(-D NDEBUG)

include_directories(.)

# find_package(gflags REQUIRED)
# find_package(glog REQUIRED)

enable_testing()

FILE(GLOB aep_test_src util/*.cc)
list(FILTER aep_test_src EXCLUDE REGEX "util/.+test.cc")

# message("${aep_test_src}")

add_executable(
    util_aep_test
    util/aep_test.cc
    ${aep_test_src}
)
target_link_libraries(
    util_aep_test
    glog pthread numa gflags pmem2
    gtest
)

