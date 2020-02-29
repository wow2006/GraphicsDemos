# ${CMAKE_SOURCE_DIR}/CMakeLists.txt
project(GraphicsDemo CXX)
cmake_minimum_required(VERSION 3.9)

option(ENABLE_SFML "" OFF)
option(ENABLE_GLFW "" OFF)
option(ENABLE_Qt5  "" OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(options)

add_subdirectory(window)
add_subdirectory(opengl)
add_subdirectory(cameras)
add_subdirectory(mimicOpenGL)
add_subdirectory(scene)

