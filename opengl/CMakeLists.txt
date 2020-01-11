# ${CMAKE_SOURCE_DIR}/window/CMakeLists.txt
find_package(SDL2   REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

add_executable(fixedPipelineOpenGLSDL fixedPipelineOpenGLSDL.cpp)

target_link_libraries(fixedPipelineOpenGLSDL PRIVATE OpenGL::GL SDL2::SDL2 options::options)

add_executable(programmablePipelineOpenGLSDL programmablePipelineOpenGLSDL.cpp)

target_link_libraries(programmablePipelineOpenGLSDL PRIVATE OpenGL::GL SDL2::SDL2 options::options)
