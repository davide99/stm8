#dummy cmakelist to make clion happy

cmake_minimum_required(VERSION 3.7)
project(stm8)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE source_list "*.c" "*.h")
add_executable(stm8 ${source_list})
