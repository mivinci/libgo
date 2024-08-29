cmake_minimum_required(VERSION 3.28)
project(libgo)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE headers "include/**/*.h")
file(GLOB_RECURSE test_sources "**/*_test.c++")

add_library(go SHARED ${headers})
add_subdirectory(core)
add_subdirectory(net)
target_link_libraries(go core net)
set_target_properties(go PROPERTIES LINKER_LANGUAGE C)

find_package(GTest CONFIG REQUIRED)
add_executable(test test.c++ ${headers} ${test_sources})
target_link_libraries(test go GTest::gtest)
