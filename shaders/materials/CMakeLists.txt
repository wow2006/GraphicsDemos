# ${CMAKE_SOURCE_DIR}/shaders/materials/CMakeLists.txt
set(
  meterials
  loadObj
  ambientPerFragment
  diffuse
  diffusePerFragment
  specular
)

foreach(material IN LISTS meterials)
  add_executable(
    ${material}
    ${material}.cpp
  )

  target_link_libraries(
    ${material}
    PRIVATE
    SDL2::SDL2
    options::options
    # TODO(Hussein): replace it with Assimp::Assimp
    ${Assimp_LIBRARY}
    gl3w::gl3w
  )
endforeach()

file(
  COPY
  ${CMAKE_CURRENT_LIST_DIR}/sphere.obj
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

