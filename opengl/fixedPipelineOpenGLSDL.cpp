// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// STL
#include <array>
#include <string>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <string_view>
// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

static auto parseProgramOptions(int argc, char** argv)
  -> std::optional<std::pair<int, int>> {
  if(argc != 3) {
    return std::nullopt;
  }

  return std::make_pair(
    std::stoi(argv[1]),
    std::stoi(argv[2])
  );
}

auto main(int argc, char *argv[]) -> int {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Can not initialize SDL2\n";
    return EXIT_FAILURE;
  }

  int width  = 640;
  int height = 480;
  if(const auto args = parseProgramOptions(argc, argv);
     args) {
    constexpr auto firstScreenIndex = 0;
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(firstScreenIndex, &displayMode);

    width  = std::min(args.value().first, displayMode.w);
    height = std::min(args.value().second, displayMode.h);
  }

  constexpr std::string_view sWindowTitle = "HelloWorld!";
  auto pWindow = SDL_CreateWindow(sWindowTitle.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);
  if(pWindow == nullptr) {
    std::cerr << "Can not create SDL2 window\n";
    return EXIT_FAILURE;
  }
  SDL_GLContext context = SDL_GL_CreateContext(pWindow);

  constexpr std::array<float, 4> clearColor = {0.F, 0.F, 0.F, 1.F};
  auto bRunning = true;
  while(bRunning) {
    SDL_Event event;
    while(SDL_PollEvent(&event) > 0) {
      switch(event.type) {
      case SDL_QUIT: bRunning = false; break;
      }
    }

    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(pWindow);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();

  return EXIT_SUCCESS;
}
