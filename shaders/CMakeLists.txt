# ${CMAKE_SOURCE_DIR}/shaders/CMakeLists.txt
find_package(SDL2      REQUIRED)
find_package(glbinding REQUIRED)
find_package(assimp    REQUIRED)

add_subdirectory(materials)
add_subdirectory(lights)
add_subdirectory(triangle)

