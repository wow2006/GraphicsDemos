// STL
#include <vector>
#include <cstdlib>
#include <iostream>
// SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

struct Model {
  void draw() const {}
};

struct Scene {
  std::vector<Model> mModels;
  void draw() const {
    for(const auto& model : mModels) {
      model.draw();
    }
  }
};

constexpr auto gTitle  = "Scene";
constexpr auto gWidth  = 640U;
constexpr auto gHeight = 480U;
constexpr auto SDL_SUCCESS = 0;

auto main() -> int {
  if(SDL_Init(SDL_INIT_VIDEO) != SDL_SUCCESS) {
    std::cerr << "Can not initialize \"" << SDL_GetError() << "\"\n";
    return EXIT_FAILURE;
  }

  // Enable Debug OpenGL
  int contextFlags;
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
  contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
  // OpenGL 3.3 Core Profile
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  auto pWindow = SDL_CreateWindow(gTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  gWidth, gHeight, SDL_WINDOW_OPENGL);
  if(pWindow == nullptr) {
    std::cerr << "Can not create window \"" << SDL_GetError() << "\"\n";
    return EXIT_FAILURE;
  }

  auto context = SDL_GL_CreateContext(pWindow);
  if(context == nullptr) {
    std::cerr << "Can not create context \"" << SDL_GetError() << "\"\n";
    return EXIT_FAILURE;
  }

  // Enable vsync
  SDL_GL_SetSwapInterval(1);

  Scene scene;

  bool bRunning = true;
  while(bRunning) {
    SDL_Event event;
    while(SDL_PollEvent(&event) != 0) {
      if(event.type == SDL_QUIT) {
        bRunning = false;
      }
    }

    scene.draw();

    SDL_GL_SwapWindow(pWindow);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();
  return EXIT_SUCCESS;
}
