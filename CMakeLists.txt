# ${CMAKE_SOURCE_DIR}/CMakeLists.txt
cmake_minimum_required(VERSION 3.9)
project(GraphicsDemo CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(options)

option(ENABLE_SFML    "Build SFML examples"  OFF)
option(ENABLE_GLFW    "Build GLFW examples"  OFF)
option(ENABLE_Qt5     "Build Qt5 examples"   OFF)
option(ENABLE_X11     "Build X11 examples"   OFF)
option(ENABLE_WIN32   "Build Win32 examples" OFF)
option(ENABLE_TESTING "Build unit-test"      OFF)
# TODO(Hussein): Move to WIP Branch
option(ENABLE_WIP     "Work in progress"     OFF)

if(ENABLE_TESTING)
  enable_testing()
  find_package(Catch2 REQUIRED)
endif()

add_subdirectory(window)
add_subdirectory(opengl)
add_subdirectory(shaders)

if(ENABLE_WIP)
  add_subdirectory(cameras)
  add_subdirectory(glTF)
  add_subdirectory(datastructures)
  add_subdirectory(mimicOpenGL)
endif()
