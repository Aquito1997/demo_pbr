cmake_minimum_required(VERSION 3.15)
project(imgui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(./inc/ImGui)

file(GLOB Src "*.cpp")
file(GLOB Inc "*.h")

add_library(imgui SHARED ${Src})

target_link_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/glfw/lib-mingw-w64/) 
target_link_libraries(imgui PUBLIC libglfw3dll.a)
