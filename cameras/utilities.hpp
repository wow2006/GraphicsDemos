#pragma once
// STL
#include <chrono>
#include <cstdint>
#include <algorithm>
// OpenGL
#include <GL/gl3w.h>

[[maybe_unused]] static constexpr auto GL3D_SUCCESS = 0;
[[maybe_unused]] static constexpr auto SDL_SUCCESS = 0;

[[maybe_unused]] static constexpr auto SDL_IMMEDIATE_UPDATE = 0;
[[maybe_unused]] static constexpr auto SDL_SYNCHRONIZED_UPDATE = 1;
[[maybe_unused]] static constexpr auto SDL_ADAPTIVE_UPDATE = -1;

static constexpr auto FrameTime = static_cast<uint32_t>(1000.F / 60.F);

enum MOUSE_BUTTONS { MOUSE_LEFT = 0, MOUSE_MIDDLE, MOUSE_RIGHT, MOUSE_SIZE };


enum TimerType : uint8_t {
  CPU,
  GPU
};

template<TimerType TYPE>
class Timer;

template<>
class Timer<TimerType::CPU> final {
public:
  void start() {
    startTime = std::chrono::high_resolution_clock::now();
  }

  float end() {
    const auto stopTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float, std::chrono::milliseconds::period>(stopTime - startTime).count();
  }

private:
  std::chrono::high_resolution_clock::time_point startTime;

};

template<>
class Timer<TimerType::GPU> final {
public:
  void initialize() {
    glGenQueries(2, query);
  }

  ~Timer() {
    glDeleteQueries(2, query);
  }

  void start() {
    glQueryCounter(query[0], GL_TIMESTAMP);
  }

  void waitForQueryToBeReady() {
    int stopTimerAvailable = 0;
    while(!stopTimerAvailable) {
      glGetQueryObjectiv(query[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
    }
  }

  float end() {
    glQueryCounter(query[1], GL_TIMESTAMP);
    waitForQueryToBeReady();

    GLuint64 startTimeGL, stopTimeGL;
    // get query results
    glGetQueryObjectui64v(query[0], GL_QUERY_RESULT, &startTimeGL);
    glGetQueryObjectui64v(query[1], GL_QUERY_RESULT, &stopTimeGL);

    return static_cast<float>(stopTimeGL - startTimeGL) / 1000000.0F;
  }

private:
  GLuint query[2];
};

template<typename Type, std::uint32_t BufferSize>
struct CircularBuffer {
  uint32_t currentSize = 0;

  Type mValues[BufferSize] = {0};

  std::uint32_t size() const {
    return (currentSize < BufferSize) ? currentSize : BufferSize;
  }

  void add(Type newValue) {
    if(currentSize < BufferSize) {
      std::copy_n(std::cbegin(mValues), currentSize, std::begin(mValues) + 1);
      ++currentSize;
    } else {
      std::copy(std::cbegin(mValues), std::cend(mValues) - 1, std::begin(mValues) + 1);
    }
    mValues[0] = newValue;
  }

};

template<typename Type, std::uint32_t BufferSize>
std::ostream& operator<<(std::ostream& out, const CircularBuffer<Type, BufferSize>& buffer) {
  out << buffer.size() << ": ";
  for(const auto& b : buffer.mValues) {
    out << b << ' ';
  }
  return out;
}
