// STL
#include <array>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <iostream>
// GL3W
#include <GL/gl3w.h>
// SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
// GLM
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
// assimp
#include <assimp/scene.h>
//#include <assimp/Defines.h>
#include <assimp/Importer.hpp>

enum GL3D { SUCCESS = 0 };
enum class ShaderResult : std::uint32_t { FAILURE = std::numeric_limits<std::uint32_t>::max() };
enum class ProgramResult : std::uint32_t { FAILURE = std::numeric_limits<std::uint32_t>::max() };
enum class SDL_GL : int { ADAPTIVE_VSYNC = -1, IMMEDIATE = 0, SYNCHRONIZED = 1 };

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

static const char *vertexShaderSource = R"GLSL(
#version 330 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;

struct Matrices {
mat3 Normal;
mat4 ModelView;
mat4 ModelViewProjection;
};
uniform Matrices uMatrices;

smooth out vec3 oNormal;
smooth out vec3 oPosition;

void main() {
  oNormal     = normalize(uMatrices.Normal * iNormal);
  oPosition   = vec3(uMatrices.ModelView * vec4(iPosition, 1));
  gl_Position = uMatrices.ModelViewProjection * vec4(iPosition, 1);
}
)GLSL";

static const char *fragmentShaderSource = R"GLSL(
#version 330 core

struct Material {
vec3 Ambient;
vec3 Diffuse;
};
uniform Material uMatrial;

struct Light {
vec3 Pos;
vec3 Color;
};
uniform Light uLight;

in vec3 oNormal;
in vec3 oPosition;
layout (location = 0) out vec4 oColor;

void main() {
  vec3 s        = normalize(uLight.Pos - oPosition);

  vec3 ambient = uMatrial.Ambient;
  vec3 diffuse = uLight.Color * uMatrial.Diffuse * max(dot(s, oNormal), 0);

  oColor = vec4(ambient + diffuse, 1.0);
}
)GLSL";

struct Model {
  GLuint vao;
  GLuint vbo[3];
  void draw() const {
    glBindVertexArray(vao);
    { glDrawArrays(GL_TRIANGLES, 0, count); }
    glBindVertexArray(0);
  }
  GLuint count;
  std::vector<float> mVertices;
  std::vector<float> mNormals;
  std::vector<float> mTexturesCoords;
};

struct Scene {
  std::vector<Model> mModels;

  void initialize() {
    for(auto &model : mModels) {
      model.count = model.mVertices.size()/3;
      glGenVertexArrays(1, &model.vao);
      glBindVertexArray(model.vao);
      {
        glGenBuffers(2, model.vbo);
        {
          constexpr auto VERTEX_ATTRIBUTE = 0U;
          glBindBuffer(GL_ARRAY_BUFFER, model.vbo[VERTEX_ATTRIBUTE]);
          glBufferData(GL_ARRAY_BUFFER, model.mVertices.size() * sizeof(float), model.mVertices.data(), GL_STATIC_DRAW);
          glVertexAttribPointer(VERTEX_ATTRIBUTE, 3, GL_FLOAT, false, 0, nullptr);
          glEnableVertexAttribArray(VERTEX_ATTRIBUTE);
        }

        {
          constexpr auto NORMAL_ATTRIBUTE = 1U;
          glBindBuffer(GL_ARRAY_BUFFER, model.vbo[NORMAL_ATTRIBUTE]);
          glBufferData(GL_ARRAY_BUFFER, model.mNormals.size() * sizeof(float), model.mNormals.data(), GL_STATIC_DRAW);
          glVertexAttribPointer(NORMAL_ATTRIBUTE, 3, GL_FLOAT, false, 0, nullptr);
          glEnableVertexAttribArray(NORMAL_ATTRIBUTE);
        }
      }
      glBindVertexArray(0);
    }
  }

  void draw() const {
    for(const auto &model : mModels) {
      model.draw();
    }
  }
};

static auto LoadMesh(const aiMesh *pMesh) -> Model {
  Model model;
  model.mVertices.resize(pMesh->mNumVertices * 3);
  model.mNormals.resize(pMesh->mNumVertices * 3);
  //model.mTexturesCoords.reserve(pMesh->mNumVertices * 3);

  glm::vec3 *pVertex = reinterpret_cast<glm::vec3 *>(model.mVertices.data());
  glm::vec3 *pNormal = reinterpret_cast<glm::vec3 *>(model.mNormals.data());
  for(auto i = 0U; i < pMesh->mNumVertices; i++) {
    const aiVector3D *pPos = &(pMesh->mVertices[i]);
    const aiVector3D *pNrm = &(pMesh->mNormals[i]);
    //const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
    *pVertex = {pPos->x, pPos->y, pPos->z};
    *pNormal = {pNrm->x, pNrm->y, pNrm->z};
    ++pVertex;
    ++pNormal;
  }
  return model;
}

static auto LoadFile(const std::string &fileName) -> Scene {
  Assimp::Importer importer;
  const auto pScene = importer.ReadFile(fileName, 0);
  if(pScene == nullptr) {
    std::cerr << "Can not load \"" << fileName << "\"!\n";
    std::exit(EXIT_FAILURE);
  }
  Scene scene;
  scene.mModels.reserve(pScene->mNumMeshes);
  for(auto i = 0U; i < pScene->mNumMeshes; ++i) {
    const auto &mesh = pScene->mMeshes[i];
    scene.mModels.push_back(LoadMesh(mesh));
  }
  return scene;
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
    return static_cast<std::uint32_t>(ShaderResult::FAILURE);
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
    return static_cast<std::uint32_t>(ProgramResult::FAILURE);
  }

  return program;
}

enum class TimerType { CPU, GPU };

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
  Timer() { glGenQueries(2, mQueries); }

  void start() { glQueryCounter(mQueries[0], GL_TIMESTAMP); }

  auto stop() -> long {
    glQueryCounter(mQueries[1], GL_TIMESTAMP);
    // wait until the results are available
    int stopTimerAvailable = 0;
    while(!stopTimerAvailable) {
      glGetQueryObjectiv(mQueries[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
    }

    GLuint64 startTime, stopTime;
    // get query results
    glGetQueryObjectui64v(mQueries[0], GL_QUERY_RESULT, &startTime);
    glGetQueryObjectui64v(mQueries[1], GL_QUERY_RESULT, &stopTime);

    return static_cast<long>(stopTime - startTime) / 1000000;
  }

  auto stopAndRestart() -> long {
    const auto time = stop();
    start();
    return time;
  }

  GLuint mQueries[2];
};

constexpr auto gTitle = "Scene";
constexpr auto gWidth = 640U;
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
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  auto pWindow = SDL_CreateWindow(gTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWidth, gHeight, SDL_WINDOW_OPENGL);
  if(pWindow == nullptr) {
    std::cerr << "Can not create window \"" << SDL_GetError() << "\"\n";
    return EXIT_FAILURE;
  }

  auto context = SDL_GL_CreateContext(pWindow);
  if(context == nullptr) {
    std::cerr << "Can not create context \"" << SDL_GetError() << "\"\n";
    return EXIT_FAILURE;
  }

  if(gl3wInit() != GL3D::SUCCESS) {
    std::cerr << "Can not initialize GL3W!\n";
    return EXIT_FAILURE;
  }

  // Set OpenGL Debug Callback
  if(glDebugMessageCallback) {
    std::cout << "Debug is enabled\n";
    glDebugMessageCallback(DebugCallback, nullptr);
  }

  // Enable Immediate update
  if(SDL_GL_SetSwapInterval(static_cast<int>(SDL_GL::SYNCHRONIZED)) != SDL_SUCCESS) {
    std::cerr << "Can not set Immediate update!\n";
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

  const auto ratio       = static_cast<float>(gWidth) / static_cast<float>(gHeight);
  const auto prespective = glm::perspective(45.F, ratio, 0.001F, 1000.F);
  const auto view        = glm::lookAt(glm::vec3{2, 2, 2}, glm::vec3{}, glm::vec3{0, 1, 0});

  const auto MVP = prespective * view;

  const auto N = glm::mat3(glm::vec3(view[0]), glm::vec3(view[1]), glm::vec3(view[2]));

  auto locationMatrialAmbient              = glGetUniformLocation(program, "uMatrial.Ambient");
  auto locationMatrialDiffuse              = glGetUniformLocation(program, "uMatrial.Diffuse");
  auto locationMatricesNormal              = glGetUniformLocation(program, "uMatrices.Normal");
  auto locationMatricesModelView           = glGetUniformLocation(program, "uMatrices.ModelView");
  auto locationMatricesModelViewProjection = glGetUniformLocation(program, "uMatrices.ModelViewProjection");
  auto locationLightPos                    = glGetUniformLocation(program, "uLight.Pos");
  auto locationLightColor                  = glGetUniformLocation(program, "uLight.Color");

  Timer<TimerType::CPU> cpuTimer;
  Timer<TimerType::GPU> gpuTimer;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

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
      glUniform3fv(locationMatrialAmbient,                    1, glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
      glUniform3fv(locationMatrialDiffuse,                    1, glm::value_ptr(glm::vec3(1, 1, 1)));
      glUniform3fv(locationLightColor,                        1, glm::value_ptr(glm::vec3(1, 1, 1)));

      glUniform3fv(locationLightPos,                          1, glm::value_ptr(glm::vec3(10, 10, 10)));

      glUniformMatrix3fv(locationMatricesNormal,              1, GL_FALSE, glm::value_ptr(N));
      glUniformMatrix4fv(locationMatricesModelView,           1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(locationMatricesModelViewProjection, 1, GL_FALSE, glm::value_ptr(MVP));

      scene.draw();
    }
    glUseProgram(0);

    SDL_GL_SwapWindow(pWindow);

    const float cpuTime = static_cast<float>(cpuTimer.stop());
    const float gpuTime = static_cast<float>(gpuTimer.stop());
    printf("\rCPU: FPS: %.3F, Time: %.3F, GPU: FPS: %.3F, Time: %.3F",
           static_cast<double>(1000.F / cpuTime),
           static_cast<double>(cpuTime),
           static_cast<double>(1000.F / gpuTime),
           static_cast<double>(gpuTime));
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();
  return EXIT_SUCCESS;
}
