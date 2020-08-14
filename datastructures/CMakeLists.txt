# ${CMAKE_SOURCE_DIR}/window/CMakeLists.txt
find_package(glm REQUIRED)

add_executable(kdtree kdtree.cpp)
target_link_libraries(kdtree PRIVATE glm options::options)

add_executable(quadtree quadtree.cpp)
target_link_libraries(quadtree PRIVATE glm options::options)

