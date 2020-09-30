// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// STL
#include <array>
#include <string>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <algorithm>
#include <string_view>
// GLM
#include <glm/vec4.hpp>
// GL3W
#include <GL/gl3w.h>
// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

static constexpr auto GL3D_SUCCESS       = 0;
static constexpr auto GL_SHADER_FAILURE  = std::numeric_limits<std::uint32_t>::max();
static constexpr auto GL_PROGRAM_FAILURE = std::numeric_limits<std::uint32_t>::max();

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

static auto parseProgramOptions(int argc, char **argv) -> std::optional<std::pair<int, int>> {
  if(argc != 3) {
    return std::nullopt;
  }

  return std::make_pair(std::stoi(argv[1]), std::stoi(argv[2]));
}

static auto checkShaderCompilation(GLuint shader) -> bool {
  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if(compiled != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(shader, 1024, &log_length, message);
    std::cerr << "ERROR: " << message << '\n';
    return false;
  }
  return true;
}

static auto createShader(GLenum shaderType, const char *shaderSource) -> GLuint {
  auto shader = glCreateShader(shaderType);
  auto vertexShaderSourcePtr = &shaderSource;
  glShaderSource(shader, 1, vertexShaderSourcePtr, nullptr);
  glCompileShader(shader);
  if(!checkShaderCompilation(shader)) {
    return std::numeric_limits<std::uint32_t>::max();
  }
  return shader;
}

static auto checkProgramLinkage(GLuint program) -> bool {
  GLint program_linked;
  glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
  if(program_linked != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetProgramInfoLog(program, 1024, &log_length, message);
    std::cerr << "ERROR: " << message << '\n';
    return false;
  }
  return true;
}

static auto createProgram(GLuint vertexShader, GLuint fragmentShader) -> GLuint {
  auto program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  if(!checkProgramLinkage(program)) {
    return GL_PROGRAM_FAILURE;
  }

  return program;
}

auto main(int argc, char *argv[]) -> int {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Can not initialize SDL2\n";
    return EXIT_FAILURE;
  }

  int width = 640;
  int height = 480;
  if(const auto args = parseProgramOptions(argc, argv); args) {
    constexpr auto firstScreenIndex = 0;
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(firstScreenIndex, &displayMode);

    width = std::min(args.value().first, displayMode.w);
    height = std::min(args.value().second, displayMode.h);
  }

  constexpr std::string_view sWindowTitle = "RedTriangle";
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

  // ============================================================
  // Create Shader
  // ============================================================
  const char *vertexShaderSource = R"GLSL(
  #version 450 core

  vec4 cPositions[3] = {
    vec4(-0.5,-0.5, 0, 1),
    vec4( 0.5,-0.5, 0, 1),
    vec4( 0.0, 0.5, 0, 1)
  };

  void main() {
    gl_Position = cPositions[gl_VertexID];
  }
  )GLSL";

  const char *fragmentShaderSource = R"GLSL(
  #version 450 core

  layout (location = 0) out vec4 oColor;

  void main() {
    oColor = vec4(1, 0, 0, 1);
  }
  )GLSL";

  glEnable(GL_PROGRAM_POINT_SIZE);
  glPointSize(10.F);

  auto vertexShader   = createShader(GL_VERTEX_SHADER, vertexShaderSource);
  auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
  if(vertexShader == GL_SHADER_FAILURE || fragmentShader == GL_SHADER_FAILURE) {
    return EXIT_FAILURE;
  }
  auto program = createProgram(vertexShader, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // ============================
  // Must create one VAO at lest.
  // ============================
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  constexpr glm::vec4 clearColor = {0.F, 0.F, 0.F, 1.F};
  auto bRunning = true;
  while(bRunning) {
    SDL_Event event;
    while(SDL_PollEvent(&event) > 0) {
      switch(event.type) {
      case SDL_QUIT: bRunning = false; break;
      }
    }

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    {
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    glUseProgram(0);

    SDL_GL_SwapWindow(pWindow);
  }

  glDeleteVertexArrays(1, &vao);

  glDeleteProgram(program);

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();

  return EXIT_SUCCESS;
}
