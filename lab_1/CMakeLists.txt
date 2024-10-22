cmake_minimum_required(VERSION 3.28)
project(lab_1)

set(CMAKE_C_STANDARD 11)


add_executable(child_1 child_1.c)
add_executable(child_2 child_2.c)
add_executable(parent parent.c)
