#pragma once
// GLM
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

struct controller {
};

struct camera {

  glm::mat4 view() const;

private:
  glm::vec3 mPosition{10, 10, 10};
  glm::vec3 mTarget{};
  glm::vec3 mUp{0, 1, 0};
};

