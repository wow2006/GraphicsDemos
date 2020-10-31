# ${CMAKE_SOURCE_DIR}/CMakeLists.txt
cmake_minimum_required(VERSION 3.9)
project(SimpleOpenGL C CXX)

add_library(
  native_app_glue
  STATIC
  ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

target_include_directories(
  native_app_glue
  SYSTEM PUBLIC
  ${ANDROID_NDK}/sources/android/native_app_glue/
)

add_library(
  ${PROJECT_NAME}
  SHARED
  android_main.cpp
)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  android
  log
  native_app_glue
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
  -Weverything
  -fcolor-diagnostics
  -Wpedantic
  -Wno-c++98-compat-pedantic
  -Wno-c++98-compat
  -Wno-documentation
  -Wno-documentation-unknown-command
  -Wno-padded              # TODO(Hussein): remove me
  -Wno-global-constructors # TODO(Hussein): remove me
  -Wno-unused-macros       # TODO(hussein): remove me
  -Wno-old-style-cast      # TODO(hussein): remove me
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  CXX_EXTENSIONS        OFF
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED ON
  # Export ANativeActivity_onCreate(),
  # Refer to: https://github.com/android-ndk/ndk/issues/381.
  LINK_FLAGS "-u ANativeActivity_onCreate"
)

