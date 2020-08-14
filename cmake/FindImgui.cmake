# ${CMAKE_SOURCE_DIR}/cmake/FindImgui.cmake
if(TARGET Imgui::SDL_OpenGL)
  return()
endif()

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/thirdparty/imgui/)

# ========================
# IMGUI Core
# ========================
add_library(
  Imgui_core
  STATIC
)

target_sources(
  Imgui_core
  PRIVATE
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
)

target_include_directories(
  Imgui_core
  SYSTEM PUBLIC
  ${IMGUI_DIR}
)

add_library(
  Imgui::core
  ALIAS
  Imgui_core
)

target_compile_definitions(
  Imgui_core
  PUBLIC
  IMGUI_IMPL_OPENGL_LOADER_GL3W
)

# ========================
# IMGUI SDL
# ========================
if(NOT TARGET SDL::SDL)
  find_package(SDL2   REQUIRED)
endif()

if(NOT TARGET gl3w::gl3w)
  find_package(gl3w REQUIRED)
endif()

add_library(
  Imgui_sdl
  STATIC
)

add_library(
  Imgui::SDL
  ALIAS
  Imgui_sdl
)

target_sources(
  Imgui_sdl
  PRIVATE
  ${IMGUI_DIR}/examples/imgui_impl_sdl.cpp
)

target_include_directories(
  Imgui_sdl
  SYSTEM PUBLIC
  ${IMGUI_DIR}/examples/
)

target_link_libraries(
  Imgui_sdl
  PUBLIC
  Imgui::core
  SDL2::SDL2
  gl3w::gl3w
)

# ========================
# IMGUI OpenGL
# ========================
add_library(
  Imgui_sdl_opengl
  STATIC
)

add_library(
  Imgui::SDL_OpenGL
  ALIAS
  Imgui_sdl_opengl
)

target_sources(
  Imgui_sdl_opengl
  PRIVATE
  ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
)

target_include_directories(
  Imgui_sdl_opengl
  SYSTEM PUBLIC
  ${IMGUI_DIR}/examples/
)

target_link_libraries(
  Imgui_sdl_opengl
  PUBLIC
  Imgui::SDL
)

