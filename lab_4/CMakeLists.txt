cmake_minimum_required(VERSION 3.0)
project(Lab4)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

# Создание динамических библиотек
add_library(Derivative1 SHARED Derivative1.c)
target_link_libraries(Derivative1 m)

add_library(Derivative2 SHARED Derivative2.c)
target_link_libraries(Derivative2 m)

add_library(Pi1 SHARED Pi1.c)

add_library(Pi2 SHARED Pi2.c)

# Тестовая программа №1 (линковка на этапе компиляции)
add_executable(Program1 Program1.c)
target_link_libraries(Program1 Derivative1 Pi1 m)

# Тестовая программа №2 (динамическая загрузка библиотек)
add_executable(Program2 Program2.c)
target_link_libraries(Program2 dl)