Creating Window
---------------

In these demos, I want to write a simple Window invariant of libraries.
These windows will not support OpenGL Context.
Also, I want to implement for a window using a native library provided by the operating system.

### The idea behind most window mangment:
1. Create a window with Title, Width, Height.
2. Enter event loop.
3. Poll events and processes it.
4. Flush/Swapbuffer.

- [Create X11 Window](simpleWindowX11.cpp)
- [Create Win32 Window](simpleWindowWin32.cpp)
- [Create Android Window](android/app/src/main/cpp/android_main.cpp)
- [Create SDL2 Window](simpleWindowSDL.cpp)
- [Create GLFW3 Window](simpleWindowGLFW.cpp)
- [Create SFML Window](simpleWindowSFML.cpp)
