# ${CMAKE_SOURCE_DIR}/window/CMakeLists.txt
find_package(SDL2   REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(gl3w   REQUIRED)

if(UNIX)
  find_package(X11 REQUIRED)

  add_executable(fixedPipelineOpenGLX11 fixedPipelineOpenGLX11.cpp)

  target_link_libraries(fixedPipelineOpenGLX11 PRIVATE X11::X11 OpenGL::GL options::options)

  add_executable(programmablePipelineX11 programmablePipelineX11.cpp)

  target_link_libraries(programmablePipelineX11 PRIVATE X11::X11 OpenGL::GL options::options)
endif()

add_executable(fixedPipelineOpenGLSDL fixedPipelineOpenGLSDL.cpp)

target_link_libraries(fixedPipelineOpenGLSDL PRIVATE OpenGL::GL SDL2::SDL2 options::options)

add_executable(programmablePipelineOpenGLSDL programmablePipelineOpenGLSDL.cpp)

target_link_libraries(programmablePipelineOpenGLSDL PRIVATE OpenGL::GL SDL2::SDL2 options::options)

add_executable(programmablePipelineGl3wSDL programmablePipelineGl3wSDL.cpp)

target_link_libraries(programmablePipelineGl3wSDL PRIVATE OpenGL::GL SDL2::SDL2 options::options gl3w::gl3w)

if(ENABLE_Qt5)
find_package(Qt5 REQUIRED Widgets)
add_executable(fixedPipelineOpenGLQt5 fixedPipelineOpenGLQt5.cpp)

target_link_libraries(fixedPipelineOpenGLQt5 PRIVATE Qt5::Widgets OpenGL::GL)
endif()

add_subdirectory(shaders)

