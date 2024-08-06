cmake_minimum_required(VERSION 3.15)
enable_language(ASM)

set(sources
  sched.c
  timer.c
  asm_${CMAKE_SYSTEM_PROCESSOR}.S
  sys_${CMAKE_SYSTEM_NAME}.c
)

if(NOT DEFINED POLLER)
  if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(POLLER "kqueue")
  elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(POLLER "epoll")
  elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(POLLER "iocp")
  endif()
endif()

list(APPEND sources poller_${POLLER}.c)

add_library(core STATIC ${sources})
include_directories(../include)

if(DEFINED TEST)
  add_definitions(-DTEST=${TEST})
  add_executable(test test.c)
  target_link_libraries(test core)
endif()
