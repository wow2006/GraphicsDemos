// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// STL
#include <algorithm>
#include <array>
#include <optional>
#include <cstdlib>
#include <iostream>
#include <string_view>
// GL3W
#include <GL/gl3w.h>
// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
// Internal
#include "firstPersonShootingCamera.hpp"

static constexpr auto GL3D_SUCCESS = 0;

static void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid *pUserParam) {
  (void)type;
  (void)id;
  (void)severity;
  (void)length;
  (void)pUserParam;
  constexpr std::array ignoreWarrings = {33350};

  if(std::any_of(std::cbegin(ignoreWarrings), std::cend(ignoreWarrings), [source](auto current) -> bool {
       return static_cast<int>(source) == current;
     })) {
    return;
  }

  std::cout << source << " : " << message << '\n';
}

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

  // Enable Debug OpenGL
  int contextFlags;
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
  contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
  // OpenGL 3.3 Core Profile
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

  // Create OpenGL Context
  SDL_GLContext context = SDL_GL_CreateContext(pWindow);
  if(context == nullptr) {
    std::cerr << "Can not create SDL2 context\n";
    return EXIT_FAILURE;
  }
  // Enable vsync
  SDL_GL_SetSwapInterval(1);

  if(gl3wInit() != GL3D_SUCCESS) {
    std::cerr << "Can not initialize GL3W!\n";
    return EXIT_FAILURE;
  }

  // Set OpenGL Debug Callback
  if(glDebugMessageCallback) {
    std::cout << "Debug is enabled\n";
    glDebugMessageCallback(DebugCallback, nullptr);
  }

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

