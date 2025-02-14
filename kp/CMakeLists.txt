cmake_minimum_required(VERSION 3.10)
project(MemoryAllocator C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(allocators
    main.c
    free_list_allocator.c
    buddy_allocator.c
)

target_link_libraries(allocators m)