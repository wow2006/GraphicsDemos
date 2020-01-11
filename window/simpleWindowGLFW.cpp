// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// STL
#include <cstdlib>
#include <optional>
#include <iostream>
// GLFW
#include <GLFW/glfw3.h>

static auto parseProgramOptions(int argc, char** argv)
  -> std::optional<std::pair<int, int>> {
  if(argc != 3) {
    std::cerr << "Usage:\n\t" << argv[0] << " width height\n";
    return std::nullopt;
  }

  return std::make_pair(
    std::stoi(argv[1]),
    std::stoi(argv[2])
  );
}

int main(int argc, char* argv[]) {
  if(!glfwInit()) {
    return EXIT_FAILURE;
  }

  int width  = 640;
  int height = 480;
  if(const auto args = parseProgramOptions(argc, argv);
     args) {

    int x, y, w, h;
    auto pPrimaryMonitor = glfwGetPrimaryMonitor();
    glfwGetMonitorWorkarea(pPrimaryMonitor, &x, &y, &w, &h);

    width  = std::min(args.value().first, w);
    height = std::min(args.value().second, h);
  }

  constexpr std::string_view sWindowTitle = "HelloWorld!";
  auto pWindow = glfwCreateWindow(width, height, sWindowTitle.data(), nullptr, nullptr);
  if(!pWindow) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  while(!glfwWindowShouldClose(pWindow)) {
    glfwSwapBuffers(pWindow);
    glfwPollEvents();
  }

  glfwDestroyWindow(pWindow);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
