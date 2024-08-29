cmake_minimum_required(VERSION 3.26)
project(misc C ASM)
SET (ASM_OPTIONS "-x assembler-with-cpp") # Thanks to https://www.cnblogs.com/sinferwu/p/16015393.html
SET (CMAKE_ASM_FLAGS "${ASM_OPTIONS}")
add_library(1 1.h 1.s 1.c)
