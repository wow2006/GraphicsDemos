find_package(fmt REQUIRED)

set(demos
  triangle450
  triangle450UseUinform
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
    SDL2::SDL2main
    options::options
    gl3w::gl3w
    fmt::fmt-header-only
  )
endforeach()

file(
  COPY
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)

