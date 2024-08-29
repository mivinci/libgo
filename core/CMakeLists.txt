cmake_minimum_required(VERSION 3.26)
enable_language(ASM C)
set(CMAKE_C_STANDARD 99)

file(GLOB headers "*.h")
set(sources
  sched.c
  asm_${GOARCH}.S
  sys_${GOSYS}.c
)

add_library(core STATIC ${headers} ${sources})
