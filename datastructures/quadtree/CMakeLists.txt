# ${CMAKE_SOURCE_DIR}/datastructures/quadtree/CMakeLists.txt
#add_executable(
#  quadtree
#  quadtree.cpp
#)
#
#target_link_libraries(
#  quadtree
#  PRIVATE
#  glm
#  fmt::fmt
#  options::options
#)

if(ENABLE_TESTING)
  add_subdirectory(test)
endif()

