// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
// STL
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <exception>
// GL3W
#include <GL/gl3w.h>
// SDL2
#include <SDL2/SDL.h>


enum GL3D { SUCCESS = 0 };
enum class SDL_GL : int { ADAPTIVE_VSYNC = -1, IMMEDIATE = 0, SYNCHRONIZED = 1 };
constexpr auto gTitle = "Scene";
constexpr auto gWidth = 640U;
constexpr auto gHeight = 480U;
constexpr auto SDL_SUCCESS = 0;
constexpr auto OpenGLMajorVersion = 4;
constexpr auto OpenGLMinorVersion = 5;
constexpr auto BUFFER_SIZE = 128U;

inline auto readTextFile(const std::string& fileName) -> std::string {
  std::ifstream inputStream(fileName, std::ios::ate);
  if(!inputStream.is_open()) {
    std::array<char, BUFFER_SIZE> buffer = {};
    snprintf(buffer.data(), buffer.size(), "ERROR: Can nor read \"%s\" file!", fileName.data());
    throw std::runtime_error(buffer.data());
  }
  const auto fileSize = inputStream.tellg();
  inputStream.seekg(0, std::ios::beg);
  std::string output;
  output.resize(static_cast<size_t>(fileSize));
  inputStream.read(output.data(), static_cast<std::streamsize>(output.size()));
  return output;
}

static void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid *pUserParam) {
  (void)type;
  (void)id;
  (void)severity;
  (void)length;
  (void)pUserParam;
  //constexpr std::array ignoreWarrings = {33350};

  //if(std::any_of(std::cbegin(ignoreWarrings), std::cend(ignoreWarrings), [source](auto current) -> bool {
  //     return static_cast<int>(source) == current;
  //   })) {
  //  return;
  //}

  //fmt::print(fg(fmt::color::red), "Error: {0} - {1}\n", source, message);
}

class Engine final {
public:
  void initialize() {
    if(SDL_Init(SDL_INIT_VIDEO) != SDL_SUCCESS) {
      std::array<char, BUFFER_SIZE> buffer = {};
      snprintf(buffer.data(), buffer.size(), "Can not initialize \"%s\"", SDL_GetError());
      throw std::runtime_error(buffer.data());
    }

    // Enable Debug OpenGL
    int contextFlags = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
    contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
    // OpenGL 4.5 Core Profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OpenGLMajorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OpenGLMinorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_pWindow = SDL_CreateWindow(gTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWidth, gHeight, SDL_WINDOW_OPENGL);
    if(m_pWindow == nullptr) {
      std::array<char, BUFFER_SIZE> buffer = {};
      snprintf(buffer.data(), buffer.size(), "Can not create window \"%s\"", SDL_GetError());
      throw std::runtime_error(buffer.data());
    }

    mContext = SDL_GL_CreateContext(m_pWindow);
    if(mContext == nullptr) {
      std::array<char, BUFFER_SIZE> buffer = {};
      snprintf(buffer.data(), buffer.size(), "Can not create context \"%s\"", SDL_GetError());
      throw std::runtime_error(buffer.data());
    }

    if(gl3wInit() != GL3D::SUCCESS) {
      throw std::runtime_error("Can not initialize GL3W!");
    }

    // Set OpenGL Debug Callback
    if(glDebugMessageCallback) {
      std::cout << "Debug is enabled\n";
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(DebugCallback, nullptr);
    }

    // Enable Immediate update
    if(SDL_GL_SetSwapInterval(static_cast<int>(SDL_GL::SYNCHRONIZED)) != SDL_SUCCESS) {
      std::cerr << "Can not set Immediate update!\n";
    }
  }

  static GLuint CreateProgramFromShader(const std::string& shaderName, GLenum shaderType) {
    const auto shaderSource = readTextFile(shaderName);
    const auto *pShaderSource = shaderSource.data();
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &pShaderSource, nullptr);
    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glLinkProgram(program);
    glDeleteShader(shader);

    GLint program_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
    if (program_linked != GL_TRUE) {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(program, 1024, &log_length, message);
        throw std::runtime_error(message);
    }
    return program;
  }

  void createProgram() {
    vsProgram = CreateProgramFromShader("shaders/vertexVBO2.glsl" , GL_VERTEX_SHADER);
    fsProgram = CreateProgramFromShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);

    glCreateProgramPipelines(1, &mProgram);
    glUseProgramStages(mProgram, GL_VERTEX_SHADER_BIT,   vsProgram);
    glUseProgramStages(mProgram, GL_FRAGMENT_SHADER_BIT, fsProgram);
    glBindProgramPipeline(mProgram);
  }

  void createBuffers() {
    glCreateVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    constexpr std::array<float, 12 * 2> cTriangleBuffer = {
       0.0F, 1.0F, 0.0F, 1.0F, 1.0F, 0.0F, 0.0F, 1.0F,
       1.0F,-1.0F, 0.0F, 1.0F, 0.0F, 1.0F, 0.0F, 1.0F,
      -1.0F,-1.0F, 0.0F, 1.0F, 0.0F, 0.0F, 1.0F, 1.0F
    };

    glCreateBuffers(1, &mVBO);
    glNamedBufferStorage(mVBO, sizeof(float) * cTriangleBuffer.size(), cTriangleBuffer.data(), GL_DYNAMIC_STORAGE_BIT);

    constexpr uint32_t positionAttrib  = 0;
    constexpr uint32_t positionBinding = 0;

    constexpr uint32_t colorAttrib     = 1;
    constexpr uint32_t colorBinding    = 1;

    glVertexArrayAttribBinding(mVAO, positionAttrib, positionBinding);
    glVertexArrayAttribBinding(mVAO, colorAttrib,    colorBinding);
    {
      GLuint buffers[2] = {
        mVBO,
        mVBO
      };
      GLintptr offsets[2] = {
        0, 0
      };
      GLsizei strides[2] = {
        sizeof(float) * 8,
        sizeof(float) * 8
      };

      glVertexArrayVertexBuffers(
          mVAO,
          positionBinding,
          2,
          buffers,
          offsets,
          strides
      );
    }
    glVertexArrayAttribFormat(mVAO, positionAttrib, 4, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(mVAO,  positionAttrib);
    glVertexArrayAttribFormat(mVAO, colorAttrib,    4, GL_FLOAT, GL_FALSE, sizeof(float) * 4);
    glEnableVertexArrayAttrib(mVAO,  colorAttrib);
  }

  void draw() const {
    bool bRunning = true;
    while(bRunning) {
      SDL_Event event;
      while(SDL_PollEvent(&event) != SDL_SUCCESS) {
        if(event.type == SDL_QUIT) {
          bRunning = false;
        }
      }

      glDrawArrays(GL_TRIANGLES, 0, 3);
      SDL_GL_SwapWindow(m_pWindow);
    }
  }

  void cleanup() {
    glDeleteProgram(vsProgram);
    glDeleteProgram(fsProgram);
    glDeleteProgramPipelines(1, &mProgram);
    glDeleteVertexArrays(1, &mVAO);
    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(m_pWindow);
    SDL_Quit();
  }

  SDL_Window *m_pWindow = nullptr;
  SDL_GLContext mContext = nullptr;
  GLuint vsProgram = 0;
  GLuint fsProgram = 0;
  GLuint mVAO = 0;
  GLuint mVBO = 0;
  GLuint mProgram = 0;
};

int main(int argc, char* argv[]) {
  try {
    Engine engine;
    engine.initialize();
    engine.createProgram();
    engine.createBuffers();
    engine.draw();
    engine.cleanup();
  } catch(const std::runtime_error& error) {
    std::cerr << error.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
