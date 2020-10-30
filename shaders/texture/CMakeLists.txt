# ${CMAKE_SOURCE_DIR}/shaders/texture/CMakeLists.txt
add_executable(
  uploadDownloadTexture
  uploadDownloadTexture.cpp
)

target_link_libraries(
  uploadDownloadTexture
  PRIVATE
  SDL2::SDL2
  $<$<PLATFORM_ID:Windows>:SDL2::SDL2main>
  options::options
  gl3w::gl3w
)

target_compile_definitions(
  uploadDownloadTexture
  PRIVATE
  $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN;NOMINMAX>
)

file(
  COPY
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)
