cmake_minimum_required(VERSION 2.6)

set(SHARED_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(SHARED_LIB_INCLUDE_DIR ${SHARED_LIB_INCLUDE_DIR} PARENT_SCOPE)

set(WRAPPERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../wrappers)
set(WRAPPERS_INCLUDE_DIR ${WRAPPERS_INCLUDE_DIR} PARENT_SCOPE)

if(NOT TARGET ccsh_lib) # "include guard"

include_directories(${SHARED_LIB_INCLUDE_DIR})

file(GLOB sources *.cpp *.hpp builtins/*.cpp)
file(GLOB headers ../include/ccsh/*.hpp ../include/ccsh/builtins/*.hpp)

add_library(ccsh_lib SHARED ${sources} ${headers})
target_link_libraries(ccsh_lib PUBLIC ${filesystem_lib})
if(NOT WIN32)
    target_link_libraries(ccsh_lib PUBLIC pthread)
endif()


endif()
