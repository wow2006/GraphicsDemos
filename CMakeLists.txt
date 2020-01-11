# ${CMAKE_SOURCE_DIR}/CMakeLists.txt
project(GraphicsDemo CXX)
cmake_minimum_required(VERSION 3.9)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(options)

add_subdirectory(window)

