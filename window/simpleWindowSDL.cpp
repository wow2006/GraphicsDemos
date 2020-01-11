// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// STL
#include <optional>
#include <cstdlib>
#include <iostream>
#include <string_view>
// SDL
#include <SDL2/SDL.h>

static auto parseProgramOptions(int argc, char** argv)
  -> std::optional<std::pair<int, int>> {
  if(argc != 3) {
    std::cerr << "Usage:\n\t" << argv[0] << " width height\n";
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

  // NOTE: Not REQUIRED
  const auto firstOneSupporting = -1;
  const auto pRenderer = SDL_CreateRenderer(pWindow, firstOneSupporting, SDL_RENDERER_ACCELERATED);
  // NOTE: Not REQUIRED

  auto bRunning = true;
  while(bRunning) {
    SDL_Event event;
    while(SDL_PollEvent(&event) > 0) {
      switch(event.type) {
      case SDL_QUIT: bRunning = false; break;
      }
    }

    // NOTE: Not REQUIRED
    SDL_RenderClear(pRenderer);
    SDL_RenderPresent(pRenderer);
    // NOTE: Not REQUIRED
  }

  // NOTE: Not REQUIRED
  SDL_DestroyRenderer(pRenderer);
  // NOTE: Not REQUIRED
  SDL_DestroyWindow(pWindow);
  SDL_Quit();

  return EXIT_SUCCESS;
}
