// STL
#include <string_view>
// OS
#include <Windows.h>


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR pCmdLine, int nCmdShow) {
  int width = 640;
  int height = 480;

  constexpr auto cTitle = std::string_view{"HelloWorld!"};

  WNDCLASS windowClass = {};

  windowClass.lpfnWndProc = WindowProc;
  windowClass.hInstance = hInstance;
  windowClass.lpszClassName = cTitle.data();

  RegisterClass(&windowClass);

  HWND hwnd = CreateWindowEx(0,
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

  if(hwnd == nullptr) {
    OutputDebugString("Can not create win32 Window");
    return 0;
  }

  ShowWindow(hwnd, nCmdShow);

  MSG msg = {};
  while(GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch(uMsg) {
  case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
  }
  case WM_PAINT: {
    return 0;
  }
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
