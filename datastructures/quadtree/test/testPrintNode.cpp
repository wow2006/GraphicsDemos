// Catch2
#include <catch2/catch.hpp>
// Internal
#include "../quadtree.hpp"


TEST_CASE("Print empty node", "[QuadTree,Node]") {
  QuadTree::Node node;
  std::stringstream out;
  out << node;

  REQUIRE("()" == out.str());
}

TEST_CASE("Print (0, 0) node", "[QuadTree,Node]") {
  QuadTree::Node node;
  node.m_bInitialied = true;
  node.value         = glm::vec2(0, 0);
  std::stringstream out;
  out << node;

  REQUIRE("( 0.00, 0.00)" == out.str());
}

TEST_CASE("Print (0.5, 0.5),(),(),()", "[QuadTree,Node]") {
  QuadTree::Node node;
  node.m_bInitialied = true;
  node.m_bChildrens  = true;
  node.m_aChildrens[0] = new QuadTree::Node();
  node.m_aChildrens[0]->m_bInitialied = true;
  node.m_aChildrens[0]->value         = glm::vec2(0.5F, 0.5F);

  std::stringstream out;
  out << node;

  REQUIRE("()\nͰ ( 0.50, 0.50)\nͰ ()\nͰ ()\nL ()\n" == out.str());
}

TEST_CASE("Print (0.5, 0.5),(0.5,-0.5),(),()", "[QuadTree,Node]") {
  QuadTree::Node node;
  node.m_bInitialied = true;
  node.m_bChildrens  = true;

  node.m_aChildrens[0] = new QuadTree::Node();
  node.m_aChildrens[0]->m_bInitialied = true;
  node.m_aChildrens[0]->value         = glm::vec2(0.5F, 0.5F);

  node.m_aChildrens[1] = new QuadTree::Node();
  node.m_aChildrens[1]->m_bInitialied = true;
  node.m_aChildrens[1]->value         = glm::vec2(0.5F,-0.5F);

  std::stringstream out;
  out << node;

  REQUIRE("()\nͰ ( 0.50, 0.50)\nͰ ( 0.50,-0.50)\nͰ ()\nL ()\n" == out.str());
}

TEST_CASE("Print (0.5, 0.5),(0.5,-0.5),(-0.5,-0.5),()", "[QuadTree,Node]") {
  QuadTree::Node node;
  node.m_bInitialied = true;
  node.m_bChildrens  = true;

  node.m_aChildrens[0] = new QuadTree::Node();
  node.m_aChildrens[0]->m_bInitialied = true;
  node.m_aChildrens[0]->value         = glm::vec2(0.5F, 0.5F);

  node.m_aChildrens[1] = new QuadTree::Node();
  node.m_aChildrens[1]->m_bInitialied = true;
  node.m_aChildrens[1]->value         = glm::vec2(0.5F,-0.5F);

  node.m_aChildrens[2] = new QuadTree::Node();
  node.m_aChildrens[2]->m_bInitialied = true;
  node.m_aChildrens[2]->value         = glm::vec2(-0.5F,-0.5F);

  std::stringstream out;
  out << node;

  REQUIRE("()\nͰ ( 0.50, 0.50)\nͰ ( 0.50,-0.50)\nͰ (-0.50,-0.50)\nL ()\n" == out.str());
}

TEST_CASE("Print (0.5, 0.5),(0.5,-0.5),(-0.5,-0.5),(-0.5, 0.5)", "[QuadTree,Node]") {
  QuadTree::Node node;
  node.m_bInitialied = true;
  node.m_bChildrens  = true;

  node.m_aChildrens[0] = new QuadTree::Node();
  node.m_aChildrens[0]->m_bInitialied = true;
  node.m_aChildrens[0]->value         = glm::vec2(0.5F, 0.5F);

  node.m_aChildrens[1] = new QuadTree::Node();
  node.m_aChildrens[1]->m_bInitialied = true;
  node.m_aChildrens[1]->value         = glm::vec2(0.5F,-0.5F);

  node.m_aChildrens[2] = new QuadTree::Node();
  node.m_aChildrens[2]->m_bInitialied = true;
  node.m_aChildrens[2]->value         = glm::vec2(-0.5F,-0.5F);

  node.m_aChildrens[3] = new QuadTree::Node();
  node.m_aChildrens[3]->m_bInitialied = true;
  node.m_aChildrens[3]->value         = glm::vec2(-0.5F, 0.5F);

  std::stringstream out;
  out << node;

  REQUIRE("()\nͰ ( 0.50, 0.50)\nͰ ( 0.50,-0.50)\nͰ (-0.50,-0.50)\nL (-0.50, 0.50)\n" == out.str());
}

