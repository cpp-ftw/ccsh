cmake_minimum_required (VERSION 2.6)

add_subdirectory(./../lib ccsh_lib)

include_directories(${SHARED_LIB_INCLUDE_DIR})
include_directories(${WRAPPERS_INCLUDE_DIR})

file(GLOB_RECURSE wrappers ../wrappers/*.hpp)
file(GLOB_RECURSE tests test*.cpp)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(ccsh_test ${tests} ${wrappers})
target_link_libraries( ccsh_test PUBLIC ccsh_lib ${filesystem_lib} ${GTEST_BOTH_LIBRARIES})
