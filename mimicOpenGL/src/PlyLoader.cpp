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

