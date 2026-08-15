cmake_minimum_required(VERSION 3.15)

project(stb_image)

# 关闭编译器警告
add_compile_options(-Wno-error=unused-parameter)
set(CMAKE_C_FLAGS "-Wno-unused-variable")
set(CMAKE_CXX_FLAGS "-Wno-unused-variable")

set(CMAKE_C_FLAGS "-Wno-unused-function")
set(CMAKE_CXX_FLAGS "-Wno-unused-function")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-variable -g -O0 ")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wno-unused-variable -g -O0")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_GENERATOR "Unix Makefiles")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
add_library(stb_image SHARED stb_image.cpp
                             stb_image_write.cpp)

