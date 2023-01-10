cmake_minimum_required (VERSION 3.0.2 FATAL_ERROR)

# ----------------------------------------------------------------------------
# package information
set (PACKAGE_NAME        "gflags-learn")
set (PACKAGE_VERSION     "1.0")
# set (PACKAGE_STRING      "${PACKAGE_NAME} ${PACKAGE_VERSION}")
# set (PACKAGE_TARNAME     "${PACKAGE_NAME}-${PACKAGE_VERSION}")
# set (PACKAGE_BUGREPORT   "https://github.com/gflags/gflags/issues")
# set (PACKAGE_DESCRIPTION "A commandline flags library that allows for distributed flags.")
# set (PACKAGE_URL         "http://gflags.github.io/gflags")

project (${PACKAGE_NAME} VERSION ${PACKAGE_VERSION} LANGUAGES CXX)

# add_compile_options(-g)
add_definitions(-D NDEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")

include_directories(../..)

# find_package(gflags REQUIRED)
# find_package(glog REQUIRED)

FILE(GLOB aep_study_src ../../util/*.cc)
list(FILTER aep_study_src EXCLUDE REGEX "../../util/.+test.cc")

# message("${aep_study_src}")

add_executable(aep_study aep_study.cc ${aep_study_src})
target_link_libraries(aep_study glog pthread numa gflags pmem2)

add_executable(intr intr.cc ${aep_study_src})
target_link_libraries(intr glog pthread numa gflags pmem2)

add_executable(helper helper.cc)

add_executable(dram_test dram_test.cc ${aep_study_src})
target_link_libraries(dram_test glog pthread numa gflags pmem2)
