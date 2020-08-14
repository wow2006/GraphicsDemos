# ${CMAKE_SOURCE_DIR}/cmake/Findgl3w.cmake
if(TARGET gl3w::gl3w)
  return()
endif()

enable_language(C)

set(GL3W_DIR ${CMAKE_SOURCE_DIR}/thirdparty/gl3w)

add_library(
  gl3w
  STATIC
)

add_library(
  gl3w::gl3w
  ALIAS
  gl3w
)

target_sources(
  gl3w
  PRIVATE
  ${GL3W_DIR}/src/gl3w.c
)

target_include_directories(
  gl3w
  SYSTEM PUBLIC
  ${GL3W_DIR}/include/
)

target_link_libraries(
  gl3w
  PRIVATE
  $<$<PLATFORM_ID:Linux>:${CMAKE_DL_LIBS}>
)

# TODO(Hussein): Additional modules
#include(FindPackageHandleStandardArgs)
# Handle REQUIRD argument, define *_FOUND variable
#find_package_handle_standard_args(gl3w DEFAULT_MSG gl3w)
