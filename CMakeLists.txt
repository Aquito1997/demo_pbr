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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb_img)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ImGui/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLAD/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLAD/include/glad)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp_5.4/include)



set(Attribute PUBLIC)
string(REGEX REPLACE ".*[/\\\\]([^/\\\\]+)[/\\\\]?$" "\\1" CURRENT_DIR_NAME "${CMAKE_CURRENT_SOURCE_DIR}")
set(Src ${CURRENT_DIR_NAME}.cpp
    ./src/demo_Mesh.cpp
    ./src/demo_PBR.cpp
    ./src/demo_Model.cpp
    ./common/VertexManager.cpp
    ./common/Devices.cpp
    ./common/Function.cpp
    ./common/ImGui_Wrap.cpp
)
add_executable(pbr ${Src})

add_subdirectory(thirdparty/GLAD thirdparty/glad)
add_subdirectory(thirdparty/ImGui thirdparty/ImGui)
add_subdirectory(thirdparty/stb_img thirdparty/stb_img)

add_dependencies(pbr glad)
add_dependencies(pbr stb_image)
add_dependencies(pbr imgui)

target_link_directories(pbr ${Attribute}    ${CMAKE_BINARY_DIR}/thirdparty/GLAD
                                            ${CMAKE_BINARY_DIR}/thirdparty/ImGui
                                            ${CMAKE_BINARY_DIR}/thirdparty/stb_img)
target_link_libraries(pbr  ${Attribute} libimgui.dll libglad.dll libstb_image.dll)

target_link_libraries(pbr  ${Attribute} ${CMAKE_SOURCE_DIR}/thirdparty/assimp_5.4/lib/libassimp.dll.a)
target_link_libraries(pbr  ${Attribute} ${CMAKE_SOURCE_DIR}/thirdparty/glfw/lib-mingw-w64/libglfw3.a)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/GLSL/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/GLSL/)

