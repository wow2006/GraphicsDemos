// Internal
#include "pointRegionQuadtree.hpp"


auto main() -> int {
  constexpr std::array<glm::vec2, 4> vertices {
    glm::vec2( 0.5, 0.5),
    glm::vec2( 0.5,-0.5),
    glm::vec2(-0.5,-0.5),
    glm::vec2(-0.5, 0.5)
  };

  QuadTree tree({-1, -1, 2, 2});
  tree.add(vertices[0]);
  tree.add(vertices[1]);
  tree.add(vertices[2]);
  tree.add(vertices[3]);
  /*
  QuadTree::Node root;
  root.m_bChildrens = true;
  for(uint32_t index = 0; index < QuadTree::Node::CHILDREN; ++index) {
    root.add(vertices.at(index));
    //auto *pChild                = new QuadTree::Node();
    //pChild->value               = vertices.at(index);
    //root.m_aChildrens.at(index) = pChild;
  }
  std::cout << root << '\n';
  */

  return EXIT_SUCCESS;
}

