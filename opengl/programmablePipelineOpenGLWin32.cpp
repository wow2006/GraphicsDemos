// Windows
#include <Windows.h>
// OpenGL
#include <gl/gl.h>

using wglCreateContextAttribsARB_type = HGLRC (*)(HDC hdc, HGLRC hShareContext, const int *attribList);
wglCreateContextAttribsARB_type wglCreateContextAttribsARB;

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt for all values
constexpr auto WGL_CONTEXT_MAJOR_VERSION_ARB = 0x2091;
constexpr auto WGL_CONTEXT_MINOR_VERSION_ARB = 0x2092;
constexpr auto WGL_CONTEXT_PROFILE_MASK_ARB = 0x9126;

constexpr auto WGL_CONTEXT_CORE_PROFILE_BIT_ARB = 0x00000001;

using wglChoosePixelFormatARB_type =
  BOOL (*)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARB_type wglChoosePixelFormatARB;

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt for all values
constexpr auto WGL_DRAW_TO_WINDOW_ARB = 0x2001;
constexpr auto WGL_ACCELERATION_ARB = 0x2003;
constexpr auto WGL_SUPPORT_OPENGL_ARB = 0x2010;
constexpr auto WGL_DOUBLE_BUFFER_ARB = 0x2011;
constexpr auto WGL_PIXEL_TYPE_ARB = 0x2013;
constexpr auto WGL_COLOR_BITS_ARB = 0x2014;
constexpr auto WGL_DEPTH_BITS_ARB = 0x2022;
constexpr auto WGL_STENCIL_BITS_ARB = 0x2023;

constexpr auto WGL_FULL_ACCELERATION_ARB = 0x2027;
constexpr auto WGL_TYPE_RGBA_ARB = 0x202B;

static LRESULT CALLBACK WindowCallback(HWND window, UINT msg, WPARAM wparam, LPARAM lparam) {
  LRESULT result = 0;

  switch(msg) {
  case WM_CLOSE:
  case WM_DESTROY: PostQuitMessage(0); break;
  default: result = DefWindowProcA(window, msg, wparam, lparam); break;
  }

  return result;
}

namespace {

HWND createWindow(HINSTANCE instance) {
  WNDCLASSA windowClass = {};
  windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  windowClass.lpfnWndProc = WindowCallback;
  windowClass.hInstance = instance;
  windowClass.hCursor = LoadCursor(0, IDC_ARROW);
  windowClass.hbrBackground = 0;
  windowClass.lpszClassName = "WGL_fdjhsklf";

  if(!RegisterClassA(&windowClass)) {
    OutputDebugString("Failed to register window.");
    return nullptr;
  }

  RECT rect = {};
  rect.right = 1024;
  rect.bottom = 576;

  DWORD windowStyle = WS_OVERLAPPEDWINDOW;
  AdjustWindowRect(&rect, windowStyle, false);

  HWND window = CreateWindowExA(
    0, windowClass.lpszClassName, "OpenGL", windowStyle, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, 0, 0, instance, 0);

  if(!window) {
    OutputDebugString("Failed to create window.");
    return nullptr;
  }

  return window;
}

bool initOpenglExtensions() {
  // Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
  // We use a dummy window because you can only set the pixel format for a window once. For the
  // real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
  // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
  // have a context.
  // clang-format off
  WNDCLASSA windowClass = {};
  windowClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  windowClass.lpfnWndProc   = DefWindowProcA;
  windowClass.hInstance     = GetModuleHandle(0);
  windowClass.lpszClassName = "DummyWGL";
  // clang-format on

  if(!RegisterClassA(&windowClass)) {
    OutputDebugString("Failed to register dummy OpenGL window.");
    return false;
  }

  HWND dummy_window = CreateWindowExA(0,
                                      windowClass.lpszClassName,
                                      "Dummy OpenGL Window",
                                      0,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      0,
                                      0,
                                      windowClass.hInstance,
                                      0);

  if(!dummy_window) {
    OutputDebugString("Failed to create dummy OpenGL window.");
    return false;
  }

  HDC dummy_dc = GetDC(dummy_window);

  // clang-format off
  PIXELFORMATDESCRIPTOR pfd = {};
  pfd.nSize        = sizeof(pfd);
  pfd.nVersion     = 1;
  pfd.iPixelType   = PFD_TYPE_RGBA;
  pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.cColorBits   = 32;
  pfd.cAlphaBits   = 8;
  pfd.iLayerType   = PFD_MAIN_PLANE;
  pfd.cDepthBits   = 24;
  pfd.cStencilBits = 8;
  // clang-format on

  int pixelFormat = ChoosePixelFormat(dummy_dc, &pfd);
  if(!pixelFormat) {
    OutputDebugString("Failed to find a suitable pixel format.");
    return false;
  }

  if(!SetPixelFormat(dummy_dc, pixelFormat, &pfd)) {
    OutputDebugString("Failed to set the pixel format.");
    return false;
  }

  HGLRC dummy_context = wglCreateContext(dummy_dc);
  if(!dummy_context) {
    OutputDebugString("Failed to create a dummy OpenGL rendering context.");
    return false;
  }

  if(!wglMakeCurrent(dummy_dc, dummy_context)) {
    OutputDebugString("Failed to activate dummy OpenGL rendering context.");
    return false;
  }

  wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type)wglGetProcAddress("wglCreateContextAttribsARB");
  wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type)wglGetProcAddress("wglChoosePixelFormatARB");

  wglMakeCurrent(dummy_dc, 0);
  wglDeleteContext(dummy_context);
  ReleaseDC(dummy_window, dummy_dc);
  DestroyWindow(dummy_window);

  return true;
}

HGLRC initializeOpenGL(HDC gldc) {
  if (!initOpenglExtensions()) {
    return nullptr;
  }

  // clang-format off
  int pixel_format_attribs[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
    WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
    WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
    WGL_COLOR_BITS_ARB,     32,
    WGL_DEPTH_BITS_ARB,     24,
    WGL_STENCIL_BITS_ARB,   8,
    0
  };
  // clang-format on

  int pixel_format;
  UINT num_formats;
  wglChoosePixelFormatARB(gldc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
  if(!num_formats) {
    OutputDebugString("Failed to set the OpenGL 3.3 pixel format.");
    return nullptr;
  }

  PIXELFORMATDESCRIPTOR pfd;
  DescribePixelFormat(gldc, pixel_format, sizeof(pfd), &pfd);
  if(!SetPixelFormat(gldc, pixel_format, &pfd)) {
    OutputDebugString("Failed to set the OpenGL 3.3 pixel format.");
    return nullptr;
  }

  // Specify that we want to create an OpenGL 3.3 core profile context
  // clang-format off
  int gl33_attribs[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
    WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0,
  };
  // clang-format on

  HGLRC gl33_context = wglCreateContextAttribsARB(gldc, 0, gl33_attribs);
  if(!gl33_context) {
    OutputDebugString("Failed to create OpenGL 3.3 context.");
    return nullptr;
  }

  if(!wglMakeCurrent(gldc, gl33_context)) {
    OutputDebugString("Failed to activate OpenGL 3.3 rendering context.");
    return nullptr;
  }

  return gl33_context;
}

}  // namespace

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd_line, int show) {
  const HWND window = createWindow(inst);
  const HDC gldc    = GetDC(window);
  const HGLRC glrc  = initializeOpenGL(gldc);

  ShowWindow(window, show);
  UpdateWindow(window);

  bool running = true;
  while(running) {
    MSG msg;
    while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
      if(msg.message == WM_QUIT) {
        running = false;
      } else {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
      }
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Do OpenGL rendering here

    SwapBuffers(gldc);
  }

  return 0;
}