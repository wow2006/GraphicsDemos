#pragma once
// STL
#include <optional>
#include <string_view>

struct Model {};

auto loadPlyFile(std::string_view fileName) -> std::optional<Model>;

