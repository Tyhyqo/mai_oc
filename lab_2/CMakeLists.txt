cmake_minimum_required(VERSION 3.10)

# Set the project name
project(lab_2_main C)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Add the executable
add_executable(main main.c)

# Link pthread library
target_link_libraries(main pthread)

# Enable ThreadSanitizer
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")