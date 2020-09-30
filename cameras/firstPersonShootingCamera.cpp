// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// STL
#include <array>
#include <chrono>
#include <string>
#include <cstdlib>
#include <numeric>
#include <optional>
#include <iostream>
#include <algorithm>
#include <string_view>
// GL3W
#include <GL/gl3w.h>
// GLM
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/std_based_type.hpp>
// IMGUI
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
// Internal
#include "firstPersonShootingCamera.hpp"

[[maybe_unused]] static constexpr auto GL3D_SUCCESS = 0;
[[maybe_unused]] static constexpr auto SDL_SUCCESS = 0;

[[maybe_unused]] static constexpr auto SDL_IMMEDIATE_UPDATE = 0;
[[maybe_unused]] static constexpr auto SDL_SYNCHRONIZED_UPDATE = 1;
[[maybe_unused]] static constexpr auto SDL_ADAPTIVE_UPDATE = -1;

static constexpr auto FrameTime = static_cast<uint32_t>(1000.F / 60.F);

auto FPSCamera::filterMouseMoves(glm::vec2 delta) -> glm::vec2 {
  std::copy(std::cbegin(mMouseHistory), std::cend(mMouseHistory)-1, std::begin(mMouseHistory)+1);
  mMouseHistory.front() = delta;

  float averageTotal = 0.0F;
  glm::vec2 averageMotion = {0, 0};
  glm::vec2 weight = {1, 1};
  for(const auto& mouse : mMouseHistory) {
    averageMotion += mouse * weight;
    averageTotal  += 1.0F * weight.x;
    weight        *= MOUSE_FILTER_WEIGHT;
  }
  return averageMotion / averageTotal;
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

static void LogOutputFunction(void *userdata, int category, SDL_LogPriority priority, const char *message) {
  (void)userdata;
  (void)category;
  (void)priority;
  std::cout << "ERROR: " << message << std::endl;
}

static auto parseProgramOptions(int argc, char **argv) -> std::optional<std::pair<int, int>> {
  if(argc != 3) {
    return std::nullopt;
  }

  return std::make_pair(std::stoi(argv[1]), std::stoi(argv[2]));
}

auto main(int argc, char *argv[]) -> int {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Can not initialize SDL2\n";
    return EXIT_FAILURE;
  }

  SDL_LogSetOutputFunction(LogOutputFunction, nullptr);

  int width  = 1280;
  int height = 720;
  if(const auto args = parseProgramOptions(argc, argv); args) {
    constexpr auto firstScreenIndex = 0;
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(firstScreenIndex, &displayMode);

    width = std::min(args.value().first, displayMode.w);
    height = std::min(args.value().second, displayMode.h);
  }

  constexpr std::string_view sWindowTitle = "HelloWorld!";
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
  SDL_GL_SetSwapInterval(SDL_IMMEDIATE_UPDATE);

  if(gl3wInit() != GL3D_SUCCESS) {
    std::cerr << "Can not initialize GL3W!\n";
    return EXIT_FAILURE;
  }

  // Set OpenGL Debug Callback
  if(glDebugMessageCallback) {
    std::cout << "Debug is enabled\n";
    glDebugMessageCallback(DebugCallback, nullptr);
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  ImGui_ImplSDL2_InitForOpenGL(pWindow, context);
  ImGui_ImplOpenGL3_Init("#version 450 core");

  GLuint vao = 0;
  std::array<GLuint, 3> vbo;
  {
    std::array<float, 42 * 3> verties{
      // Quad
      -10.F,
      0,
      10.F,  // 0
      10.F,
      0,
      10.F,  // 1
      10.F,
      0,
      -10.F,  // 2
      -10.F,
      0,
      10.F,  // 0
      10.F,
      0,
      -10.F,  // 2
      -10.F,
      0,
      -10.F,  // 3
      // Cube
      // Front
      -1.F,
      0,
      1.F,  // 4
      1.F,
      0,
      1.F,  // 5
      1.F,
      2.F,
      1.F,  // 6
      -1.F,
      0,
      1.F,  // 4
      1.F,
      2.F,
      1.F,  // 6
      -1.F,
      2.F,
      1.F,  // 7
            // Right
      1.F,
      0,
      1.F,  // 5
      1.F,
      0,
      -1.F,  // 8
      1.F,
      2,
      -1.F,  // 9
      1.F,
      0,
      1.F,  // 5
      1.F,
      2,
      -1.F,  // 9
      1.F,
      2.F,
      1.F,  // 6
            // Top
      -1.F,
      2.F,
      1.F,  // 7
      1.F,
      2.F,
      1.F,  // 6
      1.F,
      2,
      -1.F,  // 9
      -1.F,
      2.F,
      1.F,  // 7
      1.F,
      2,
      -1.F,  // 9
      -1.F,
      2,
      -1.F,  // 11
             // Back
      1.F,
      0,
      -1.F,  // 8
      -1.F,
      0,
      -1.F,  // 10
      -1.F,
      2,
      -1.F,  // 11
      1.F,
      0,
      -1.F,  // 8
      -1.F,
      2,
      -1.F,  // 11
      1.F,
      2,
      -1.F,  // 9
             // Left
      -1.F,
      0,
      -1.F,  // 10
      -1.F,
      0,
      1.F,  // 4
      -1.F,
      2.F,
      1.F,  // 7
      -1.F,
      0,
      -1.F,  // 10
      -1.F,
      2.F,
      1.F,  // 7
      -1.F,
      2,
      -1.F,  // 11
    };
    std::array<float, 42 * 2> uvcoords{
      // Quad
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
      // Cube
      // Front side (0)
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
      // Front side (0)
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
      // Front side (0)
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
      // Front side (0)
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
      // Front side (0)
      0,
      0,  // 0
      1,
      0,  // 1
      1,
      1,  // 2
      0,
      0,  // 0
      1,
      1,  // 2
      0,
      1,  // 3
    };
    std::array<float, 42 * 3> normals{
      // Quad
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      // Cube
      // Front
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      // Right
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      // Top
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      0,
      1,
      0,
      // Back
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      // Left
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
      -1,
      0,
      0,
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(vbo.size(), vbo.data());
    glBindVertexArray(vao);
    {
      glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verties.size(), verties.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
      glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvcoords.size(), uvcoords.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);
      glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_STATIC_DRAW);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, nullptr);
    }
  }

  GLuint program = 0;
  {
    const char *vSource = R"(
      #version 450 core

      layout(location=0) in vec3 position;
      layout(location=1) in vec2 uvcoord;
      layout(location=2) in vec3 normal;

      out vec3 vNormal;
      out vec2 vUVCoord;

      uniform mat4 MVP;

      void main() {
        gl_Position = MVP * vec4(position, 1);
        vNormal = normal;
        vUVCoord = uvcoord;
      }
      )";
    auto vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vSource, nullptr);
    glCompileShader(vShader);
    GLint vertex_compiled;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertex_compiled);
    if(vertex_compiled != GL_TRUE) {
      GLsizei log_length = 0;
      GLchar message[1024];
      glGetShaderInfoLog(vShader, 1024, &log_length, message);
      std::cerr << "VertexShader: " << message << '\n';
      return EXIT_FAILURE;
    }

    const char *fSource = R"(
      #version 450 core

      in vec3 vNormal;
      in vec2 vUVCoord;

      out layout(location = 0) vec4 oFrag;

      void main() {
        oFrag = vec4(vUVCoord, 0, 1.0);
      }
      )";
    auto fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fSource, nullptr);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &vertex_compiled);
    if(vertex_compiled != GL_TRUE) {
      GLsizei log_length = 0;
      GLchar message[1024];
      glGetShaderInfoLog(fShader, 1024, &log_length, message);
      std::cerr << "FragmentShader: " << message << '\n';
      return EXIT_FAILURE;
    }

    program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);
    GLint program_linked;
    glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
    if(program_linked != GL_TRUE) {
      GLsizei log_length = 0;
      GLchar message[1024];
      glGetProgramInfoLog(program, 1024, &log_length, message);
      std::cerr << "Program: " << message << '\n';
      return EXIT_FAILURE;
    }
  }

  glEnable(GL_DEPTH_TEST);

  FPSCamera camera;

  enum MOUSE_BUTTONS {
    MOUSE_LEFT = 0,
    MOUSE_MIDDLE,
    MOUSE_RIGHT,
    MOUSE_SIZE
  };

  glm::vec3 cameraPosition{10, 10, 10}, cameraTarget{}, cameraUp{0, 1, 0};
  std::array<int, MOUSE_SIZE> mouseButton = {};
  std::array<glm::vec2, MOUSE_SIZE> mousePositions = {};
  glm::vec2 leftDelta = {};

  bool smoothMouseFilter = false;

  GLuint query[2];
  glGenQueries(2, query);
  float delta = 0.F;
  float deltaCPUs[512] = {0};
  float deltaGPUs[512] = {0};
  auto bRunning = true;
  while(bRunning) {
    SDL_Event event;
    while(SDL_PollEvent(&event) > 0) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch(event.type) {
      case SDL_QUIT: bRunning = false; break;
      case SDL_WINDOWEVENT: break;
      case SDL_MOUSEWHEEL: break;
      case SDL_MOUSEBUTTONDOWN: {
        const auto mouseButtonType = event.button;
        if(mouseButtonType.button == SDL_BUTTON_LEFT) {
          mouseButton[MOUSE_LEFT] = 1;
          mousePositions[MOUSE_LEFT] = {mouseButtonType.x, mouseButtonType.y};
        }
        if(mouseButtonType.button == SDL_BUTTON_MIDDLE) {
          mouseButton[MOUSE_MIDDLE] = 1;
          mousePositions[MOUSE_MIDDLE] = {mouseButtonType.x, mouseButtonType.y};
        }
        if(mouseButtonType.button == SDL_BUTTON_RIGHT) {
          mouseButton[MOUSE_RIGHT] = 1;
          mousePositions[MOUSE_RIGHT] = {mouseButtonType.x, mouseButtonType.y};
        }
        break;
      }
      case SDL_MOUSEBUTTONUP: {
        const auto mouseButtonType = event.button;
        if(mouseButtonType.button == SDL_BUTTON_LEFT) {
          mouseButton[MOUSE_LEFT] = 0;
        }
        if(mouseButtonType.button == SDL_BUTTON_MIDDLE) {
          mouseButton[MOUSE_MIDDLE] = 0;
        }
        if(mouseButtonType.button == SDL_BUTTON_RIGHT) {
          mouseButton[MOUSE_RIGHT] = 0;
        }
        break;
      }
      case SDL_MOUSEMOTION: {
        const auto mouseMotion = event.motion;
        if(mouseButton[MOUSE_LEFT]) {
          leftDelta =
            glm::vec2(mouseMotion.x, mouseMotion.y) - mousePositions[MOUSE_LEFT];
          if(smoothMouseFilter) {
            leftDelta = camera.filterMouseMoves(leftDelta);
          }
          leftDelta /= width;
        }
        if(mouseButton[MOUSE_MIDDLE]) {
        }
        if(mouseButton[MOUSE_RIGHT]) {
        }
        break;
      }
      }
    }

    glQueryCounter(query[0], GL_TIMESTAMP);
    const auto startTime = std::chrono::high_resolution_clock::now();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(pWindow);
    ImGui::NewFrame();

    ImGui::Begin("Camera");
    {
      ImGui::Checkbox("Enable smooth mouse filter", &smoothMouseFilter);

      ImGui::NewLine();

      ImGui::DragFloat3("Position", reinterpret_cast<float*>(&cameraPosition));
      ImGui::DragFloat3("Target",   reinterpret_cast<float*>(&cameraTarget));
      ImGui::DragFloat3("Up",       reinterpret_cast<float*>(&cameraUp));

      ImGui::NewLine();

      ImGui::DragFloat2("Left",   reinterpret_cast<float*>(&mousePositions[MOUSE_LEFT]));
      ImGui::DragFloat2("Middle", reinterpret_cast<float*>(&mousePositions[MOUSE_MIDDLE]));
      ImGui::DragFloat2("Right",  reinterpret_cast<float*>(&mousePositions[MOUSE_RIGHT]));

      ImGui::NewLine();

      ImGui::DragFloat2("Delta", reinterpret_cast<float*>(&leftDelta));

      ImGui::PlotLines("CPU", deltaCPUs, IM_ARRAYSIZE(deltaCPUs));
      ImGui::SameLine();
      double cpu = std::reduce(std::cbegin(deltaCPUs), std::cend(deltaCPUs)) / sizeof(deltaCPUs)/sizeof(float);
      ImGui::Text("%.6f", cpu);
      ImGui::PlotLines("GPU", deltaGPUs, IM_ARRAYSIZE(deltaGPUs));
      ImGui::SameLine();
      double gpu = std::reduce(std::cbegin(deltaGPUs), std::cend(deltaGPUs)) / sizeof(deltaGPUs)/sizeof(float);
      ImGui::Text("%.6f", gpu);
    }
    ImGui::End();

    const auto pKeyStatus = SDL_GetKeyboardState(nullptr);
    camera.updateRotation(mouseButtonsPositionAngles);
    camera.update(delta, pKeyStatus);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glUseProgram(program);
      {
        static GLint viewAttribPosition = glGetUniformLocation(program, "MVP");
        const auto projection = glm::perspective(glm::radians(60.F), static_cast<float>(width) / static_cast<float>(height), 0.1F, 1000.F);
        const auto radians = glm::radians(leftDelta);
        const auto rotation = glm::yawPitchRoll(radians.x, radians.y, 0.F);
        const auto MVP = projection * rotation * camera.view();
        glUniformMatrix4fv(viewAttribPosition, 1, GL_FALSE, glm::value_ptr(MVP));

        glDrawArrays(GL_TRIANGLES, 0, 42);
      }
      glUseProgram(0);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(pWindow);

    glQueryCounter(query[1], GL_TIMESTAMP);
    const auto stopTime = std::chrono::high_resolution_clock::now();

    delta = std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();

    // wait until the results are available
    int stopTimerAvailable = 0;
    while(!stopTimerAvailable) {
      glGetQueryObjectiv(query[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
    }

    GLuint64 startTimeGL, stopTimeGL;
    // get query results
    glGetQueryObjectui64v(query[0], GL_QUERY_RESULT, &startTimeGL);
    glGetQueryObjectui64v(query[1], GL_QUERY_RESULT, &stopTimeGL);

    const auto deltaGPU = static_cast<double>(stopTimeGL - startTimeGL) / 1000000.0;
    if(delta < FrameTime) {
      SDL_Delay(FrameTime - static_cast<uint32_t>(delta));
    }
    std::copy(std::begin(deltaCPUs), std::end(deltaCPUs)-1, std::begin(deltaCPUs)+1);
    deltaCPUs[0] = delta;

    std::copy(std::begin(deltaGPUs), std::end(deltaGPUs)-1, std::begin(deltaGPUs)+1);
    deltaGPUs[0] = static_cast<float>(deltaGPU);
    //snprintf(message, 512, "Frame %+3.3F/%+3.3F ms", static_cast<double>(delta), static_cast<double>(stopTimeGL - startTimeGL) / 1000000.0);
  }

  glBindVertexArray(0);
  glDeleteVertexArrays(1, &vao);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();

  return EXIT_SUCCESS;
}
