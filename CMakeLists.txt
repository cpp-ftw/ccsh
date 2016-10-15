cmake_minimum_required (VERSION 2.6)

project (ccsh)

find_package(Boost COMPONENTS filesystem system REQUIRED)

include_directories(include)

add_subdirectory(lib)

add_subdirectory(ui)
add_subdirectory(test)
