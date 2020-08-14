#pragma once
// STL
#include <cstdint>

template<typename T>
struct vec3 {
  union { T x, r, s; };
  union { T y, g, t; };
  union { T z, b, p; };
};

using vec3_f = vec3<float>;
using vec3_8u = vec3<std::uint8_t>;

