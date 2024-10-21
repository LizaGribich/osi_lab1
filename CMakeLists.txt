cmake_minimum_required(VERSION 3.26)
project(osi1 C)

set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_FLAGS "-O0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -funroll-loops -march=native -fomit-frame-pointer")

add_executable(short_path short-path.c)
add_executable(io_thpt_read io-thpt-read.c)
add_executable(create_file create-file.c)
add_executable(combined combined.c)
add_executable(shell shell.c)
add_executable(test test.c)

