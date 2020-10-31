# ${CMAKE_SOURCE_DIR}/opengl/CMakeLists.txt
find_package(SDL2   REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(gl3w   REQUIRED)

add_subdirectory(context)
add_subdirectory(shaders)
