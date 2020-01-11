# ${CMAKE_SOURCE_DIR}/window/CMakeLists.txt
find_package(SDL2 REQUIRED)

add_executable(
  simpleWindowSDL
  simpleWindowSDL.cpp
)

target_link_libraries(
  simpleWindowSDL
  PRIVATE
  SDL2::SDL2
  options::options
)

