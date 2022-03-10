// STL
#include <array>
#include <chrono>
#include <vector>
#include <cstdlib>
// glbinding
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
// fmt
#include <fmt/color.h>
#include <fmt/printf.h>
// SDL2
#include <SDL2/SDL.h>
// GLM
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
// assimp
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

using namespace gl;

enum GL3D { SUCCESS = 0 };
enum class ShaderResult : std::uint32_t { FAILURE = std::numeric_limits<std::uint32_t>::max() };
enum class ProgramResult : std::uint32_t { FAILURE = std::numeric_limits<std::uint32_t>::max() };
enum class SDL_GL : int { ADAPTIVE_VSYNC = -1, IMMEDIATE = 0, SYNCHRONIZED = 1 };

enum class TimerType { CPU, GPU };
constexpr inline auto gTitle              = "Scene";
constexpr inline auto gWidth              = 640U;
constexpr inline auto gHeight             = 480U;
constexpr inline auto gMassageLength      = 1024U;
constexpr inline auto gMilisecond         = 1'000.F;
constexpr inline auto gNanosecond         = 1'000'000;
constexpr inline auto SDL_SUCCESS         = 0;
constexpr inline auto gOpenGLMinorVersion = 4;
constexpr inline auto gOpenGLMajorVersion = 5;

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

  fmt::print("{}\n", message);
}

// NOLINTNEXTLINE
static const char *vertexShaderSource = R"GLSL(
#version 450 core

layout (location = 0) in vec4 iPosition;

layout (location = 0) uniform mat4 MVP;
layout (location = 1) uniform vec4 uColor;

out vec4 vsColor;

void main() {
  vsColor = uColor;
  gl_Position = MVP * iPosition;
}
)GLSL";

// NOLINTNEXTLINE
static const char *fragmentShaderSource = R"GLSL(
#version 450 core

layout (location = 0) out vec4 oColor;

in vec4 vsColor;

void main() {
  oColor = vsColor;
}
)GLSL";

struct Model {
  GLuint vao = 0;
  GLuint vbo[3] = {}; // NOLINT
  void draw(GLenum type = GL_TRIANGLES) const {
    glBindVertexArray(vao);
    { glDrawArrays(type, 0, static_cast<int>(mVertices.size() / 3)); }
    glBindVertexArray(0);
  }
  std::vector<float> mVertices;
};

struct Scene {
  std::vector<Model> mModels;

  void initialize() {
    for(auto &model : mModels) {
      glGenVertexArrays(1, &model.vao);
      glBindVertexArray(model.vao);
      {
        glCreateBuffers(2, model.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, model.mVertices.size() * 3 * sizeof(float), model.mVertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
      }
      glBindVertexArray(0);
    }
  }

  void draw(GLenum type = GL_TRIANGLES) const {
    for(const auto &model : mModels) {
      model.draw(type);
    }
  }
};

static auto LoadMesh(const aiMesh *pMesh) -> Model {
  Model model;
  model.mVertices.resize(pMesh->mNumVertices * 3);

  auto *pVertex = reinterpret_cast<glm::vec3 *>(model.mVertices.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  for(auto i = 0U; i < pMesh->mNumVertices; i++) {
    const aiVector3D *pPos = &(pMesh->mVertices[i]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    *pVertex = {pPos->x, pPos->y, pPos->z};
    ++pVertex; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }
  return model;
}

static auto LoadFile(const std::string &fileName) -> Scene {
  Assimp::Importer importer;
  const auto *pScene = importer.ReadFile(fileName, 0);
  if(pScene == nullptr) {
    fmt::print(stderr, fg(fmt::color::red), "Can not load \"{}\"\n", fileName);
    std::exit(EXIT_FAILURE);
  }
  Scene scene;
  scene.mModels.reserve(pScene->mNumMeshes);
  for(auto i = 0U; i < pScene->mNumMeshes; ++i) {
    const auto &mesh = pScene->mMeshes[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    scene.mModels.push_back(LoadMesh(mesh));
  }
  return scene;
}

static auto checkShaderCompilation(GLuint shader) -> bool {
  GLint compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if(compiled != 1) {
    GLsizei log_length = 0;
    std::array<GLchar, gMassageLength> message = {};
    glGetShaderInfoLog(shader, gMassageLength, &log_length, message.data());
    fmt::print(stderr, fg(fmt::color::red), "{}\n", message.data());
    return false;
  }
  return true;
}

static auto createShader(GLenum shaderType, const char *shaderSource) -> GLuint {
  auto shader = glCreateShader(shaderType);
  auto *vertexShaderSourcePtr = &shaderSource;
  glShaderSource(shader, 1, vertexShaderSourcePtr, nullptr);
  glCompileShader(shader);
  if(!checkShaderCompilation(shader)) {
    return static_cast<std::uint32_t>(ShaderResult::FAILURE);
  }
  return shader;
}

static auto checkProgramLinkage(GLuint program) -> bool {
  GLint program_linked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
  if(program_linked != 1) {
    GLsizei log_length = 0;
    std::array<GLchar, gMassageLength> message = {};
    glGetProgramInfoLog(program, gMassageLength, &log_length, message.data());
    fmt::print(stderr, fg(fmt::color::red), "{}\n", message.data());
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
    return static_cast<std::uint32_t>(ProgramResult::FAILURE);
  }

  return program;
}

template<TimerType Type>
struct Timer;

template<>
struct Timer<TimerType::CPU> {
  void start() { mStart = std::chrono::high_resolution_clock::now(); }

  auto stop() -> long {
    const auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - mStart).count();
  }

  auto stopAndRestart() -> long {
    const auto time = stop();
    start();
    return time;
  }

private:
  std::chrono::high_resolution_clock::time_point mStart;
};

template<>
struct Timer<TimerType::GPU> {
  Timer() noexcept { glGenQueries(static_cast<int>(mQueries.size()), mQueries.data()); }

  void start() { glQueryCounter(mQueries[0], GL_TIMESTAMP); }

  auto stop() -> long {
    glQueryCounter(mQueries[1], GL_TIMESTAMP);
    // wait until the results are available
    int stopTimerAvailable = 0;
    while(stopTimerAvailable != 0) {
      glGetQueryObjectiv(mQueries[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
    }

    GLuint64 startTime = 0;
    GLuint64 stopTime  = 0;
    // get query results
    glGetQueryObjectui64v(mQueries[0], GL_QUERY_RESULT, &startTime);
    glGetQueryObjectui64v(mQueries[1], GL_QUERY_RESULT, &stopTime);

    return static_cast<long>(stopTime - startTime) / gNanosecond;
  }

  auto stopAndRestart() -> long {
    const auto time = stop();
    start();
    return time;
  }

  std::array<GLuint, 2> mQueries = {};
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  if(SDL_Init(SDL_INIT_VIDEO) != SDL_SUCCESS) {
    fmt::print(stderr, fg(fmt::color::red), "Can not initialize \"{}\"\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  // Enable Debug OpenGL
  int contextFlags = {};
  SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
  contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
  // OpenGL 3.3 Core Profile
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gOpenGLMinorVersion);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gOpenGLMajorVersion);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  auto *pWindow = SDL_CreateWindow(gTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWidth, gHeight, SDL_WINDOW_OPENGL);
  if(pWindow == nullptr) {
    fmt::print(stderr, fg(fmt::color::red), "Can not create a window \"{}\"\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  const auto context = SDL_GL_CreateContext(pWindow); // NOLINT
  if(context == nullptr) {
    fmt::print(stderr, fg(fmt::color::red), "Can not create a context \"{}\"\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  glbinding::initialize(nullptr, false);

  // Set OpenGL Debug Callback
  glDebugMessageCallback(DebugCallback, nullptr);

  // Enable Immediate update
  if(SDL_GL_SetSwapInterval(static_cast<int>(SDL_GL::SYNCHRONIZED)) != SDL_SUCCESS) {
    fmt::print(fg(fmt::color::yellow), "Can not set Immediate update!\n");
  }

  Scene scene = LoadFile("sphere.obj");
  scene.initialize();

  GLuint program = 0U;
  {
    auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if(vertexShader == static_cast<std::uint32_t>(ShaderResult::FAILURE) ||
       fragmentShader == static_cast<std::uint32_t>(ShaderResult::FAILURE)) {
      return EXIT_FAILURE;
    }
    program = createProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

  const auto ratio = static_cast<float>(gWidth) / static_cast<float>(gHeight);
  const auto prespective = glm::perspective(45.F, ratio, 0.001F, 1000.F);
  const auto view = glm::lookAt(glm::vec3{0, 0,-10}, glm::vec3{}, glm::vec3{0, 1, 0});

  const auto MVP = prespective * view;

  Timer<TimerType::CPU> cpuTimer;
  Timer<TimerType::GPU> gpuTimer;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);

  bool bRunning = true;
  while(bRunning) {
    SDL_Event event;
    while(SDL_PollEvent(&event) != 0) {
      if(event.type == SDL_QUIT) {
        bRunning = false;
      }
    }
    cpuTimer.start();
    gpuTimer.start();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
    {
      glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(MVP));
      glUniform4fv(1,       1, glm::value_ptr(glm::vec4(0.2F, 0.2F, 0.2F, 1.0F)));
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      scene.draw();
      glUniform4fv(1,       1, glm::value_ptr(glm::vec4(1.0F, 1.0F, 1.0F, 1.0F)));
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      scene.draw();
    }
    glUseProgram(0);

    SDL_GL_SwapWindow(pWindow);

    const auto cpuTime = static_cast<float>(cpuTimer.stop());
    const auto gpuTime = static_cast<float>(gpuTimer.stop());
    fmt::print("\rCPU: FPS: {:.2f}, Time: {:.2f}, GPU: FPS: {:.2f}, Time: {:.2f}",
           static_cast<double>(gMilisecond / cpuTime),
           static_cast<double>(cpuTime),
           static_cast<double>(gMilisecond / gpuTime),
           static_cast<double>(gpuTime));
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();
  return EXIT_SUCCESS;
}
