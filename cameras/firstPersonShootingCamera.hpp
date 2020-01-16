#pragma once
// GLM
#include <glm/glm.hpp>

constexpr auto g_fSpeed = 0.1F;

struct FPSCamera {
public:
  void walk(float deltaTime);

  void strafe(float deltaTime);

  void lift(float deltaTime);

  void update();

  glm::mat4 view() const;

private:
  glm::vec3 mDeltas;
  glm::vec3 mPosition;
  glm::vec3 mRotation;
};

void FPSCamera::walk(float deltaTime) {
  mDeltas.x += (g_fSpeed * deltaTime);
}

void FPSCamera::strafe(float deltaTime) {
  mDeltas.y += (g_fSpeed * deltaTime);
}

void FPSCamera::lift(float deltaTime) {
  mDeltas.z += (g_fSpeed * deltaTime);
}

void FPSCamera::update() {
}
