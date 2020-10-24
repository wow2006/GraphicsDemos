# ${CMAKE_SOURCE_DIR}/opengl/shaders/CMakeLists.txt
set(
  samples
  drawRedPointShader
  drawRedTriangleShader
  controlTriangleColorWithMousePosition
  drawTriangleUsingAttributes
  drawTrianglePositionAndColorUsingAttributes
  drawCube
  drawCubeWithPerspective
)

foreach(sample IN LISTS samples)
  add_executable(
    ${sample}
    ${sample}.cpp
  )

  target_link_libraries(
    ${sample}
    PRIVATE
    OpenGL::GL
    SDL2::SDL2
    $<$<PLATFORM_ID:Windows>:SDL2::SDL2main>
    gl3w::gl3w
    options::options
  )

  target_compile_definitions(
    ${sample}
    PRIVATE
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN;NOMINMAX>
  )
endforeach()

