# ${CMAKE_SOURCE_DIR}/window/CMakeLists.txt
find_package(SDL2   REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(gl3w   REQUIRED)

# ===================================
# X11
# ===================================
if(UNIX)
  find_package(X11 REQUIRED)

  add_executable(
    fixedPipelineOpenGLX11
    fixedPipelineOpenGLX11.cpp
  )

  target_link_libraries(
    fixedPipelineOpenGLX11
    PRIVATE
    X11::X11
    OpenGL::GL
    options::options
  )

  add_executable(
    programmablePipelineX11
    programmablePipelineX11.cpp
  )

  target_link_libraries(
    programmablePipelineX11
    PRIVATE
    X11::X11
    OpenGL::GL
    options::options
  )
endif()

# ===================================
# Win32
# ===================================
if(WIN32)
  add_executable(
    fixedPipelineOpenGLWin32
    WIN32
    fixedPipelineOpenGLWin32.cpp
  )

  target_link_libraries(
    fixedPipelineOpenGLWin32
    PRIVATE
    OpenGL::GL
    options::options
  )

  target_compile_definitions(
    fixedPipelineOpenGLWin32
    PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
  )

  add_executable(
    programmablePipelineGl3wWin32
    WIN32
    programmablePipelineGl3wWin32.cpp
  )

  target_link_libraries(
    programmablePipelineGl3wWin32
    PRIVATE
    gl3w::gl3w
    OpenGL::GL
    options::options
  )

  target_compile_definitions(
    programmablePipelineGl3wWin32
    PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
  )
endif()

# ===================================
# SDL2
# ===================================
add_executable(
  fixedPipelineOpenGLSDL
  fixedPipelineOpenGLSDL.cpp
)

target_link_libraries(
  fixedPipelineOpenGLSDL
  PRIVATE
  OpenGL::GL
  SDL2::SDL2
  $<$<PLATFORM_ID:Windows>:SDL2::SDL2main>
  options::options
)

target_compile_definitions(
  fixedPipelineOpenGLSDL
  PRIVATE
  $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
)

add_executable(
  programmablePipelineOpenGLSDL
  programmablePipelineOpenGLSDL.cpp
)

target_link_libraries(
  programmablePipelineOpenGLSDL
  PRIVATE
  OpenGL::GL
  SDL2::SDL2
  $<$<PLATFORM_ID:Windows>:SDL2::SDL2main>
  options::options
)

target_compile_definitions(
  programmablePipelineOpenGLSDL
  PRIVATE
  $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
)

add_executable(
  programmablePipelineGl3wSDL
  programmablePipelineGl3wSDL.cpp
)

target_link_libraries(
  programmablePipelineGl3wSDL
  PRIVATE
  OpenGL::GL
  SDL2::SDL2
  $<$<PLATFORM_ID:Windows>:SDL2::SDL2main>
  options::options
  gl3w::gl3w
)

target_compile_definitions(
  programmablePipelineGl3wSDL
  PRIVATE
  $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN;NOMINMAX>
)
# ===================================
# Qt5
# ===================================
if(ENABLE_Qt5)
  find_package(Qt5 REQUIRED Widgets)
  add_executable(fixedPipelineOpenGLQt5 fixedPipelineOpenGLQt5.cpp)

  target_link_libraries(fixedPipelineOpenGLQt5 PRIVATE Qt5::Widgets OpenGL::GL)
endif()

add_subdirectory(shaders)

