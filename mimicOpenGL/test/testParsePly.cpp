// STL
#include <string>
#include <cstdlib>
#include <sstream>
#include <iostream>
// tinyobj
#define TINYOBJLOADER_IMPLEMENTATION
#include "../tiny_obj_loader.h"
#undef TINYOBJLOADER_IMPLEMENTATION

auto main() -> int {
  const std::string sPlyFile = R"(
  ply
  format ascii 10
  element vertex 8
  property float x
  property float y
  property float z
  element face 6
  roperty list uchar int vertex_index
  end_header
  0 0 0
  0 0 1
  0 1 1
  0 1 0
  1 0 0
  1 0 1
  1 1 1
  1 1 0
  4 0 1 2 3
  4 7 6 5 4
  4 0 4 5 1
  4 1 5 6 2
  4 2 6 7 3
  4 3 7 4 0
  )";

  tinyobj::ObjReader loader;
  if(!loader.ParseFromString(sPlyFile, "")) {
    std::cerr << "Can not parse ply file\n";
    std::cerr << loader.Error() << ", "
              << loader.Warning() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
