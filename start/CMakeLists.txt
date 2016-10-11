cmake_minimum_required (VERSION 2.6)

add_subdirectory(./../lib ccsh_lib)

include_directories(${SHARED_LIB_INCLUDE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)

add_executable(ccsh ccsh_start.cpp)
target_link_libraries( ccsh PUBLIC ccsh_lib ${Boost_LIBRARIES})
target_compile_options(ccsh PUBLIC -std=c++11 -Wall -Wextra -pedantic -O3 -g -ggdb)
