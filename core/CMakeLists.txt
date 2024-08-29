cmake_minimum_required(VERSION 3.26)
enable_language(ASM C)

set(CMAKE_C_STANDARD 99)
set (CMAKE_ASM_FLAGS "-x assembler-with-cpp") # Thanks to https://www.cnblogs.com/sinferwu/p/16015393.html

file(GLOB headers "*.h")
set(sources
  lock.c
  sched.c
  panic.c
  asm_${GOARCH}.s
  sys_${GOOS}.c
)

add_library(core STATIC ${headers} ${sources})
add_dependencies(core gen)
