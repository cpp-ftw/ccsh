cmake_minimum_required (VERSION 2.6)

project (ccsh)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

option(WITH_LIB         "Compile shared library." ON)
option(WITH_COMPILER    "Compile ccshc." ON)
option(WITH_SAMPLE      "Compile sample code as well, not required for release." ON)
option(WITH_TEST        "Compile test code as well, not required for release." ON)

if(NOT WIN32)
    option(WITH_CLING   "Compile cling front-end (interactive shell) as well, requires cling at /opt/cling." ON)
endif()

option(BOOST_FILESYSTEM  "Use boost::filesystem instead of std::[experimental::]filesystem" OFF)

if(BOOST_FILESYSTEM)
    find_package(Boost COMPONENTS filesystem system REQUIRED)
    add_definitions(-DCCSH_FILESYSTEM_BOOST)
    set(filesystem_lib "${Boost_LIBRARIES}")
elseif(NOT WIN32)
    set(filesystem_lib "stdc++fs")
endif()

include_directories(include)

if(WIN32)
    add_definitions(/DUNICODE /D_UNICODE /D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING /EHsc /std:c++latest)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Library output path")
    SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Executable output path")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic") # warning level
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") # ensure C++11
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g") # add debug symbols even in release
endif()

if(WITH_LIB)
    add_subdirectory(lib)
endif()

if(WITH_COMPILER)
    add_subdirectory(cc)
endif()

if(WITH_SAMPLE)
    add_subdirectory(sample)
endif()

if(WITH_TEST)
    if(WIN32)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_LANG_CXX11")
        set(gtest_force_shared_crt ON)
        add_subdirectory(googletest)
        set(GTEST_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/googletest/googletest/include")
        set(GTEST_BOTH_LIBRARIES gtest gtest_main)
    else()
        find_package(GTest REQUIRED)
    endif()

    add_subdirectory(test)
endif()

if(WITH_CLING)
    add_subdirectory(ui)
endif()

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(CodeCoverage)
    setup_target_for_coverage(coverage ccsh_test coverage)
else()
    add_custom_target(coverage)
endif()
