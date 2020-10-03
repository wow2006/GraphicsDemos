// STL
#include <cstdio>
#include <string_view>
// OS
#include <windows.h>
// OpenGL
#include <GL/gl.h>
#include <GL/glu.h>


constexpr auto START_X = 0U;
constexpr auto START_Y = 0U;
constexpr auto WIDTH   = 800U;
constexpr auto HEIGHT  = 600U;
constexpr std::string_view TITLE = "Fixed OpenGL";
constexpr uint8_t Key_ESC = 27;

void display() {
  /* rotate a triangle around */
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_TRIANGLES);
  {
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(0, 1);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(-1, -1);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2i(1, -1);
  }
  glEnd();
  glFlush();
}

LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;

  switch(uMsg) {
  case WM_PAINT: {
    display();
    BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    return 0;
  }

  case WM_SIZE: {
    glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
    PostMessage(hWnd, WM_PAINT, 0, 0);
    return 0;
  }

  case WM_CHAR: {
    switch(wParam) {
    case Key_ESC:
        PostQuitMessage(0);
        break;
    }
    return 0;
  }

  case WM_CLOSE:
  case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
  }
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow) {
  const auto hInstance = GetModuleHandle(nullptr);

  WNDCLASS wc;
  // clang-format off
  wc.style         = CS_OWNDC;
  wc.lpfnWndProc   = reinterpret_cast<WNDPROC>(WindowProc);
  wc.cbClsExtra    = START_X;
  wc.cbWndExtra    = START_Y;
  wc.hInstance     = hInstance;
  wc.hIcon         = LoadIcon(nullptr, IDI_WINLOGO);
  wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = nullptr;
  wc.lpszMenuName  = nullptr;
  wc.lpszClassName = "OpenGL";
  // clang-format on

  if(!RegisterClass(&wc)) {
    MessageBox(nullptr, "RegisterClass() failed: Cannot register window class.", "Error", MB_OK);
    return EXIT_FAILURE;
  }

  // clang-format off
  auto hWnd = CreateWindow("OpenGL",
                           TITLE.data(),
                           WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                           0,
                           100,
                           WIDTH,
                           HEIGHT,
                           nullptr,
                           nullptr,
                           hInstance,
                           nullptr);
  // clang-format on
  if(hWnd == nullptr) {
    MessageBox(nullptr, "CreateWindow() failed:  Cannot create a window.", "Error", MB_OK);
    return EXIT_FAILURE;
  }

  auto hDC = GetDC(hWnd);

  PIXELFORMATDESCRIPTOR pfd;
  // clang-format off
  memset(&pfd, 0, sizeof(pfd));
  pfd.nSize      = sizeof(pfd);
  pfd.nVersion   = 1;
  pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  // clang-format on

  const auto pf = ChoosePixelFormat(hDC, &pfd);
  if(pf == 0) {
    MessageBox(nullptr,
               "ChoosePixelFormat() failed:  "
               "Cannot find a suitable pixel format.",
               "Error",
               MB_OK);
    return 0;
  }

  if(SetPixelFormat(hDC, pf, &pfd) == FALSE) {
    MessageBox(nullptr,
               "SetPixelFormat() failed:  "
               "Cannot set format specified.",
               "Error",
               MB_OK);
    return 0;
  }

  DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

  const auto hRC = wglCreateContext(hDC);
  wglMakeCurrent(hDC, hRC);

  ShowWindow(hWnd, nCmdShow);

  MSG msg;
  while(GetMessage(&msg, hWnd, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  wglMakeCurrent(nullptr, nullptr);
  ReleaseDC(hWnd, hDC);
  wglDeleteContext(hRC);
  DestroyWindow(hWnd);

  return msg.wParam;
}
