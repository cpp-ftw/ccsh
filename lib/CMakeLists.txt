cmake_minimum_required (VERSION 2.6)

cmake_minimum_required(VERSION 2.6)

set(SHARED_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(SHARED_LIB_INCLUDE_DIR ${SHARED_LIB_INCLUDE_DIR} PARENT_SCOPE)

set(WRAPPERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../wrappers)
set(WRAPPERS_INCLUDE_DIR ${WRAPPERS_INCLUDE_DIR} PARENT_SCOPE)


if(TARGET ccsh_lib)

message("ccsh_lib is already defined")

else()

include_directories(${SHARED_LIB_INCLUDE_DIR})

file(GLOB sources *.cpp *.hpp)
file(GLOB headers ../include/ccsh/*.hpp)

add_library(ccsh_lib SHARED ${sources} ${headers})
target_link_libraries(ccsh_lib   PUBLIC ${Boost_LIBRARIES})
target_compile_options(ccsh_lib  PUBLIC -std=c++11 -Wall -Wextra -pedantic -O3 -g -ggdb)

endif()

