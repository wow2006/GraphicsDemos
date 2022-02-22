# ${CMAKE_SOURCE_DIR}/opengl/shaders/CMakeLists.txt
find_package(glm REQUIRED)
if(NOT TARGET glm::glm)
  add_library(
    glm::glm
    ALIAS
    glm
  )
endif()

set(
  samples
  drawRedPointShader
  drawRedTriangleShader
  controlTriangleColorWithMousePosition
  drawTriangleUsingAttributes
  drawTriangleUsingAttributesDSA
  drawTrianglePositionAndColorUsingAttributes
  drawTrianglePositionAndColorUsingAttributesDSA
  drawTrianglePositionAndColorUsingAttributesDSAOneVBO
  drawCube
  drawCubeWithPerspective
  drawCubeWithPerspectiveUBO
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
    glm::glm
  )

  target_compile_definitions(
    ${sample}
    PRIVATE
    NOMINMAX
  )
endforeach()

