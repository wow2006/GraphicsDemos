// STL
#include <array>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <exception>
// GL3W
#include <GL/gl3w.h>
// SDL2
#include <SDL2/SDL.h>
// stb
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


enum GL3D { SUCCESS = 0 };
enum class SDL_GL : int { ADAPTIVE_VSYNC = -1, IMMEDIATE = 0, SYNCHRONIZED = 1 };
constexpr auto gTitle = "UploadDownloadTexture";
constexpr auto gWidth = 640U;
constexpr auto gHeight = 480U;
constexpr auto SDL_SUCCESS = 0;
constexpr auto GL_MINOR = 5;
constexpr auto GL_MAJOR = 4;
constexpr auto VERTICES_TO_DRAW = 6;

constexpr auto TextureWidth    = 4;
constexpr auto TextureChannels = 3;
constexpr auto TextureHeight   = 4;
constexpr std::array<uint8_t, TextureWidth * TextureChannels * TextureHeight> g_cTexture = {
  255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 255,
  255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 255,
  255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 255,
  255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 255,
};

static void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid *pUserParam) {
  (void)type;
  (void)id;
  (void)severity;
  (void)length;
  (void)pUserParam;
  if(severity == GL_DEBUG_SEVERITY_HIGH) {
    std::printf("Error: %d - %s\n", source, message);
  }
}

inline auto readTextFile(const std::string& fileName) -> std::string {
  std::ifstream inputStream(fileName, std::ios::ate);
  if(!inputStream.is_open()) {
    constexpr auto BUFFER_SIZE = 265;
    std::array<char, BUFFER_SIZE> buffer = {};
    std::snprintf(buffer.data(), BUFFER_SIZE, "ERROR: Can not read \"%s\" file!\n", fileName.c_str());
    throw std::runtime_error(buffer.data());
  }
  const auto fileSize = inputStream.tellg();
  inputStream.seekg(0, std::ios::beg);
  std::string output;
  output.resize(static_cast<size_t>(fileSize));
  inputStream.read(output.data(), static_cast<std::streamsize>(output.size()));
  return output;
}

class Engine final {
public:
  void initialize() {
    if(SDL_Init(SDL_INIT_VIDEO) != SDL_SUCCESS) {
      constexpr auto BUFFER_SIZE = 265;
      std::array<char, BUFFER_SIZE> buffer = {};
      std::snprintf(buffer.data(), BUFFER_SIZE, "Can not initialize \"%s\"", SDL_GetError());
      throw std::runtime_error(buffer.data());
    }

    // Enable Debug OpenGL
    int contextFlags = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
    contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
    // OpenGL 4.5 Core Profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_pWindow = SDL_CreateWindow(gTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWidth, gHeight, SDL_WINDOW_OPENGL);
    if(m_pWindow == nullptr) {
      constexpr auto BUFFER_SIZE = 265;
      std::array<char, BUFFER_SIZE> buffer = {};
      std::snprintf(buffer.data(), BUFFER_SIZE, "Can not create window \"%s\"", SDL_GetError());
      throw std::runtime_error(buffer.data());
    }

    mContext = SDL_GL_CreateContext(m_pWindow);
    if(mContext == nullptr) {
      constexpr auto BUFFER_SIZE = 265;
      std::array<char, BUFFER_SIZE> buffer = {};
      std::snprintf(buffer.data(), BUFFER_SIZE, "Can not create context \"%s\"", SDL_GetError());
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

  static GLuint CreateProgramFromShader(const std::string &shaderName, GLenum shaderType) {
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
    if(program_linked != GL_TRUE) {
      constexpr auto MESSAGE_SIZE = 1024;
      GLsizei log_length = 0;
      std::array<GLchar, MESSAGE_SIZE> message = {};
      glGetProgramInfoLog(program, MESSAGE_SIZE, &log_length, message.data());
      throw std::runtime_error(message.data());
    }
    return program;
  }

  static GLuint createTexture() {
    GLuint renderedTexture = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &renderedTexture);

    glTextureParameteri(renderedTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTextureParameteri(renderedTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTextureParameteri(renderedTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(renderedTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureStorage2D(renderedTexture, 1, GL_RGB8, TextureWidth, TextureHeight);
    glTextureSubImage2D(renderedTexture, 0, 0, 0, TextureWidth, TextureHeight, GL_RGB, GL_UNSIGNED_BYTE, g_cTexture.data());

    return renderedTexture;
  }

  void createProgram() {
    const auto vsProgram = CreateProgramFromShader("shaders/framebuffer_vs.glsl", GL_VERTEX_SHADER);
    const auto fsProgram = CreateProgramFromShader("shaders/framebuffer_fs.glsl", GL_FRAGMENT_SHADER);

    glCreateProgramPipelines(1, &mProgram);
    glUseProgramStages(mProgram, GL_VERTEX_SHADER_BIT, vsProgram);
    glUseProgramStages(mProgram, GL_FRAGMENT_SHADER_BIT, fsProgram);
    glDeleteProgram(vsProgram);
    glDeleteProgram(fsProgram);
  }

  void createBuffers() {
    glCreateVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    mRenderedTexture = createTexture();
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

      // Draw into FrameBuffer
      glBindTextureUnit(0, mRenderedTexture);
      glBindProgramPipeline(mProgram);
      glDrawArrays(GL_TRIANGLES, 0, VERTICES_TO_DRAW);
      SDL_GL_SwapWindow(m_pWindow);
    }
  }

  void cleanup() {
    std::vector<uint8_t> buffer(TextureWidth * TextureChannels * TextureHeight);
    glGetTextureImage(mRenderedTexture, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.size(), buffer.data());
    stbi_write_png("buffer.png", TextureWidth, TextureHeight, 3, buffer.data(), TextureWidth * TextureChannels);

    glBindProgramPipeline(0);
    glDeleteTextures(1, &mRenderedTexture);
    glDeleteProgramPipelines(1, &mProgram);
    glDeleteVertexArrays(1, &mVAO);
    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(m_pWindow);
    SDL_Quit();
  }

  SDL_Window *m_pWindow = nullptr;
  SDL_GLContext mContext = nullptr;
  GLuint mVAO = 0;
  GLuint mProgram = 0;
  GLuint mRenderedTexture = 0;
};

int main(int argc, char *argv[]) {
  (void)argc; (void)argv;
  try {
    Engine engine;
    engine.initialize();
    engine.createBuffers();
    engine.createProgram();
    engine.draw();
    engine.cleanup();
  } catch(const std::runtime_error &error) {
    std::cerr << error.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

