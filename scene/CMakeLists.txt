# ${CMAKE_SOURCE_DIR}/cameras/CMakeLists.txt
find_package(SDL2 REQUIRED)

add_executable(
  scene
  main.cpp
)

target_link_libraries(
  scene
  PRIVATE
  SDL2::SDL2
  options::options
)

