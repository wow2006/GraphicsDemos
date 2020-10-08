#pragma once
// STL
#include <array>
#include <limits>
#include <cstdlib>
#include <cstdint>
#include <iomanip>
#include <iostream>
// fmt
#include <fmt/format.h>
// glm
#include <glm/glm.hpp>


struct QuadTree final {

struct Node final {
  static constexpr std::size_t CHILDREN = 4;
  bool m_bInitialied = false;
  bool m_bChildrens  = false;
  glm::vec2 value;
  glm::vec4 rect;
  std::array<Node*, CHILDREN> m_aChildrens = {};

  void add(glm::vec2 newNode) {
    if(!m_bInitialied) {
      value = newNode;
      m_bInitialied = true;
      return;
    }

    if(!m_bChildrens) {
      m_bChildrens = true;
    }
    //const auto r = glm::vec2(rect.x / 2.F, rect.y / 2.F);
  }
};

};

inline std::ostream& operator<<(std::ostream& out, const glm::vec2& vec) {
  return out << fmt::format("({: .2f},{: .2f})", vec.x, vec.y);
}

std::ostream& operator<<(std::ostream& out, const QuadTree::Node& node) {
  if(!node.m_bInitialied) {
    return out << "()";
  }

  if(node.m_bChildrens) {
    out << "()\n";
    for(uint32_t index = 0; index < QuadTree::Node::CHILDREN-1; ++index) {
      auto *pChild = node.m_aChildrens[index];
      if(pChild != nullptr) {
        out << fmt::format("Ͱ ") << *pChild << '\n';
      } else {
        out << fmt::format("Ͱ ()\n");
      }
    }

    auto *pChild = node.m_aChildrens[QuadTree::Node::CHILDREN-1];
    if(pChild != nullptr) {
      out << fmt::format("L ") << *pChild << '\n';
    } else {
      out << fmt::format("L ()\n");
    }
  } else {
    out << node.value;
  }
  return out;
}

