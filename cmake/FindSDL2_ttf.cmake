if(TARGET SDL2::ttf)
  return()
endif()

set(
  INC_SEARCH_PATH
  /usr/include/
)

set(
  LIB_SEARCH_PATH
  /usr/lib/
)

find_path(
  SDL2_TTF_INCLUDE_PATH
  SDL2/SDL_ttf.h
  ${INC_SEARCH_PATH}
)

find_library(
  SDL2_TTF_LIB
  SDL2_ttf
  ${LIB_SEARCH_PATH}
)

find_package_handle_standard_args(
  SDL2_ttf
  DEFAULT_MSG
  SDL2_TTF_INCLUDE_PATH
  SDL2_TTF_LIB
)

add_library(
  SDL2::tff
  UNKNOWN
  IMPORTED
)

set_target_properties(
  SDL2::tff
  PROPERTIES
  IMPORTED_LOCATION "${SDL2_TTF_LIB}"
  INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_INCLUDE_PATH}"
)

