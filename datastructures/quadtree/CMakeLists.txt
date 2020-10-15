# ${CMAKE_SOURCE_DIR}/datastructures/quadtree/CMakeLists.txt
add_executable(
  pointRegionQuadtree
  pointRegionQuadtree.cpp
)

target_link_libraries(
  pointRegionQuadtree
  PRIVATE
  glm
  fmt::fmt
  options::options
)

if(ENABLE_TESTING)
  add_subdirectory(test)
endif()

