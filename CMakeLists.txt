cmake_minimum_required (VERSION 2.6)

project (ccsh)

option(WITH_LIB   "Compile shared library." ON)
option(WITH_CLING "Compile cling front-end (interactive shell) as well, requires cling at /opt/cling." ON)
option(WITH_SAMPLE  "Compile sample code as well, not required for release." ON)

option(BOOST_FILESYSTEM  "Use boost::filesystem instead of std::[experimental::]filesystem" OFF)

if(BOOST_FILESYSTEM)
    find_package(Boost COMPONENTS filesystem system REQUIRED)
    add_definitions(-DCCSH_FILESYSTEM_BOOST)
    set(filesystem_lib "${Boost_LIBRARIES}")
else()
    set(filesystem_lib "stdc++fs")
endif()

include_directories(include)

if(WITH_LIB)
    add_subdirectory(lib)
endif()

if(WITH_SAMPLE)
    add_subdirectory(sample)
endif()

if(WITH_CLING)
    add_subdirectory(ui)
endif()
