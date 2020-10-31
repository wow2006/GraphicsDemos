// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// STL
#include <optional>
#include <cstdlib>
#include <iostream>
// X11
#include <X11/Xlib.h>

static auto parseProgramOptions(int argc, char** argv)
  -> std::optional<std::pair<uint32_t, uint32_t>> {
  if(argc != 3) {
    return std::nullopt;
  }

  return std::make_pair(
    std::stoi(argv[1]),
    std::stoi(argv[2])
  );
}

auto main(int argc, char *argv[]) -> int {
  auto pDisplay = XOpenDisplay(nullptr);
  if(pDisplay == nullptr) {
    fprintf(stderr, "Cannot open display\n");
    return EXIT_FAILURE;
  }

  auto width  = 640U;
  auto height = 480U;
  if(const auto args = parseProgramOptions(argc, argv);
     args) {
    constexpr auto primaryDisplay = 0;
    auto screen = ScreenOfDisplay(pDisplay, primaryDisplay);
    width  = std::min(args.value().first,  static_cast<uint32_t>(screen->width));
    height = std::min(args.value().second, static_cast<uint32_t>(screen->height));
  }

  auto screen = DefaultScreen(pDisplay);
  auto window = XCreateSimpleWindow(pDisplay,
                RootWindow(pDisplay, screen),
                0, 0, width, height, 1,
                WhitePixel(pDisplay, screen),
                BlackPixel(pDisplay, screen));
  XSelectInput(pDisplay, window, ExposureMask | KeyPressMask);
  XMapWindow(pDisplay, window);

  std::cout << "Press any key to close the window!\n";

  auto bRunning = true;
  XEvent event;
  while(bRunning) {
    XNextEvent(pDisplay, &event);
    if (event.type == KeyPress) {
      bRunning = false;
    }
  }

  XCloseDisplay(pDisplay);
  return EXIT_SUCCESS;
}
