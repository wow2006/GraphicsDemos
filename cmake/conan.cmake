# Thanks for Jason Turner github.com/lefticus/cpp_weekly_game_project/
macro(run_conan)
set(CONAN_DIR "${CMAKE_SOURCE_DIR}/thirdparty/conan.cmake")
# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS ${CONAN_DIR})
  message(
    STATUS
      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
       ${CONAN_DIR})
endif()

include(${CONAN_DIR})

conan_add_remote(NAME bincrafters URL
                 https://api.bintray.com/conan/bincrafters/public-conan)

conan_cmake_run(
  REQUIRES
  ${CONAN_EXTRA_REQUIRES}
  OPTIONS
  ${CONAN_EXTRA_OPTIONS}
  BASIC_SETUP
  CMAKE_TARGETS # individual targets to link to
  BUILD
  missing
)

endmacro()
