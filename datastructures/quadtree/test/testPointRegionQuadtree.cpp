// Catch2
#include <catch2/catch.hpp>
// Internal
#include "../pointRegionQuadtree.hpp"


TEST_CASE("two floating point equal") {
  constexpr glm::vec2 a(0.5, 0.5);
  constexpr glm::vec2 b(0.5, 0.5);
  REQUIRE(glm::equal(a, b));
}

TEST_CASE("two floating point not equal") {
  constexpr glm::vec2 a(1.5, 1.5);
  constexpr glm::vec2 b(0.5, 0.5);
  REQUIRE_FALSE(glm::equal(a, b));
}

inline constexpr glm::vec4 Rect{-1, -1, 2, 2};
TEST_CASE("Create a tree", "[QuadTree]") {
  QuadTree tree(Rect);
  REQUIRE(tree.m_pRoot == nullptr);
  REQUIRE(glm::equal(tree.rect, Rect));
}

TEST_CASE("Add one point (0.5, 0.5)", "[QuadTree]") {
  constexpr glm::vec2 point(0.5, 0.5);
  QuadTree tree(Rect);
  tree.add(point);
  REQUIRE_FALSE(tree.m_pRoot == nullptr);
  REQUIRE(tree.m_pRoot->m_bInitialied);
  REQUIRE_FALSE(tree.m_pRoot->m_iChildrens > -1);
  REQUIRE(glm::equal(point, tree.m_pRoot->value));
  REQUIRE(glm::equal(Rect, tree.m_pRoot->rect));
}

TEST_CASE("Remove one point (0.5, 0.5)", "[QuadTree]") {
  constexpr glm::vec2 point(0.5, 0.5);
  QuadTree tree(Rect);
  tree.add(point);
  tree.remove(point);
  REQUIRE(tree.m_pRoot == nullptr);
}

TEST_CASE("Add points (0.5, 0.5)(0.5,-0.5)", "[QuadTree]") {
  constexpr glm::vec2 point0(0.5, 0.5);
  constexpr glm::vec2 point1(0.5,-0.5);
  QuadTree tree(Rect);
  tree.add(point0);
  tree.add(point1);
  REQUIRE_FALSE(tree.m_pRoot == nullptr);
  REQUIRE(tree.m_pRoot->m_bInitialied);
  REQUIRE(tree.m_pRoot->m_iChildrens > -1);
  REQUIRE_FALSE(glm::equal(point0, tree.m_pRoot->value));
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[0] == nullptr);
  REQUIRE(glm::equal(point0, tree.m_pRoot->m_aChildrens[0]->value));
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[1] == nullptr);
  REQUIRE(glm::equal(point1, tree.m_pRoot->m_aChildrens[1]->value));
  REQUIRE(tree.m_pRoot->m_aChildrens[2] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[3] == nullptr);
}

TEST_CASE("Add points (0.5, 0.5)(0.5,-0.5) then remove (0.5, 0.5)", "[QuadTree]") {
  constexpr std::array<glm::vec2, 2> points = {
    glm::vec2(0.5, 0.5),
    glm::vec2(0.5,-0.5)
  };

  QuadTree tree(Rect);
  for(const auto& point : points) {
    tree.add(point);
  }
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[0] == nullptr);
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[1] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[2] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[3] == nullptr);

  tree.remove(points[0]);
  REQUIRE_FALSE(tree.m_pRoot == nullptr);
  REQUIRE(tree.m_pRoot->m_iChildrens == -1);
  REQUIRE(tree.m_pRoot->m_aChildrens[0] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[1] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[2] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[3] == nullptr);
}

TEST_CASE("Add one point (0.5, 0.5)(0.5,-0.5)(-0.5,-0.5)", "[QuadTree]") {
  constexpr glm::vec2 point0( 0.5, 0.5);
  constexpr glm::vec2 point1( 0.5,-0.5);
  constexpr glm::vec2 point2(-0.5,-0.5);
  QuadTree tree(Rect);
  tree.add(point0);
  tree.add(point1);
  tree.add(point2);
  REQUIRE_FALSE(tree.m_pRoot == nullptr);
  REQUIRE(tree.m_pRoot->m_bInitialied);
  REQUIRE(tree.m_pRoot->m_iChildrens > -1);
  //REQUIRE_FALSE(glm::equal(point0, tree.m_pRoot->value));
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[0] == nullptr);
  REQUIRE(glm::equal(point0, tree.m_pRoot->m_aChildrens[0]->value));
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[1] == nullptr);
  REQUIRE(glm::equal(point1, tree.m_pRoot->m_aChildrens[1]->value));
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[2] == nullptr);
  REQUIRE(glm::equal(point2, tree.m_pRoot->m_aChildrens[2]->value));
  REQUIRE(tree.m_pRoot->m_aChildrens[3] == nullptr);
}

TEST_CASE("Add one point (0.5, 0.5)(0.5,-0.5)(-0.5,-0.5)(-0.5, 0.5)", "[QuadTree]") {
  constexpr glm::vec2 point0( 0.5, 0.5);
  constexpr glm::vec2 point1( 0.5,-0.5);
  constexpr glm::vec2 point2(-0.5,-0.5);
  constexpr glm::vec2 point3(-0.5, 0.5);
  QuadTree tree(Rect);
  tree.add(point0);
  tree.add(point1);
  tree.add(point2);
  tree.add(point3);
  REQUIRE_FALSE(tree.m_pRoot == nullptr);
  REQUIRE(tree.m_pRoot->m_bInitialied);
  REQUIRE(tree.m_pRoot->m_iChildrens > -1);
  //REQUIRE_FALSE(glm::equal(point0, tree.m_pRoot->value));
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[0] == nullptr);
  REQUIRE(glm::equal(point0, tree.m_pRoot->m_aChildrens[0]->value));
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[1] == nullptr);
  REQUIRE(glm::equal(point1, tree.m_pRoot->m_aChildrens[1]->value));
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[2] == nullptr);
  REQUIRE(glm::equal(point2, tree.m_pRoot->m_aChildrens[2]->value));
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[3] == nullptr);
  REQUIRE(glm::equal(point3, tree.m_pRoot->m_aChildrens[3]->value));
}

TEST_CASE("Add one point (0.25, 0.25),(0.75, 0.75)", "[QuadTree]") {
  constexpr std::array<glm::vec2, 2> points = {
    glm::vec2{0.25F, 0.25F},
    glm::vec2{0.75F, 0.75F}
  };

  QuadTree tree(Rect);
  tree.add(points[0]);
  tree.add(points[1]);

  REQUIRE(tree.m_pRoot->m_iChildrens > -1);
  REQUIRE(glm::equal(tree.m_pRoot->rect, Rect));
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[0] == nullptr);
  REQUIRE(glm::equal(tree.m_pRoot->m_aChildrens[0]->rect, glm::vec4(0, 0, 1, 1)));
  REQUIRE(tree.m_pRoot->m_aChildrens[1] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[2] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[3] == nullptr);

  const auto &pChild = tree.m_pRoot->m_aChildrens[0];
  REQUIRE(pChild->m_iChildrens > -1);
  REQUIRE_FALSE(pChild->m_aChildrens[0] == nullptr);
  REQUIRE(glm::equal(pChild->m_aChildrens[0]->rect, glm::vec4(0.5, 0.5, 0.5, 0.5)));
  REQUIRE(pChild->m_aChildrens[1] == nullptr);
  REQUIRE_FALSE(pChild->m_aChildrens[2] == nullptr);
  REQUIRE(glm::equal(pChild->m_aChildrens[2]->rect, glm::vec4(0, 0, 0.5, 0.5)));
  REQUIRE(pChild->m_aChildrens[3] == nullptr);
}

TEST_CASE("Add one point (0.25, 0.25),(0.75, 0.75) remove (0.75, 0.75)", "[QuadTree]") {
  constexpr std::array<glm::vec2, 2> points = {
    glm::vec2{0.25F, 0.25F},
    glm::vec2{0.75F, 0.75F}
  };

  QuadTree tree(Rect);
  tree.add(points[0]);
  tree.add(points[1]);
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[0] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[1] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[2] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[3] == nullptr);

  REQUIRE(tree.m_pRoot->m_aChildrens[0]->m_iChildrens > -1);
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[0]->m_aChildrens[0] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[0]->m_aChildrens[1] == nullptr);
  REQUIRE_FALSE(tree.m_pRoot->m_aChildrens[0]->m_aChildrens[2] == nullptr);
  REQUIRE(tree.m_pRoot->m_aChildrens[0]->m_aChildrens[3] == nullptr);

  //tree.remove(points[1]);
  //REQUIRE_FALSE(tree.m_pRoot == nullptr);
  //REQUIRE_FALSE(tree.m_pRoot->m_iChildrens > -1);
}