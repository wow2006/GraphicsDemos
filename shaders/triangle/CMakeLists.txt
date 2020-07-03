find_package(fmt REQUIRED)

add_executable(
  triangle450
  triangle450.cpp
)

target_link_libraries(
  triangle450
  PRIVATE
  SDL2::SDL2
  options::options
  gl3w::gl3w
  fmt::fmt
)

file(
  COPY
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)
