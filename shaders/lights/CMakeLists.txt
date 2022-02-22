# ${CMAKE_SOURCE_DIR}/shaders/lights/CMakeLists.txt
set(
  lights
  ambient
  #point
  #direct
  #area
  #spot
)

foreach(light IN LISTS lights)
  add_executable(
    ${light}
    ${light}.cpp
  )

  target_link_libraries(
    ${light}
    PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    options::options
    assimp::assimp
    gl3w::gl3w
  )
endforeach()

