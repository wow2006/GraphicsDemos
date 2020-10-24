#pragma once
// STL
#include <array>
#include <limits>
#include <cstdlib>
#include <cstdint>
#include <iomanip>
#include <utility>
#include <iostream>
// fmt
#include <fmt/format.h>
// glm
#include <glm/glm.hpp>

namespace glm {
bool equal(glm::vec2 a, glm::vec2 b) {
  constexpr glm::vec2 EPSILON(std::numeric_limits<float>::epsilon());
  return glm::all(glm::lessThan(glm::abs(a - b), EPSILON));
}

bool equal(glm::vec4 a, glm::vec4 b) {
  constexpr glm::vec4 EPSILON(std::numeric_limits<float>::epsilon());
  return glm::all(glm::lessThan(glm::abs(a - b), EPSILON));
}
template<uint32_t count>
glm::vec<count, float> invalid() {
  return {std::numeric_limits<float>::max()};
}

template<>
glm::vec<2, float> invalid<2>() {
  return {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
}
}  // namespace glm

struct QuadTree final {
  /*
   *                   ^
   *   +---------------|----------------+
   *   |               |                |
   *   |               |                |
   *   | 3. TopLeft    | 0- TopRight    |
   *   |               |                |
   *   |               |                |
   *   +---------------------------------->
   *   |               |                |
   *   |               |                |
   *   | 2- BottomLeft | 1- BottomRight |
   *   |               |                |
   *   |               |                |
   *   +--------------------------------+
   */
  struct Node final {
    static constexpr std::size_t CHILDREN = 4;
    enum RemoveState { RemoveMe, Check, NotFound };
    enum Index { TopRight = 0, BottomRight, BottomLeft, TopLeft };
    bool m_bInitialied = false;
    int m_iChildrens = -1;
    glm::vec2 value;
    glm::vec4 rect;
    std::array<std::unique_ptr<Node>, CHILDREN> m_aChildrens;

    Node(glm::vec2 newValue, glm::vec4 newRect) : m_bInitialied{true}, value(newValue), rect(newRect) {}

    glm::vec2 mid() const { return {rect.x + (rect.z / 2), rect.y + (rect.w / 2)}; }

    std::pair<Index, glm::vec4> index(glm::vec2 newValue) const {
      const auto mindex = mid();
      if(newValue.x < mindex.x) {
        if(newValue.y < mindex.y) {
          return {Index::BottomLeft, glm::vec4(rect.x, rect.y, rect.z / 2, rect.w / 2)};
        } else {
          return {Index::TopLeft, glm::vec4(rect.x, rect.y + rect.w / 2, rect.z / 2, rect.w / 2)};
        }
      } else {
        if(newValue.y < mindex.y) {
          return {Index::BottomRight, glm::vec4(rect.x + rect.z / 2, rect.y, rect.z / 2, rect.w / 2)};
        } else {
          return {Index::TopRight, glm::vec4(rect.x + rect.z / 2, rect.y + rect.w / 2, rect.z / 2, rect.w / 2)};
        }
      }
    }

    void add(glm::vec2 newValue) {
      if(m_iChildrens == -1) {
        m_iChildrens = 0;
        const auto [currentValueIndex, currentRect] = index(value);
        if(m_aChildrens[currentValueIndex] == nullptr) {
          m_aChildrens[currentValueIndex] = std::make_unique<Node>(value, currentRect);
        }
        value = glm::invalid<2>();
      }

      const auto [newValueIndex, newRect] = index(newValue);
      if(!m_aChildrens[newValueIndex]) {
        m_aChildrens[newValueIndex] = std::make_unique<Node>(newValue, newRect);
        return;
      }
      m_aChildrens[newValueIndex]->add(newValue);
    }

    RemoveState remove(glm::vec2 pointToRemove) {
      if(m_iChildrens > -1) {
        const auto [idx, _] = index(pointToRemove);
        auto status = RemoveState::NotFound;
        if(m_aChildrens[idx] != nullptr) {
          status = m_aChildrens[idx]->remove(pointToRemove);
        }
        switch(status) {
        case RemoveState::RemoveMe: m_aChildrens[idx].reset();
        case RemoveState::Check: {
          if(m_iChildrens == 0) {
            auto pNode = std::begin(m_aChildrens);
            for(; pNode != std::end(m_aChildrens); ++pNode) {
              if(*pNode != nullptr) {
                break;
              }
            }
            value = (*pNode)->value;
            (*pNode).reset();
            --m_iChildrens;
          }
          return RemoveState::Check;
        }
        }
        return RemoveState::NotFound;
      }

      if(glm::equal(value, pointToRemove)) {
        return RemoveState::RemoveMe;
      }
      return RemoveState::NotFound;
    }

  };

  QuadTree(glm::vec4 fullsceen) : rect{fullsceen} {}

  void add(glm::vec2 value) {
    if(m_pRoot == nullptr) {
      m_pRoot = std::make_unique<Node>(value, rect);
      return;
    }
    m_pRoot->add(value);
  }

  void remove(glm::vec2 value) {
    if(m_pRoot) {
      if(m_pRoot->remove(value) == Node::RemoveState::RemoveMe) {
        m_pRoot.reset();
      }
    }
  }

  glm::vec4 rect;
  std::unique_ptr<Node> m_pRoot;
};

inline std::ostream &operator<<(std::ostream &out, const glm::vec2 &vec) {
  return out << fmt::format("({: .2f},{: .2f})", vec.x, vec.y);
}

inline std::ostream &operator<<(std::ostream &out, const QuadTree::Node &node) {
  if(!node.m_bInitialied) {
    return out << "()";
  }

  if(node.m_iChildrens > -1) {
    out << "()\n";
    for(uint32_t index = 0; index < QuadTree::Node::CHILDREN - 1; ++index) {
      auto &pChild = node.m_aChildrens.at(index);
      if(pChild != nullptr) {
        out << fmt::format("Ͱ ") << *pChild << '\n';
      } else {
        out << fmt::format("Ͱ ()\n");
      }
    }

    auto &pChild = node.m_aChildrens[QuadTree::Node::CHILDREN - 1];
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
