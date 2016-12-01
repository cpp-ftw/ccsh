cmake_minimum_required(VERSION 2.6)

set(SHARED_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(SHARED_LIB_INCLUDE_DIR ${SHARED_LIB_INCLUDE_DIR} PARENT_SCOPE)

set(WRAPPERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../wrappers)
set(WRAPPERS_INCLUDE_DIR ${WRAPPERS_INCLUDE_DIR} PARENT_SCOPE)


if(TARGET ccsh_wrappers)

message("ccsh_wrappers is already defined")

else()

include_directories(${SHARED_LIB_INCLUDE_DIR})

file(GLOB_RECURSE sources *.cpp *.hpp)

add_library(ccsh_wrappers SHARED ${sources})
target_compile_options(ccsh_wrappers  PUBLIC -std=c++11 -Wall -Wextra -pedantic -O3 -g -ggdb)

endif()

