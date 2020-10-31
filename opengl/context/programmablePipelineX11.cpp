// STL
#include <array>
#include <optional>
#include <cstring>
#include <cstdlib>
#include <iostream>
// System
#include <unistd.h>
#include <sys/time.h>
// X11
#include <X11/Xlib.h>
// OpenGL
#include <GL/glx.h>

static bool isExtensionSupported(const char *extList, const char *extension) { return strstr(extList, extension) != nullptr; }

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

  auto screen = DefaultScreenOfDisplay(pDisplay);
  (void)screen;
  auto screenID = DefaultScreen(pDisplay);

  // Check GLX version
  GLint majorGLX, minorGLX = 0;
  glXQueryVersion(pDisplay, &majorGLX, &minorGLX);
  if(majorGLX <= 1 && minorGLX < 2) {
    std::cout << "GLX 1.2 or greater is required.\n";
    XCloseDisplay(pDisplay);
    return EXIT_FAILURE;
  }

  // clang-format off
  // Default FrameBuffer
  constexpr int defaultFramebufferAttributes[] = {
    GLX_X_RENDERABLE,  True,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    GLX_RED_SIZE,      8,
    GLX_GREEN_SIZE,    8,
    GLX_BLUE_SIZE,     8,
    GLX_ALPHA_SIZE,    8,
    GLX_DEPTH_SIZE,    24,
    GLX_STENCIL_SIZE,  8,
    GLX_DOUBLEBUFFER,  True,
    None
  };
  // clang-format on

  int fbcount;
  GLXFBConfig *fbc = glXChooseFBConfig(pDisplay, screenID, defaultFramebufferAttributes, &fbcount);
  if(fbc == nullptr) {
    std::cout << "Failed to retrievente framebuffer.\n";
    XCloseDisplay(pDisplay);
    return EXIT_FAILURE;
  }

  XVisualInfo *visual = nullptr;
  GLXFBConfig bestFbc;
  {
    // Pick the FB config/visual with the most samples per pixel
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
    for(int i = 0; i < fbcount; ++i) {
      XVisualInfo *vi = glXGetVisualFromFBConfig(pDisplay, fbc[i]);
      if(vi != nullptr) {
        int samp_buf, samples;
        glXGetFBConfigAttrib(pDisplay, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
        glXGetFBConfigAttrib(pDisplay, fbc[i], GLX_SAMPLES, &samples);

        if(best_fbc < 0 || (samp_buf && samples > best_num_samp)) {
          best_fbc = i;
          best_num_samp = samples;
        }
        if(worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
          worst_fbc = i;
        worst_num_samp = samples;
      }
      XFree(vi);
    }
    bestFbc = fbc[best_fbc];
    XFree(fbc);  // Make sure to free this!

    visual = glXGetVisualFromFBConfig(pDisplay, bestFbc);
    if(visual == nullptr) {
      std::cout << "Could not create correct visual window.\n";
      XCloseDisplay(pDisplay);
      return 1;
    }

    if(screenID != visual->screen) {
      std::cout << "screenId(" << screenID << ") does not match visual->screen(" << visual->screen << ").\n";
      XCloseDisplay(pDisplay);
      return 1;
    }
  }

  XSetWindowAttributes windowAttribs;
  windowAttribs.border_pixel = BlackPixel(pDisplay, screenID);
  windowAttribs.background_pixel = WhitePixel(pDisplay, screenID);
  windowAttribs.override_redirect = True;
  windowAttribs.colormap = XCreateColormap(pDisplay, RootWindow(pDisplay, screenID), visual->visual, AllocNone);
  windowAttribs.event_mask = ExposureMask;

  // clang-format off
  auto window = XCreateWindow(pDisplay,
                              RootWindow(pDisplay, screenID),
                              0,
                              0,
                              width,
                              height,
                              0,
                              visual->depth,
                              InputOutput,
                              visual->visual,
                              CWBackPixel | CWColormap | CWBorderPixel | CWEventMask,
                              &windowAttribs);
  // clang-format on

  // Redirect Close
  Atom atomWmDeleteWindow = XInternAtom(pDisplay, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(pDisplay, window, &atomWmDeleteWindow, 1);

  GLXContext context = nullptr;
  {  // Create GLX OpenGL context
    using glXCreateContextAttribsARBProc = GLXContext (*)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = nullptr;

    constexpr char funName[] = "glXCreateContextAttribsARB";
    auto funNamePtr = reinterpret_cast<const GLubyte *>(funName);
    glXCreateContextAttribsARB = reinterpret_cast<glXCreateContextAttribsARBProc>(glXGetProcAddressARB(funNamePtr));

    // clang-format off
    int context_attribs[] = {
      GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
      GLX_CONTEXT_MINOR_VERSION_ARB, 2,
      GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
      None
    };
    // clang-format on

    const char *glxExts = glXQueryExtensionsString(pDisplay, screenID);
    if(!isExtensionSupported(glxExts, "GLX_ARB_create_context")) {
      std::cout << "GLX_ARB_create_context not supported\n";
      context = glXCreateNewContext(pDisplay, bestFbc, GLX_RGBA_TYPE, nullptr, True);
    } else {
      context = glXCreateContextAttribsARB(pDisplay, bestFbc, nullptr, true, context_attribs);
    }
    XSync(pDisplay, False);

    // Verifying that context is a direct context
    if(!glXIsDirect(pDisplay, context)) {
      std::cout << "Indirect GLX rendering context obtained\n";
    } else {
      std::cout << "Direct GLX rendering context obtained\n";
    }
    glXMakeCurrent(pDisplay, window, context);
  }

  std::cout << "GL Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "GL Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

  // Show the window
  XClearWindow(pDisplay, window);
  XMapRaised(pDisplay, window);

  glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

  std::array<float, 4> clearColor = {0.1F, 0.1F, 0.15F, 1.F};
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

    // Present frame
    glXSwapBuffers(pDisplay, window);
  }

  // Cleanup GLX
  glXDestroyContext(pDisplay, context);

  // Cleanup X11
  XFree(visual);
  XFreeColormap(pDisplay, windowAttribs.colormap);
  XDestroyWindow(pDisplay, window);
  XCloseDisplay(pDisplay);
  return EXIT_SUCCESS;
}
