#pragma once
// STL
#include <string_view>
// Internal
#include "Type.hpp"

struct Image {
  Image(std::uint32_t width, std::uint32_t height);

  std::uint32_t width;
  std::uint32_t height;
  vec3_8u *m_pData = nullptr;

  auto operator()(std::uint32_t x, std::uint32_t y) -> vec3_8u& { return m_pData[y * width + x]; }

  auto operator()(std::uint32_t x, std::uint32_t y) const -> const vec3_8u& { return m_pData[y * width + x]; }

  auto saveImage(std::string_view outputFileName) -> bool;
};

