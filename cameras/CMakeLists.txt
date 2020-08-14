# ${CMAKE_SOURCE_DIR}/cameras/CMakeLists.txt
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(SDL2   REQUIRED)
find_package(gl3w   REQUIRED)
find_package(glm    REQUIRED)
find_package(Imgui  REQUIRED)

add_executable(
  firstPersonShootingCamera
  firstPersonShootingCamera.cpp
)

target_link_libraries(
  firstPersonShootingCamera
  PRIVATE
  OpenGL::GL
  SDL2::SDL2
  glm
  gl3w::gl3w
  Imgui::SDL_OpenGL
  options::options
)

