# ${CMAKE_SOURCE_DIR}/cameras/CMakeLists.txt
find_package(SDL2 REQUIRED)

add_executable(
  mimicOpenGL
  mimicOpenGL.cpp
  src/Line.cpp
  src/Image.cpp
)

target_link_libraries(
  mimicOpenGL
  PRIVATE
  SDL2::SDL2
  options::options
)

target_include_directories(
  mimicOpenGL
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

add_subdirectory(test)

