// STL
#include <string_view>
// OS
#include <Windows.h>
// OpenGL
#include <gl/GL.h>
#include <gl/GLU.h>


constexpr float FovY = 45.0F;
constexpr float Near = 3.0F;
constexpr float Far  = 7.0F;

void resize(HWND hWnd) {
  RECT rect = {};
  GetClientRect(hWnd, &rect);
  const GLsizei width = rect.right - rect.left;
  const GLsizei height = rect.bottom - rect.top;
  glViewport(0, 0, width, height);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_CLOSE:
    DestroyWindow(hWnd);
    return 0;
    case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_SIZE:
      resize(hWnd);
      break; 
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool SetupPixelFormat(HDC hdc) {
  PIXELFORMATDESCRIPTOR pfd, *ppfd;
  int pixelformat;

  ppfd = &pfd;

  // clang-format off
  ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
  ppfd->nVersion     = 1;
  ppfd->dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  ppfd->dwLayerMask  = PFD_MAIN_PLANE;
  ppfd->iPixelType   = PFD_TYPE_COLORINDEX;
  ppfd->cColorBits   = 8;
  ppfd->cDepthBits   = 16;
  ppfd->cAccumBits   = 0;
  ppfd->cStencilBits = 0;
  // clang-format on

  pixelformat = ChoosePixelFormat(hdc, ppfd);

  if((pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0) {
    MessageBox(nullptr, "ChoosePixelFormat failed", "Error", MB_OK);
    return false;
  }

  if(SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) {
    MessageBox(nullptr, "SetPixelFormat failed", "Error", MB_OK);
    return false;
  }

  return true;
} 

HWND CreateMainWindow(HINSTANCE hInstance, int nCmdShow) {
  int width = 640;
  int height = 480;

  constexpr auto cTitle = std::string_view{"HelloWorld!"};

  WNDCLASS windowClass = {};

  // clang-format off
  windowClass.lpfnWndProc   = WindowProc;
  windowClass.hInstance     = hInstance;
  windowClass.lpszClassName = cTitle.data();
  // clang-format on

  RegisterClass(&windowClass);

  HWND hWnd = CreateWindowEx(0,
                             cTitle.data(),
                             cTitle.data(),
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             width,
                             height,
                             nullptr,    // Parent window
                             nullptr,    // Menu
                             hInstance,  // Instance handle
                             nullptr     // Additional application data
  );

  if(hWnd == nullptr) {
    OutputDebugString("Can not create win32 Window");
    return nullptr;
  }

  ShowWindow(hWnd, nCmdShow);
  return hWnd;
}
std::pair<HGLRC, HDC> CreateOpenGLContext(HWND hWnd) {
  const auto hDC = GetDC(hWnd);
  if(!SetupPixelFormat(hDC)) {
    PostQuitMessage(0);
  }

  const HGLRC hRC = wglCreateContext(hDC);
  wglMakeCurrent(hDC, hRC);
  return {hRC, hDC};
}
void InitializeOpenGL(HWND hWnd) {
  resize(hWnd);
  glClearColor(1, 0, 0, 1);
}
void Draw(HDC hDC) {
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_QUADS);
      glVertex3f( 0.5f, 0.5f, 0.0f);
      glVertex3f( 0.5f,-0.5f, 0.0f);
      glVertex3f(-0.5f,-0.5f, 0.0f);
      glVertex3f(-0.5f, 0.5f, 0.0f);
  glEnd();
  SwapBuffers(hDC);
}
void MainLoop(HDC hDC) {
  bool bRunning = true;
  while(bRunning) {
    MSG message = {};
    while(PeekMessage(&message, nullptr, 0, 0, PM_NOREMOVE) == TRUE) {
      if(GetMessage(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
      } else {
        bRunning = false;
        break;
      }
    }
    Draw(hDC);
  }
}
void Cleanup(HWND hWindow, HDC hDC, HGLRC hRC) {
  wglDeleteContext(hRC);
  ReleaseDC(hWindow, hDC);
  DestroyWindow(hWindow);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
  const auto hWindow = CreateMainWindow(hInstance, nCmdShow);
  if(hWindow == nullptr) {
      return 1;
  }
  const auto [hRC, hDC] = CreateOpenGLContext(hWindow);
  InitializeOpenGL(hWindow);
  MainLoop(hDC);
  Cleanup(hWindow, hDC, hRC);

  return 0;
}
