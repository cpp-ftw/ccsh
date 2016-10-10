cmake_minimum_required (VERSION 2.6)

project (ccsh)

file(GLOB_RECURSE headers src/*.hpp)

file(GLOB_RECURSE sources_test src/main.cpp)
file(GLOB_RECURSE sources_so   src/ccsh_utils.cpp src/ccsh_command.cpp src/ccsh_operators.cpp)
file(GLOB_RECURSE sources_bin  src/ccsh_start.cpp)

add_executable(ccsh_test  ${sources_test} ${sources_so})
add_executable(ccsh_bin   ${sources_bin} ${sources_so})
add_library   (ccsh_lib   SHARED ${sources_so})

# this defines the variables Boost_LIBRARIES that contain all library names
# that we need to link to

find_package(Boost COMPONENTS filesystem system REQUIRED)

target_link_libraries(ccsh_test  PUBLIC ${Boost_LIBRARIES})
target_link_libraries(ccsh_bin   PUBLIC ${Boost_LIBRARIES})
target_link_libraries(ccsh_lib   PUBLIC ${Boost_LIBRARIES})

target_compile_options(ccsh_test PUBLIC -std=c++11 -Wall -Wextra -pedantic -O0 -g -ggdb)
target_compile_options(ccsh_bin  PUBLIC -std=c++11 -Wall -Wextra -pedantic -O3 -g -ggdb)
target_compile_options(ccsh_lib  PUBLIC -std=c++11 -Wall -Wextra -pedantic -O3 -g -ggdb)
