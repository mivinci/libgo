cmake_minimum_required(VERSION 3.26)
project(libgo)
set(NAME go)

string(TOLOWER ${CMAKE_SYSTEM_NAME} GOOS)
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} GOARCH)
if(GOARCH STREQUAL "amd64")
  set(GOARCH "x86_64")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE headers "include/**/*.h")
file(GLOB_RECURSE test_sources "**/*_test.c++")

add_custom_target(
  gen ALL
  COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/gen
  COMMENT "Generating code from template"
)

add_library(${NAME} SHARED ${headers})
add_subdirectory(core)
target_link_libraries(${NAME} core)
set_target_properties(${NAME} PROPERTIES LINKER_LANGUAGE C)

find_package(GTest CONFIG REQUIRED)
add_executable(test test.c++ ${headers} ${test_sources})
target_link_libraries(test ${NAME} GTest::gtest)
