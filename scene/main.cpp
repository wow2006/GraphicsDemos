// STL
#include <vector>
#include <cstdlib>
#include <iostream>
// GLM
#include <glm/vec3.hpp>
// GL3W
#include "GL/gl3w.h"
// SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
// assimp
#include <assimp/scene.h>
//#include <assimp/Defines.h>
#include <assimp/Importer.hpp>

static constexpr auto GL_SHADER_FAILURE  = std::numeric_limits<std::uint32_t>::max();
static constexpr auto GL_PROGRAM_FAILURE = std::numeric_limits<std::uint32_t>::max();

static const char *vertexShaderSource = R"GLSL(
#version 450 core

layout (location = 0) in vec4 iPosition;

void main() {
  gl_Position = iPosition;
}
)GLSL";

static const char *fragmentShaderSource = R"GLSL(
#version 450 core

layout (location = 0) out vec4 oColor;

void main() {
  oColor = vec4(1, 0, 0, 1);
}
)GLSL";

struct Model {
  GLuint vao;
  GLuint vbo[3];
  void draw() const {
      glBindVertexArray(vao);
      {
        glDrawArrays(GL_TRIANGLES, 0, mVertices.size() / 3);
      }
      glBindVertexArray(0);
  }
  std::vector<float> mVertices;
  std::vector<float> mNormals;
  std::vector<float> mTexturesCoords;
};

struct Scene {
  std::vector<Model> mModels;

  void initialize() {
    for(auto& model : mModels) {
      glGenVertexArrays(1, &model.vao);
      glBindVertexArray(model.vao);
      {
        glGenBuffers(1, model.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo[0]);
        {
          glBufferData(GL_ARRAY_BUFFER, model.mVertices.size() * 3 * sizeof(float),
                        model.mVertices.data(), GL_STATIC_DRAW);
          glEnableVertexAttribArray(0);
          glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
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

static auto LoadMesh(const aiMesh* pMesh) -> Model {
  Model model;
  model.mVertices.resize(pMesh->mNumVertices * 3);
  //model.mNormals.reserve(pMesh->mNumVertices * 3);
  //model.mTexturesCoords.reserve(pMesh->mNumVertices * 3);

  glm::vec3* pVertex = reinterpret_cast<glm::vec3*>(model.mVertices.data());
  for(auto i = 0U; i < pMesh->mNumVertices; i++) {
    const aiVector3D* pPos    = &(pMesh->mVertices[i]);
    //const aiVector3D* pNormal = &(pMesh->mNormals[i]) : &Zero3D;
    //const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
    *pVertex = {pPos->x, pPos->y, pPos->z};
    ++pVertex;
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
    const auto& mesh = pScene->mMeshes[i];
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

  gl3wInit();

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

  // Enable vsync
  SDL_GL_SetSwapInterval(1);

  Scene scene = LoadFile("cube.obj");
  scene.initialize();

  GLuint program = 0U;
  {
    auto vertexShader   = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if(vertexShader == GL_SHADER_FAILURE || fragmentShader == GL_SHADER_FAILURE) {
      return EXIT_FAILURE;
    }
    program = createProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

  bool bRunning = true;
  while(bRunning) {
    SDL_Event event;
    while(SDL_PollEvent(&event) != 0) {
      if(event.type == SDL_QUIT) {
        bRunning = false;
      }
    }

    glUseProgram(program);
    scene.draw();
    glUseProgram(0);

    SDL_GL_SwapWindow(pWindow);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();
  return EXIT_SUCCESS;
}
