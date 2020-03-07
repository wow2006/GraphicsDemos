# ${CMAKE_SOURCE_DIR}/cameras/CMakeLists.txt
find_package(SDL2   REQUIRED)
find_package(gl3w   REQUIRED)
find_package(Assimp REQUIRED)

add_executable(
  scene
  main.cpp
)

target_link_libraries(
  scene
  PRIVATE
  SDL2::SDL2
  options::options
  # TODO(Hussein): replace it with Assimp::Assimp
  ${Assimp_LIBRARY}
  gl3w::gl3w
)

file(
  COPY
  ${CMAKE_CURRENT_LIST_DIR}/cube.obj
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

