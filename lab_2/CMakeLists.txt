cmake_minimum_required(VERSION 3.10)

# Set the project name
project(TestProject)

# Add the executable
add_executable(test test.c)

# Enable ThreadSanitizer
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")