// STL
#include <cmath>
// Internal
#include "Line.hpp"

namespace mimicOpenGL {

void line1(int x0, int y0, int x1, int y1, vec3_8u color, Image &output) {
  for(float i = 0.F; i < 1.F; i += 0.01F) {
    const std::uint32_t x = x0 + (x1 - x0) * i;
    const std::uint32_t y = y0 + (y1 - y0) * i;

    output(x, y) = color;
  }
}

void line2(int x0, int y0, int x1, int y1, vec3_8u color, Image &output) {
  for(auto x = x0; x < x1; ++x) {
    const auto t = (x - x0) / static_cast<float>(x1 - x0);
    const auto y = y0 + (y1 - y0) * t;

    output(x, y) = color;
  }
}

void line2a(int x0, int y0, int x1, int y1, vec3_8u color, Image &output) {
  for(auto x = x0; x < x1; ++x) {
    const auto t = (x - x0) / static_cast<float>(x1 - x0);
    const auto y = y0 * (1.F - t) + y1 * t;

    output(x, y) = color;
  }
}

void line3(int x0, int y0, int x1, int y1, vec3_8u color, Image &output) {
  bool steep = false;
  if(std::abs(x0 - x1) < std::abs(y0 - y1)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }

  if(x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  for(int x = x0; x <= x1; ++x) {
    const float t = (x - x0) / static_cast<float>(x1 - x0);
    const int y = y0 * (1.F - t) + y1 * t;
    if(steep) {
      output(y, x) = color;
    } else {
      output(x, y) = color;
    }
  }
}

void line4(int x0, int y0, int x1, int y1, vec3_8u color, Image &output) {
  bool steep = false;
  if(std::abs(x0 - x1) < std::abs(y0 - y1)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }

  if(x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  const int dx = x1 - x0;
  const int dy = y1 - y0;
  const float derror = std::abs(dy / float(dx));
  float error = 0;
  int y = y0;
  for(int x = x0; x <= x1; x++) {
    if(steep) {
      output(y, x) = color;
    } else {
      output(x, y) = color;
    }

    error += derror;
    if(error > .5) {
      y += (y1 > y0 ? 1 : -1);
      error -= 1.;
    }
  }
}

void line5(int x0, int y0, int x1, int y1, vec3_8u color, Image &output) {
  bool steep = false;
  if(std::abs(x0 - x1) < std::abs(y0 - y1)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }

  if(x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  const int dx = x1 - x0;
  const int dy = y1 - y0;
  const int derror2 = std::abs(dy) * 2;
  int error2 = 0;
  int y = y0;
  for(int x = x0; x <= x1; x++) {
    if(steep) {
      output(y, x) = color;
    } else {
      output(x, y) = color;
    }

    error2 += derror2;
    if(error2 > dx) {
      y += (y1 > y0 ? 1 : -1);
      error2 -= dx * 2;
    }
  }
}

}
