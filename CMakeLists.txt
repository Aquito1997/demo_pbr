cmake_minimum_required(VERSION 3.15)
project(pbr)

execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT ${GIT_VERSION} STREQUAL "")
    set(CMAKE_PROJECT_VERSION ${GIT_VERSION})
endif()

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win/stb_img)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win/glm/inc)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win/ImGui/inc)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win/glfw/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win/GLAD/include/)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win/GLAD/include/glad)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win/assimp_5.4/include)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/linux_x86/stb_img)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/linux_x86/glm/inc)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/linux_x86/ImGui/inc)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/linux_x86/GLAD/include/)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/linux_x86/GLAD/include/glad)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/linux_x86/assimp_5.4/include)

    # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/linux_x86/glfw/include)
endif()

set(Attribute PUBLIC)
string(REGEX REPLACE ".*[/\\\\]([^/\\\\]+)[/\\\\]?$" "\\1" CURRENT_DIR_NAME "${CMAKE_CURRENT_SOURCE_DIR}")
set(Src Demo_PBR.cpp
    ./src/demo_Mesh.cpp
    ./src/demo_PBR.cpp
    ./src/demo_Model.cpp
    ./common/VertexManager.cpp
    ./common/Devices.cpp
    ./common/Function.cpp
    ./common/ImGui_Wrap.cpp
)
add_executable(pbr ${Src}
    common/StackTracer.hpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_subdirectory(thirdparty/win/GLAD thirdparty/glad)
    add_subdirectory(thirdparty/win/ImGui thirdparty/ImGui)
    add_subdirectory(thirdparty/win/stb_img thirdparty/stb_img)

    add_dependencies(pbr glad)
    add_dependencies(pbr stb_image)
    add_dependencies(pbr imgui)

    target_link_directories(pbr ${Attribute}
        ${CMAKE_BINARY_DIR}/thirdparty/GLAD
        ${CMAKE_BINARY_DIR}/thirdparty/ImGui
        ${CMAKE_BINARY_DIR}/thirdparty/stb_img)
    target_link_libraries(pbr ${Attribute} libimgui.dll libglad.dll libstb_image.dll)
    target_link_libraries(pbr ${Attribute} ${CMAKE_SOURCE_DIR}/thirdparty/win/assimp_5.4/lib/libassimp.dll.a)
    target_link_libraries(pbr ${Attribute} ${CMAKE_SOURCE_DIR}/thirdparty/win/glfw/lib-mingw-w64/libglfw3.a)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_subdirectory(thirdparty/linux_x86/GLAD thirdparty/glad)
    add_subdirectory(thirdparty/linux_x86/ImGui thirdparty/ImGui)
    add_subdirectory(thirdparty/linux_x86/stb_img thirdparty/stb_img)
    add_subdirectory(thirdparty/linux_x86/glfw-3.5.1 thirdparty/glfw)

    add_dependencies(pbr glfw)
    add_dependencies(pbr glad)
    add_dependencies(pbr stb_image)
    add_dependencies(pbr imgui)

    target_link_directories(pbr ${Attribute}
        ${CMAKE_BINARY_DIR}/thirdparty/glad
        ${CMAKE_BINARY_DIR}/thirdparty/ImGui
        ${CMAKE_BINARY_DIR}/thirdparty/stb_img
        ${CMAKE_BINARY_DIR}/thirdparty/glfw/src/
        ${CMAKE_SOURCE_DIR}/thirdparty/linux_x86/assimp5.4)

    target_link_libraries(pbr ${Attribute} libimgui.so libglad.so libstb_image.so libglfw3.a)
    target_link_libraries(pbr ${Attribute} ${CMAKE_SOURCE_DIR}/thirdparty/linux_x86/assimp5.4/release/lib/libassimp.so.5)

    # target_link_libraries(pbr ${Attribute} ${CMAKE_SOURCE_DIR}/thirdparty/linux_x86/glfw/lib-mingw-w64/libglfw3.a)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/GLSL/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/GLSL/)
file(COPY ${CMAKE_SOURCE_DIR}/res/Font DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res)
file(COPY ${CMAKE_SOURCE_DIR}/res/module DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res)
file(COPY ${CMAKE_SOURCE_DIR}/res/PBR DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res)
file(COPY ${CMAKE_SOURCE_DIR}/res/skybox DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res)
