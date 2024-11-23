cmake_minimum_required(VERSION 3.10)
project(lab_03 C)

set(CMAKE_C_STANDARD 99)

add_executable(parent parent.c)
add_executable(child_1 child_1.c)
add_executable(child_2 child_2.c)

target_link_libraries(parent rt pthread)
target_link_libraries(child_1 rt pthread)
target_link_libraries(child_2 rt pthread)