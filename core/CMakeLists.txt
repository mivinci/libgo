cmake_minimum_required(VERSION 3.28)
enable_language(ASM C)
set(CMAKE_C_STANDARD 99)

file(GLOB headers "*.h")
set(sources
  sched.c
  asm_${CMAKE_SYSTEM_PROCESSOR}.S
  sys_${CMAKE_SYSTEM_NAME}.c
)

add_library(core STATIC ${headers} ${sources})
