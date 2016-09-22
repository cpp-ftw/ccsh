cmake_minimum_required (VERSION 2.6)

project (ccsh)

file(GLOB_RECURSE sources src/*.cpp src/*.hpp)

add_executable(ccsh ${sources})

# this defines the variables Boost_LIBRARIES that contain all library names
# that we need to link to
find_package(Boost COMPONENTS filesystem system REQUIRED)

target_link_libraries(ccsh PUBLIC
  ${Boost_LIBRARIES}
)

target_compile_options(ccsh PUBLIC -std=c++11 -Wall -Wextra -pedantic -O0 -g -ggdb)
