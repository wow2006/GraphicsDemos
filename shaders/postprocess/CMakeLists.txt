# ${CMAKE_SOURCE_DIR}/shaders/postprocess/CMakeLists.txt
add_executable(
  simpleFrameBuffer
  simpleFrameBuffer.cpp
)

target_link_libraries(
  simpleFrameBuffer
  PRIVATE
  SDL2::SDL2
  $<$<PLATFORM_ID:Windows>:SDL2::SDL2main>
  options::options
  gl3w::gl3w
)

target_compile_definitions(
  simpleFrameBuffer
  PRIVATE
  $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN;NOMINMAX>
)

file(
  COPY
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)
