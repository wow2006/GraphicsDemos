# ${CMAKE_SOURCE_DIR}/cameras/CMakeLists.txt
find_package(Assimp REQUIRED)
find_package(gl3w   REQUIRED)

add_executable(
  scene
  main.cpp
)

target_include_directories(
  scene
  PRIVATE
  ${Assimp_INCLUDE_DIR}
)

target_link_libraries(
  scene
  PRIVATE
  options::options
  gl3w::gl3w
  CONAN_PKG::glm
  CONAN_PKG::sdl2
  # TODO(Hussein): replace it with Assimp::Assimp
  ${Assimp_LIBRARY}
)

file(
  COPY
  ${CMAKE_CURRENT_LIST_DIR}/Sphere.obj
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

