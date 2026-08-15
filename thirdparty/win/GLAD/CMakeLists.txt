cmake_minimum_required(VERSION 3.15)
project(glad)

include_directories(public "${CMAKE_SOURCE_DIR}/ThirdLib/Win_X86/GLAD/include/KHR")
include_directories(public "${CMAKE_SOURCE_DIR}/ThirdLib/Win_X86/GLAD/include/glad")

add_library(glad SHARED "src/glad.cpp")

target_compile_options(glad PUBLIC -O3)

# target_compile_options(glad PRIVATE -g)