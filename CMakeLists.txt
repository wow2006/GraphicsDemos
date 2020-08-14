# ${CMAKE_SOURCE_DIR}/CMakeLists.txt
cmake_minimum_required(VERSION 3.9)
project(GraphicsDemo CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(options)

#add_subdirectory(window)
#add_subdirectory(opengl)
add_subdirectory(glTF)
