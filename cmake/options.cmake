# /cmake/options.cmake

if(CMAKE_TOOLCHAIN_FILE AND WIN32)
  get_filename_component(TOOLCHAIN_DIRECTORY_TEMP ${CMAKE_TOOLCHAIN_FILE}
                         DIRECTORY)
  set(TOOLCHAIN_DIRECTORY ${TOOLCHAIN_DIRECTORY_TEMP}/../..)
endif()

add_library(options INTERFACE)

add_library(options::options ALIAS options)

target_compile_features(options INTERFACE cxx_std_17)

target_compile_options(
  options
  INTERFACE
    $<$<CXX_COMPILER_ID:Clang>:-Weverything;-Wno-c++98-compat-pedantic;-fcolor-diagnostics;-Wno-unused-macros>
    $<$<CXX_COMPILER_ID:GNU>:-Wall;-W;-Wpedantic;-Wshadow;-Wnon-virtual-dtor;-Wold-style-cast;-Wunused;-Wformat=2;-fdiagnostics-color=always>
    $<$<CXX_COMPILER_ID:MSVC>:/W3;/permissive->
)

target_link_libraries(options INTERFACE $<$<PLATFORM_ID:Linux>:stdc++fs>)
