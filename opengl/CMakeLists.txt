# ${CMAKE_SOURCE_DIR}/window/CMakeLists.txt
find_package(SDL2   REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(gl3w   REQUIRED)

if(UNIX AND ENABLE_X11)
  find_package(X11 REQUIRED)

  add_executable(fixedPipelineOpenGLX11 fixedPipelineOpenGLX11.cpp)

  target_link_libraries(fixedPipelineOpenGLX11 PRIVATE X11::X11 OpenGL::GL options::options)

  add_executable(programmablePipelineX11 programmablePipelineX11.cpp)

  target_link_libraries(programmablePipelineX11 PRIVATE X11::X11 OpenGL::GL options::options)
endif()

if(WIN32 AND ENABLE_WIN32)
  add_executable(
    fixedPipelineOpenGLWin32
    WIN32
    fixedPipelineOpenGLWin32.cpp
  )

  target_link_libraries(
    fixedPipelineOpenGLWin32
    PRIVATE
    OpenGL::GL
    OpenGL::GLU
    options::options
  )
endif()

set(
    demos
    #fixedPipelineOpenGLSDL
    #programmablePipelineOpenGLSDL
    #programmablePipelineGl3wSDL
    #programmablePipelineSDL2EGL
)

foreach(demo IN LISTS demos)
    add_executable(
        ${demo}
        ${demo}.cpp
    )

    target_link_libraries(
        ${demo}
        PRIVATE
        OpenGL::GL
        gl3w::gl3w
        SDL2::SDL2
        SDL2::SDL2main
        options::options
    )

    target_compile_definitions(
        ${demo}
        PRIVATE
        NOMINMAX
    )
endforeach()

if(ENABLE_Qt5)
  find_package(Qt5 REQUIRED Widgets)
  add_executable(fixedPipelineOpenGLQt5 fixedPipelineOpenGLQt5.cpp)

  target_link_libraries(fixedPipelineOpenGLQt5 PRIVATE Qt5::Widgets OpenGL::GL)
endif()

add_subdirectory(shaders)
