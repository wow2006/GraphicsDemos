# ${CMAKE_SOURCE_DIR}/shaders/triangle/CMakeLists.txt
set(
  demos
  triangle450
  triangle450UseUinform
  triangle450UseVertexBufferObject
)

foreach(demo IN LISTS demos)
  add_executable(
    ${demo}
    ${demo}.cpp
  )

  target_link_libraries(
    ${demo}
    PRIVATE
    SDL2::SDL2
    $<$<PLATFORM_ID:Windows>:SDL2::SDL2main>
    options::options
    gl3w::gl3w
  )

  target_compile_definitions(
    ${demo}
    PRIVATE
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN;NOMINMAX>
  )
endforeach()

file(
  COPY
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)

