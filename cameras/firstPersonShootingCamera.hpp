#pragma once
// GLM
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

constexpr auto g_fSpeed = 0.1F;

constexpr auto UP = glm::vec3{0, 1, 0};

struct FPSCamera {
public:
  void walk(bool bForward);

  void strafe();

  void lift();

  void update([[maybe_unused]] float delta);

  glm::mat4 view() const;

private:
  glm::vec3 mDeltas;
  glm::vec3 mPosition = { 0, 0,-1};
  glm::vec3 mRotation;

  glm::mat4 mView;
};

void FPSCamera::walk(bool bForward) {
  mDeltas.x += g_fSpeed * ((bForward) ? 1 : -1);
}

void FPSCamera::strafe() {
  //mDeltas.y += (g_fSpeed * deltaTime);
}

void FPSCamera::lift() {
  //mDeltas.z += (g_fSpeed * deltaTime);
}

void FPSCamera::update([[maybe_unused]] float delta) {
  mPosition.z += mDeltas.x * delta;
  mDeltas = {0, 0, 0};
  mView = glm::lookAt(mPosition, {0, 0, 0}, UP);
}

inline glm::mat4 FPSCamera::view() const { return mView; }

