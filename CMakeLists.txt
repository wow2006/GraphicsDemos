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

if(ENABLE_TESTING)
  enable_testing()
  find_package(Catch2 REQUIRED)
endif()

add_subdirectory(window)
add_subdirectory(opengl)
add_subdirectory(cameras)
add_subdirectory(mimicOpenGL)
add_subdirectory(shaders)
add_subdirectory(glTF)
add_subdirectory(datastructures)

