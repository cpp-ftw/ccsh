cmake_minimum_required (VERSION 2.6)

project (ccsh)

find_package(Boost COMPONENTS filesystem system REQUIRED)

option(WITH_LIB   "Compile shared library." ON)
option(WITH_CLING "Compile cling front-end (interactive shell) as well, requires cling at /opt/cling." ON)
option(WITH_TEST  "Compile test code as well, not required for release." ON)

include_directories(include)

if(WITH_LIB)
    add_subdirectory(lib)
endif()

if(WITH_TEST)
    add_subdirectory(test)
endif()

if(WITH_CLING)
    add_subdirectory(ui)
endif()
