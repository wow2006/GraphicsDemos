// STL
#include <array>
#include <optional>
#include <cstring>
#include <cstdlib>
#include <iostream>
// X11
#include <X11/Xlib.h>
// OpenGL
#include <GL/glx.h>

static auto parseProgramOptions(int argc, char **argv) -> std::optional<std::pair<uint32_t, uint32_t>> {
  if(argc != 3) {
    return std::nullopt;
  }

  return std::make_pair(std::stoi(argv[1]), std::stoi(argv[2]));
}

auto main(int argc, char *argv[]) -> int {
  auto pDisplay = XOpenDisplay(nullptr);
  if(pDisplay == nullptr) {
    fprintf(stderr, "Cannot open display\n");
    return EXIT_FAILURE;
  }

  auto width = 640U;
  auto height = 480U;
  if(const auto args = parseProgramOptions(argc, argv); args) {
    constexpr auto primaryDisplay = 0;
    auto screenID = ScreenOfDisplay(pDisplay, primaryDisplay);
    width = std::min(args.value().first, static_cast<uint32_t>(screenID->width));
    height = std::min(args.value().second, static_cast<uint32_t>(screenID->height));
  }

  auto screen = DefaultRootWindow(pDisplay);

  // clang-format off
  GLint defaultFramebufferAttributes[] = {
    GLX_RGBA,
    GLX_DEPTH_SIZE, 24,
    GLX_DOUBLEBUFFER,
    None
  };
  // clang-format on

  auto visual = glXChooseVisual(pDisplay, 0, defaultFramebufferAttributes);
  if(visual == nullptr) {
    std::cerr << "Can not create visual\n";
    return EXIT_FAILURE;
  }

  auto cmap = XCreateColormap(pDisplay, screen, visual->visual, AllocNone);

  XSetWindowAttributes windowAttribs;
  windowAttribs.border_pixel      = BlackPixel(pDisplay, screen);
  windowAttribs.background_pixel  = WhitePixel(pDisplay, screen);
  windowAttribs.override_redirect = True;
  windowAttribs.colormap          = cmap;
  windowAttribs.event_mask        = ExposureMask;

  auto pWindow =
    XCreateWindow(pDisplay, screen, 0, 0, width, height, 0, visual->depth, InputOutput, visual->visual, CWColormap | CWEventMask, &windowAttribs);

  XMapWindow(pDisplay, pWindow);
  XStoreName(pDisplay, pWindow, "Hello OpenGL X11!");

  // Redirect Close
  Atom atomWmDeleteWindow = XInternAtom(pDisplay, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(pDisplay, pWindow, &atomWmDeleteWindow, 1);

  auto context = glXCreateContext(pDisplay, visual, nullptr, GL_TRUE);
  glXMakeCurrent(pDisplay, pWindow, context);

  std::cout << "GL Renderer: "  << glGetString(GL_RENDERER) << "\n";
  std::cout << "GL Version: "   << glGetString(GL_VERSION) << "\n";
  std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

  glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

  std::array<float, 4> clearColor= {0.1F, 0.1F, 0.15F, 1.F};
  glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);


  auto running = true;
  while(running) {
    if(XPending(pDisplay) > 0) {
      XEvent event;
      XNextEvent(pDisplay, &event);
      if(event.type == ClientMessage) {
        if(event.xclient.data.l[0] == static_cast<long>(atomWmDeleteWindow)) {
          running = false;
        }
      } else if(event.type == DestroyNotify) {
          running = false;
      }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glXSwapBuffers(pDisplay, pWindow);
  }

  glXMakeCurrent(pDisplay, None, nullptr);
  glXDestroyContext(pDisplay, context);
  XDestroyWindow(pDisplay, pWindow);
  XCloseDisplay(pDisplay);
  return EXIT_SUCCESS;
}
