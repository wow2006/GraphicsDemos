# ${CMAKE_SOURCE_DIR}/window/CMakeLists.txt
find_package(SDL2 REQUIRED)
find_package(GLFW REQUIRED)

add_executable(simpleWindowSDL simpleWindowSDL.cpp)

target_link_libraries(simpleWindowSDL PRIVATE SDL2::SDL2 options::options)

add_executable(simpleWindowGLFW simpleWindowGLFW.cpp)

target_link_libraries(simpleWindowGLFW PRIVATE GLFW::glfw3 options::options)
