# ${CMAKE_SOURCE_DIR}/datastructures/kdtree/CMakeLists.txt
add_executable(
  kdtree
  kdtree.cpp
)

target_link_libraries(
  kdtree
  PRIVATE
  glm
  options::options
)

