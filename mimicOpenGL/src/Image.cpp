// Internal
#include "Image.hpp"
// STB
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

Image::Image(std::uint32_t width, std::uint32_t height) : width{width}, height{height} { m_pData = new vec3_8u[width * height]; }

auto Image::saveImage(std::string_view outputFileName) -> bool {
  return stbi_write_png(outputFileName.data(), width, height, 3, m_pData, width * 3) != 0;
}
