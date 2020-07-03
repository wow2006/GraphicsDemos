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

inline auto readTextFile(const std::string& fileName) -> std::string {
  std::ifstream inputStream(fileName, std::ios::ate);
  if(!inputStream.is_open()) {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "ERROR: Can nor read \"{0}\" file!\n", fileName);
    return {};
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
  constexpr std::array ignoreWarrings = {33350};

  if(std::any_of(std::cbegin(ignoreWarrings), std::cend(ignoreWarrings), [source](auto current) -> bool {
       return static_cast<int>(source) == current;
     })) {
    return;
  }

  std::cout << source << " : " << message << '\n';
}

class Engine final {
public:
  void initialize() {
    if(SDL_Init(SDL_INIT_VIDEO) != SDL_SUCCESS) {
      throw std::runtime_error(fmt::format("Can not initialize \"{}\"", SDL_GetError()));
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

    m_pWindow = SDL_CreateWindow(gTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWidth, gHeight, SDL_WINDOW_OPENGL);
    if(m_pWindow == nullptr) {
      throw std::runtime_error(fmt::format("Can not create window \"{}\"", SDL_GetError()));
    }

    auto context = SDL_GL_CreateContext(m_pWindow);
    if(context == nullptr) {
      throw std::runtime_error(fmt::format("Can not create context \"{}\"", SDL_GetError()));
    }

    if(gl3wInit() != GL3D::SUCCESS) {
      throw std::runtime_error("Can not initialize GL3W!");
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
  }

  void createBuffer() {
    glCreateVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    constexpr std::array data = {
     0.25f, -0.25f, 0.5f, 1.0f,
    -0.25f, -0.25f, 0.5f, 1.0f,
     0.25f,  0.25f, 0.5f, 1.0f
    };

    glCreateBuffers(1, &mVBO);
    glNamedBufferStorage(mVBO, sizeof(float) * data.size(), nullptr, GL_MAP_WRITE_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    void* ptr = glMapNamedBuffer(mVBO, GL_WRITE_ONLY);
    std::memcpy(ptr, data.data(), sizeof(float) * data.size());
    glUnmapNamedBuffer(mVBO);

    glVertexArrayVertexBuffer(mVAO, 0, 0, sizeof(float) * 4, 0);
    glVertexArrayAttribFormat(mVAO, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(mVAO, 0);

    glBindVertexArray(0);
  }

  void createProgram() {
    const auto vertexShaderSource = readTextFile("shaders/simple_vertex.glsl");
    const auto pVertexShaderSource = vertexShaderSource.data();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &pVertexShaderSource, nullptr);
    GLuint vsProgram = glCreateProgram();
    glAttachShader(vsProgram, vertexShader);
    glProgramParameteri(vsProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glLinkProgram(vsProgram);

    const auto fragmentShaderSource = readTextFile("shaders/simple_fragment.glsl");
    const auto pFragmentShaderSource = fragmentShaderSource.data();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &pFragmentShaderSource, nullptr);
    GLuint fsProgram = glCreateProgram();
    glAttachShader(fsProgram, fragmentShader);
    glProgramParameteri(fsProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glLinkProgram(fsProgram);
  }

  void draw() {
    bool bRunning = true;
    while(bRunning) {
      SDL_Event event;
      while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
          bRunning = false;
        }
      }

      glBindVertexArray(mVAO);
      glBindBuffer(GL_ARRAY_BUFFER, mVBO);
      //glDrawArrays(GL_TRIANGLES, 0, 3);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
    }
  }

  SDL_Window *m_pWindow = nullptr;
  GLuint mVAO;
  GLuint mVBO;
  GLuint mProgram;
};

int main() {
  try {
    Engine engine;
    engine.initialize();
    engine.createBuffer();
    engine.createProgram();
    engine.draw();
  } catch(const std::runtime_error& error) {
    std::cerr << error.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
