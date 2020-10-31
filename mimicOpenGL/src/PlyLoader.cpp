// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// STL
#include <fstream>
// Internal
#include "PlyLoader.hpp"

auto loadPlyFile(std::string_view fileName) -> std::optional<Model> {
  std::ifstream inputStream(fileName.data(), std::ios::ate);
  if(!inputStream.is_open()) {
    return std::nullopt;
  }

  std::string line;
  while(std::getline(inputStream, line)) {
  }
}

