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
    SDL2::SDL2main
    options::options
    gl3w::gl3w
  )

  target_compile_definitions(
    ${sample}
    PRIVATE
    NOMINMAX
  )
endforeach()

