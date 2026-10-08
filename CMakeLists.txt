cmake_minimum_required(VERSION 3.10)

project(lab2_ipc C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(child1 src/child1.c)
add_executable(child2 src/child2.c)
add_executable(parent src/parent.c)

add_dependencies(parent child1 child2)