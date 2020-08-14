// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// STL
#include <array>
#include <chrono>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <algorithm>
#include <string_view>
// GL3W
#include <GL/gl3w.h>
// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
// GLM
#include <glm/gtc/type_ptr.hpp>
// Internal
#include "firstPersonShootingCamera.hpp"

static constexpr auto GL3D_SUCCESS = 0;
static constexpr auto SDL_SUCCESS  = 0;

[[maybe_unused]] static constexpr auto SDL_IMMEDIATE_UPDATE = 0;
[[maybe_unused]] static constexpr auto SDL_SYNCHRONIZED_UPDATE = 1;
[[maybe_unused]] static constexpr auto SDL_ADAPTIVE_UPDATE = -1;

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

static void LogOutputFunction(void *userdata, int category, SDL_LogPriority priority, const char *message) {
  (void)userdata;
  (void)category;
  (void)priority;
  std::cout << "ERROR: " << message << std::endl;
}

static auto parseProgramOptions(int argc, char **argv) -> std::optional<std::pair<int, int>> {
  if(argc != 3) {
    return std::nullopt;
  }

  return std::make_pair(std::stoi(argv[1]), std::stoi(argv[2]));
}

static void drawText(const char *pMessage, glm::vec2 topLeft) {
  if(TTF_Init() != SDL_SUCCESS) {
    std::cerr << "Can not Initialize SDL2\n";
    std::exit(EXIT_FAILURE);
  }

  static auto pTitleFont = TTF_OpenFont("font/DroidSansMono.ttf", 48);
  if(pTitleFont == nullptr) {
    std::string ttferr = TTF_GetError();
    std::cerr << "Can not load \"DroidSansMono.ttf\": " << ttferr << '\n';
    std::exit(EXIT_FAILURE);
  }

  SDL_Color fg = {255, 128, 0, 255};
  auto pSurface = TTF_RenderText_Blended(pTitleFont, pMessage, fg);
  if(pSurface == nullptr) {
    std::string ttferr = TTF_GetError();
    std::cerr << "Can not create Blenderd Texture: " << ttferr << '\n';
    std::exit(EXIT_FAILURE);
  }

  GLuint colors = pSurface->format->BytesPerPixel;
  GLuint format;
  if(colors == 4) {  // alpha
    if(pSurface->format->Rmask == 0x000000ff)
      format = GL_RGBA;
    else
      format = GL_BGRA;
  } else {  // no alpha
    if(pSurface->format->Rmask == 0x000000ff)
      format = GL_RGB;
    else
      format = GL_BGR;
  }

  auto pPixels = reinterpret_cast<GLubyte *>(pSurface->pixels);
  GLuint hTex;
  glGenTextures(1, &hTex);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pSurface->w, pSurface->h, 0, format, GL_UNSIGNED_BYTE, pPixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(pSurface);
}

auto main(int argc, char *argv[]) -> int {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Can not initialize SDL2\n";
    return EXIT_FAILURE;
  }

  SDL_LogSetOutputFunction(LogOutputFunction, nullptr);

  int width  = 640;
  int height = 480;
  if(const auto args = parseProgramOptions(argc, argv); args) {
    constexpr auto firstScreenIndex = 0;
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(firstScreenIndex, &displayMode);

    width = std::min(args.value().first, displayMode.w);
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
  // OpenGL 4.5 Core Profile
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // Create OpenGL Context
  SDL_GLContext context = SDL_GL_CreateContext(pWindow);
  if(context == nullptr) {
    std::cerr << "Can not create SDL2 context\n";
    return EXIT_FAILURE;
  }
  SDL_GL_SetSwapInterval(SDL_IMMEDIATE_UPDATE);

  if(gl3wInit() != GL3D_SUCCESS) {
    std::cerr << "Can not initialize GL3W!\n";
    return EXIT_FAILURE;
  }

  // Set OpenGL Debug Callback
  if(glDebugMessageCallback) {
    std::cout << "Debug is enabled\n";
    glDebugMessageCallback(DebugCallback, nullptr);
  }

  GLuint vao = 0;
  std::array<GLuint, 3> vbo;
  {
    std::array<float, 42 * 3> verties{
      // Quad
      -10.F,
      0,
      10.F,  // 0
      10.F,
      0,
      10.F,  // 1
      10.F,
      0,
      -10.F,  // 2
      -10.F,
      0,
      10.F,  // 0
      10.F,
      0,
      -10.F,  // 2
      -10.F,
      0,
      -10.F,  // 3
      // Cube
      // Front
      -1.F,
      0,
      1.F,  // 4
      1.F,
      0,
      1.F,  // 5
      1.F,
      2.F,
      1.F,  // 6
      -1.F,
      0,
      1.F,  // 4
      1.F,
      2.F,
      1.F,  // 6
      -1.F,
      2.F,
      1.F,  // 7
            // Right
      1.F,
      0,
      1.F,  // 5
      1.F,
      0,
      -1.F,  // 8
      1.F,
      2,
      -1.F,  // 9
      1.F,
      0,
      1.F,  // 5
      1.F,
      2,
      -1.F,  // 9
      1.F,
      2.F,
      1.F,  // 6
            // Top
      -1.F,
      2.F,
      1.F,  // 7
      1.F,
      2.F,
      1.F,  // 6
      1.F,
      2,
      -1.F,  // 9
      -1.F,
      2.F,
      1.F,  // 7
      1.F,
      2,
      -1.F,  // 9
      -1.F,
      2,
      -1.F,  // 11
             // Back
      1.F,
      0,
      -1.F,  // 8
      -1.F,
      0,
      -1.F,  // 10
      -1.F,
      2,
      -1.F,  // 11
      1.F,
      0,
      -1.F,  // 8
      -1.F,
      2,
      -1.F,  // 11
      1.F,
      2,
      -1.F,  // 9
             // Left
      -1.F,
      0,
      -1.F,  // 10
      -1.F,
      0,
      1.F,  // 4
      -1.F,
      2.F,
      1.F,  // 7
      -1.F,
      0,
      -1.F,  // 10
      -1.F,
      2.F,
      1.F,  // 7
      -1.F,
      2,
      -1.F,  // 11
    };
    std::array<float, 42 * 2> uvcoords{
      // Quad
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
      // Cube
      // Front side (0)
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
      // Front side (0)
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
      // Front side (0)
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
      // Front side (0)
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
      // Front side (0)
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
    };
    std::array<float, 42 * 3> normals{
      // Quad
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      // Cube
      // Front
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      // Right
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      // Top
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      // Back
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      // Left
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(vbo.size(), vbo.data());
    glBindVertexArray(vao);
    {
      glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verties.size(), verties.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
      glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvcoords.size(), uvcoords.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);
      glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, nullptr);
    }
    //glBindVertexArray(0);
  }

  GLuint program = 0;
  {
    const char *vSource = R"(
      #version 450 core

      layout(location=0) in vec3 position;
      layout(location=1) in vec2 uvcoord;
      layout(location=2) in vec3 normal;

      out vec3 vNormal;
      out vec2 vUVCoord;

      uniform mat4 MVP;

      void main() {
        gl_Position = MVP * vec4(position, 1);
        vNormal = normal;
        vUVCoord = uvcoord;
      }
      )";
    auto vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vSource, nullptr);
    glCompileShader(vShader);
    GLint vertex_compiled;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertex_compiled);
    if(vertex_compiled != GL_TRUE) {
      GLsizei log_length = 0;
      GLchar message[1024];
      glGetShaderInfoLog(vShader, 1024, &log_length, message);
      std::cerr << "VertexShader: " << message << '\n';
      return EXIT_FAILURE;
    }

    const char *fSource = R"(
      #version 450 core

      in vec3 vNormal;
      in vec2 vUVCoord;

      out layout(location = 0) vec4 oFrag;

      void main() {
        oFrag = vec4(vUVCoord, 0, 1.0);
      }
      )";
    auto fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fSource, nullptr);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &vertex_compiled);
    if(vertex_compiled != GL_TRUE) {
      GLsizei log_length = 0;
      GLchar message[1024];
      glGetShaderInfoLog(fShader, 1024, &log_length, message);
      std::cerr << "FragmentShader: " << message << '\n';
      return EXIT_FAILURE;
    }

    program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);
    GLint program_linked;
    glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
    if(program_linked != GL_TRUE) {
      GLsizei log_length = 0;
      GLchar message[1024];
      glGetProgramInfoLog(program, 1024, &log_length, message);
      std::cerr << "Program: " << message << '\n';
      return EXIT_FAILURE;
    }
  }

  glEnable(GL_DEPTH_TEST);

  FPSCamera camera;

  char message[512] = "HelloWorld!";

  GLuint query[2];
  glGenQueries(2, query);
  float delta = 0.F;
  auto bRunning = true;
  while(bRunning) {
    SDL_Event event;
    while(SDL_PollEvent(&event) > 0) {
      switch(event.type) {
      case SDL_QUIT: bRunning = false; break;
      case SDL_WINDOWEVENT: break;
      case SDL_KEYUP:
      case SDL_KEYDOWN: break;
      case SDL_MOUSEWHEEL: break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP: {
        const auto mouseButtonEvent = event.button;
        std::cout << "(" << mouseButtonEvent.x << ", " << mouseButtonEvent.y << ")\n";
      } break;
      case SDL_MOUSEMOTION: break;
      }
    }
    glQueryCounter(query[0], GL_TIMESTAMP);
    const auto startTime = std::chrono::high_resolution_clock::now();

    camera.update(delta);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glUseProgram(program);
      {
        glBindVertexArray(vao);
        static GLint viewAttribPosition = glGetUniformLocation(program, "MVP");
        const auto projection = glm::perspective(glm::radians(60.F), 4.F / 3.F, 0.1F, 1000.F);
        const auto view = glm::lookAt(glm::vec3(10, 10, 10), glm::vec3{}, glm::vec3(0, 1, 0));
        const auto MVP = projection * view;
        glUniformMatrix4fv(viewAttribPosition, 1, GL_FALSE, glm::value_ptr(MVP));

        glDrawArrays(GL_TRIANGLES, 0, 42);
      }
      glUseProgram(0);

      drawText(message, glm::vec2{0, 0});
    }
    SDL_GL_SwapWindow(pWindow);

    glQueryCounter(query[1], GL_TIMESTAMP);
    const auto stopTime = std::chrono::high_resolution_clock::now();

    delta = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();

    // wait until the results are available
    int stopTimerAvailable = 0;
    while(!stopTimerAvailable) {
      glGetQueryObjectiv(query[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
    }

    GLuint64 startTimeGL, stopTimeGL;
    // get query results
    glGetQueryObjectui64v(query[0], GL_QUERY_RESULT, &startTimeGL);
    glGetQueryObjectui64v(query[1], GL_QUERY_RESULT, &stopTimeGL);

    snprintf(message, 512, "Frame %+3.3F/%+3.3F ms", delta, static_cast<float>(stopTimeGL - startTimeGL) / 1000000.F);
  }

  glBindVertexArray(0);
  glDeleteVertexArrays(1, &vao);

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();

  return EXIT_SUCCESS;
}
