// STL
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <string_view>
// fmt
#include <fmt/color.h>
#include <fmt/format.h>
// glm
#include <glm/matrix.hpp>
// GLFW
#include <GL/gl3w.h>
// GLFW3
#include <GLFW/glfw3.h>
// RapidJSON
#include <rapidjson/document.h>

constexpr auto cWidth = 640;
constexpr auto cHeight = 480;
const char *sTitle = "Hello World";

constexpr uint32_t toUint32(std::string_view buffer) {
  if(buffer.size() == 3) {
    return static_cast<uint32_t>(0) << 24 |
           static_cast<uint32_t>(buffer[2]) << 16 |
           static_cast<uint32_t>(buffer[1]) << 8  |
           static_cast<uint32_t>(buffer[0]);
  }
  return static_cast<uint32_t>(buffer[3]) << 24 |
         static_cast<uint32_t>(buffer[2]) << 16 |
         static_cast<uint32_t>(buffer[1]) << 8  |
         static_cast<uint32_t>(buffer[0]);
}

static void loadGLTF(std::string_view filePath) {
  std::ifstream inputStream(filePath.data(), std::ios::binary | std::ios::ate);
  if(!inputStream.is_open()) {
    fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "ERROR: Can not open {}\n", filePath.data());
    std::exit(EXIT_FAILURE);
  }
  const auto fileSize = inputStream.tellg();
  inputStream.seekg(0, std::ios::beg);

  // Read file header
  struct Header {
    uint32_t magic = 0;
    uint32_t version = 0;
    uint32_t length = 0;
  };
  Header header;
  inputStream.read(reinterpret_cast<char *>(&header), sizeof(Header));

  if(header.magic != toUint32("glTF")) {
    fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "ERROR: Magic number is not equal match\n");
    std::exit(EXIT_FAILURE);
  }

  if(header.length != fileSize) {
    fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "ERROR: File size not match header\n");
    std::exit(EXIT_FAILURE);
  }

  if(header.version != 2) {
    fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "ERROR: glTF version 2 only supported\n");
    std::exit(EXIT_FAILURE);
  }

  struct ChunkHeader {
    uint32_t chunkLength = 0;
    uint32_t chunkType = 0;
  };
  ChunkHeader chunckHeader;
  while(inputStream.good()) {
    inputStream.read(reinterpret_cast<char*>(&chunckHeader), sizeof(ChunkHeader));

    if(chunckHeader.chunkType == toUint32("JSON")) {
      std::vector<char> jsonBuffer(chunckHeader.chunkLength);
      inputStream.read(jsonBuffer.data(), jsonBuffer.size());

      rapidjson::Document document;
      document.Parse(jsonBuffer.data());
    } else if(chunckHeader.chunkType == toUint32("BIN")) {
      std::vector<char> binaryBuffer(chunckHeader.chunkLength);
      inputStream.read(binaryBuffer.data(), binaryBuffer.size());
    } else {
      break;
    }
  }

}

int main() {
  if(!glfwInit()) {
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  auto pWindow = glfwCreateWindow(cWidth, cHeight, sTitle, nullptr, nullptr);
  if(pWindow == nullptr) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(pWindow);
  if(gl3wInit() != GL3W_OK) {
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwSwapInterval(1);

  fmt::print("OpenGL {}, GLSL {}\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

  loadGLTF("Box.glb");

  while(!glfwWindowShouldClose(pWindow)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(pWindow);
    glfwPollEvents();
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}
