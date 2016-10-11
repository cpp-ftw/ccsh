cmake_minimum_required (VERSION 2.6)

add_subdirectory(./../lib ccsh_lib)

include_directories(${SHARED_LIB_INCLUDE_DIR})
include_directories(${WRAPPERS_INCLUDE_DIR})

add_executable(ccsh_test main.cpp)
target_link_libraries( ccsh_test PUBLIC ccsh_lib ${Boost_LIBRARIES})
target_compile_options(ccsh_test PUBLIC -std=c++11 -Wall -Wextra -pedantic -O0 -g -ggdb)
