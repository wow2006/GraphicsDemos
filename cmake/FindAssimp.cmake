find_path(
  Assimp_INCLUDE_DIR
  NAMES assimp/scene.h
  PATHS /usr/include/
        /usr/local/include/
)

find_library(
  Assimp_LIBRARY
  NAMES  assimp
  PATHS /usr/lib/
        /usr/local/lib/
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Assimp DEFAULT_MSG Assimp_INCLUDE_DIR Assimp_LIBRARY
)

# TODO(Hussein): Check static or shared
add_library(
  Assimp::Assimp
  SHARED
  IMPORTED
  GLOBAL
)

target_include_directories(
  Assimp::Assimp
  INTERFACE
  ${Assimp_INCLUDE_DIR}
)

target_link_libraries(
  Assimp::Assimp
  INTERFACE
  ${Assimp_LIBRARY}
)

