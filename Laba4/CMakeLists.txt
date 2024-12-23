cmake_minimum_required(VERSION 3.10)

project(Laba4_Allocator C)

set(CMAKE_C_STANDARD 11)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR})

add_library(free_blocks_allocator SHARED ${SOURCE_DIR}/memory_allocator.c)
add_library(power2_allocator SHARED ${SOURCE_DIR}/memory_allocator.c)
add_executable(allocator_test ${SOURCE_DIR}/main.c)

target_link_libraries(allocator_test dl)

set_target_properties(free_blocks_allocator PROPERTIES OUTPUT_NAME "free_blocks_allocator")
set_target_properties(power2_allocator PROPERTIES OUTPUT_NAME "power2_allocator")
