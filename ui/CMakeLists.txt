cmake_minimum_required (VERSION 2.6)

add_subdirectory(./../lib ccsh_lib)

include_directories(${SHARED_LIB_INCLUDE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)

include_directories(AFTER /opt/cling/include /opt/cling/include/cling/UserInterface)
link_directories(/opt/cling/lib)

add_executable(ccsh ccsh_ui.cpp ccsh_custom_user_interface.cpp)
target_link_libraries( ccsh PUBLIC cling clingUserInterface ccsh_lib ${Boost_LIBRARIES})
target_compile_options(ccsh PUBLIC -std=c++11 -Wall -Wextra -Wno-unused-parameter -pedantic -O3 -g -ggdb -fno-rtti)
