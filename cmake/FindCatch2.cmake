# ${CMAKE_SOURCE_DIR}/cmake/FindCatch2.cmake
if(TARGET CATCH2::header_only)
  return()
endif()

set(CATCH_DIR ${CMAKE_SOURCE_DIR}/thirdparty/Catch2/)
if(NOT EXISTS ${CATCH_DIR}/include/catch2/catch.hpp)
  file(MAKE_DIRECTORY ${CATCH_DIR}/include/catch2/)
  file(WRITE "// Catch2\n#define CATCH_CONFIG_MAIN\n#include <catch2/catch.hpp>\n" ${CATCH_DIR}/catch.cpp)
  file(
    DOWNLOAD https://github.com/catchorg/Catch2/releases/download/v2.13.1/catch.hpp
    ${CATCH_DIR}/include/catch2/catch.hpp
  )
endif()

add_library(
  CATCH2::header_only
  INTERFACE IMPORTED
)

set_target_properties(
  CATCH2::header_only
  PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CATCH_DIR}/include/
)

