// STL
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <exception>
// FMT
#include <fmt/color.h>
#include <fmt/format.h>
// glbinding
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
// SDL2
#include <SDL2/SDL.h>

using namespace gl;

enum GL3D { SUCCESS = 0 };
enum class SDL_GL : int { ADAPTIVE_VSYNC = -1, IMMEDIATE = 0, SYNCHRONIZED = 1 };
constexpr auto gTitle = "Scene";
constexpr auto gWidth = 640U;
constexpr auto gHeight = 480U;
constexpr auto gMessageLength = 1024U;
constexpr auto gMajorVersion = 4;
constexpr auto gMinorVersion = 5;
inline const bool gDebugOpenGL = std::getenv("DEBUG_OPENGL") != nullptr;
constexpr auto SDL_SUCCESS = 0;

inline auto readTextFile(const std::string& fileName) -> std::string {
  std::ifstream inputStream(fileName, std::ios::ate);
  if(!inputStream.is_open()) {
    throw std::runtime_error(fmt::format("ERROR: Can nor read \"{0}\" file!\n", fileName));
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

  fmt::print(fg(fmt::color::red), "Error: {0} - {1}\n", static_cast<int>(source), message);
}

class Engine final {
public:
  void initialize() {
    if(SDL_Init(SDL_INIT_VIDEO) != SDL_SUCCESS) {
      throw std::runtime_error(fmt::format("Can not initialize \"{}\"", SDL_GetError()));
    }

    // Enable Debug OpenGL
    int contextFlags = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
    contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
    // OpenGL 4.5 Core Profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gMajorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gMinorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_pWindow = SDL_CreateWindow(gTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWidth, gHeight, SDL_WINDOW_OPENGL);
    if(m_pWindow == nullptr) {
      throw std::runtime_error(fmt::format("Can not create window \"{}\"", SDL_GetError()));
    }

    mContext = SDL_GL_CreateContext(m_pWindow);
    if(mContext == nullptr) {
      throw std::runtime_error(fmt::format("Can not create context \"{}\"", SDL_GetError()));
    }

    glbinding::initialize(nullptr, false);

    // Set OpenGL Debug Callback
    if(gDebugOpenGL) {
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
    if (program_linked != 1) {
        GLsizei log_length = 0;
        std::array<GLchar, gMessageLength> message{};
        glGetProgramInfoLog(program, gMessageLength, &log_length, message.data());
        throw std::runtime_error(message.data());
    }
    return program;
  }

  void createProgram() {
    vsProgram = CreateProgramFromShader("shaders/simple_vertex.glsl" , GL_VERTEX_SHADER);
    fsProgram = CreateProgramFromShader("shaders/simple_fragment.glsl", GL_FRAGMENT_SHADER);

    glCreateProgramPipelines(1, &mProgram);
    glUseProgramStages(mProgram, GL_VERTEX_SHADER_BIT,   vsProgram);
    glUseProgramStages(mProgram, GL_FRAGMENT_SHADER_BIT, fsProgram);
    glBindProgramPipeline(mProgram);
  }

  void createBuffers() {
    glCreateVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
  }

  void draw() const {
    bool bRunning = true;
    while(bRunning) {
      SDL_Event event;
      while(SDL_PollEvent(&event) != 0) {
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
  GLuint vsProgram= 0;
  GLuint fsProgram = 0;
  GLuint mVAO = 0;
  GLuint mProgram = 0;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
  try {
    Engine engine;
    engine.initialize();
    engine.createBuffers();
    engine.createProgram();
    engine.draw();
    engine.cleanup();
  } catch(const std::runtime_error& error) {
    std::cerr << error.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
