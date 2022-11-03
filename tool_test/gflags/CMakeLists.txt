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

find_package(gflags REQUIRED)

add_executable(gflags_basic gflags_basic.cc)
target_link_libraries(gflags_basic gflags)
