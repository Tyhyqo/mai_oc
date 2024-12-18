cmake_minimum_required(VERSION 3.10)
project(DistributedSystem)

set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZeroMQ REQUIRED libzmq)
include_directories(${ZeroMQ_INCLUDE_DIRS})
link_directories(${ZeroMQ_LIBRARY_DIRS})

add_executable(manager manager.cpp message_queue.cpp)
add_executable(worker worker.cpp message_queue.cpp)

target_link_libraries(manager ${ZeroMQ_LIBRARIES})
target_link_libraries(worker ${ZeroMQ_LIBRARIES})