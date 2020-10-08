# ${CMAKE_SOURCE_DIR}/cmake/FindCatch2.cmake
if(TARGET CATCH2::header_only)
  return()
endif()

set(CATCH_DIR         ${CMAKE_SOURCE_DIR}/thirdparty/Catch2)
set(CATCH_INCLUDE_DIR ${CATCH_DIR}/include/)
if(NOT EXISTS ${CATCH_DIR}/include/catch2/catch.hpp)
  file(MAKE_DIRECTORY ${CATCH_INCLUDE_DIR}/include/catch2/)
  file(
    DOWNLOAD https://github.com/catchorg/Catch2/releases/download/v2.13.1/catch.hpp
    ${CATCH_DIR}/include/catch2/catch.hpp
    SHOW_PROGRESS
  )
  file(
    WRITE
    ${CATCH_DIR}/catch.cpp
    "// Catch2\n#define CATCH_CONFIG_MAIN\n#include <catch2/catch.hpp>\n"
  )
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  Catch2
  REQUIRED_VARS
  CATCH_INCLUDE_DIR
  HANDLE_COMPONENTS
)

add_library(
  CATCH2::header_only
  INTERFACE IMPORTED
)

set_target_properties(
  CATCH2::header_only
  PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CATCH_INCLUDE_DIR}
)

