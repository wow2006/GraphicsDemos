# ${CMAKE_SOURCE_DIR}/window/CMakeLists.txt
find_package(SDL2 REQUIRED)
find_package(GLFW REQUIRED)
find_package(SFML REQUIRED window)

add_executable(simpleWindowSDL simpleWindowSDL.cpp)

target_link_libraries(simpleWindowSDL PRIVATE SDL2::SDL2 options::options)

add_executable(simpleWindowGLFW simpleWindowGLFW.cpp)

target_link_libraries(simpleWindowGLFW PRIVATE GLFW::glfw3 options::options)

add_executable(simpleWindowSFML simpleWindowSFML.cpp)

target_link_libraries(simpleWindowSFML PRIVATE sfml-window options::options)

if(UNIX)
  find_package(X11 REQUIRED)

  add_executable(simpleWindowX11 simpleWindowX11.cpp)
  target_link_libraries(simpleWindowX11 PRIVATE 	X11::X11 options::options)
endif()

if(WIN32)
  add_executable(simpleWindowWin32 WIN32 simpleWindowWin32.cpp)
  target_link_libraries(simpleWindowWin32 PRIVATE options::options)
endif()
