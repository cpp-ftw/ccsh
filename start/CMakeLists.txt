cmake_minimum_required (VERSION 2.6)

add_subdirectory(./../lib ccsh_lib)

include_directories(${SHARED_LIB_INCLUDE_DIR})

add_executable(ccsh_start ccsh_start.cpp)
target_link_libraries( ccsh_start PUBLIC ccsh_lib ${Boost_LIBRARIES})
target_compile_options(ccsh_start PUBLIC -std=c++11 -Wall -Wextra -pedantic -O3 -g -ggdb)
