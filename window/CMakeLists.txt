# ${CMAKE_SOURCE_DIR}/window/CMakeLists.txt
find_package(SDL2 REQUIRED)

add_executable(simpleWindowSDL simpleWindowSDL.cpp)

target_link_libraries(simpleWindowSDL PRIVATE SDL2::SDL2 options::options)

if(ENABLE_GLFW)
  find_package(GLFW REQUIRED)

  add_executable(simpleWindowGLFW simpleWindowGLFW.cpp)

  target_link_libraries(simpleWindowGLFW PRIVATE GLFW::glfw3 options::options)
endif()

if(ENABLE_SFML)
  find_package(SFML REQUIRED window)

  add_executable(simpleWindowSFML simpleWindowSFML.cpp)

  target_link_libraries(simpleWindowSFML PRIVATE sfml-window options::options)
endif()

if(ENABLE_Qt5)
  find_package(Qt5 REQUIRED Widgets)

  add_executable(simpleWindowQt5 simpleWindowQt5.cpp)

  target_link_libraries(simpleWindowQt5 PRIVATE Qt5::Widgets)
endif()

if(UNIX)
  find_package(X11 REQUIRED)

  add_executable(
    simpleWindowX11
    simpleWindowX11.cpp
  )

  target_link_libraries(
    simpleWindowX11
    PRIVATE
    X11::X11
    options::options
  )
endif()

if(WIN32)
  add_executable(
    simpleWindowWin32
    WIN32
    simpleWindowWin32.cpp
  )

  target_link_libraries(
    simpleWindowWin32
    PRIVATE
    options::options
  )
endif()
