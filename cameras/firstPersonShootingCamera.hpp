#pragma once
// SDL
#include <SDL2/SDL.h>
// GLM
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

[[maybe_unused]] constexpr auto SYSTEM_UP = glm::vec3{0, 1, 0};

struct FPSCamera {
public:
  void updateRotation(const glm::vec3& angles);

  void update(float delta, const uint8_t *pStatus);

  glm::mat4 view() const;

  glm::vec3 mSpeed{0.5, 0.5, 0.5};
  glm::vec3 mPosition{0, 0, 10};
  glm::vec3 mTarget{};
  glm::vec3 mUp{0, 1, 0};

  glm::mat4 rotationMatrix;
};

void FPSCamera::updateRotation(const glm::vec3& angles) {
  const auto slowAngles = angles * mSpeed;
  rotationMatrix = glm::yawPitchRoll(slowAngles.x, slowAngles.z, 0.F);
}

void FPSCamera::update(float delta, const uint8_t *pStatus) {
  const auto forward = glm::normalize(mTarget - mPosition);
  if(pStatus[SDL_SCANCODE_W]) {
    mTarget   += forward * delta * mSpeed.x;
    mPosition += forward * delta * mSpeed.x;
  }
  if(pStatus[SDL_SCANCODE_S]) {
    mTarget   -= forward * delta * mSpeed.x;
    mPosition -= forward * delta * mSpeed.x;
  }
  const auto right = glm::normalize(glm::cross(forward, SYSTEM_UP));
  if(pStatus[SDL_SCANCODE_D]) {
    mTarget   += right * delta * mSpeed.x;
    mPosition += right * delta * mSpeed.x;
  }
  if(pStatus[SDL_SCANCODE_A]) {
    mTarget   -= right * delta * mSpeed.x;
    mPosition -= right * delta * mSpeed.x;
  }
}

inline glm::mat4 FPSCamera::view() const { return rotationMatrix * glm::lookAt(mPosition, mTarget, mUp); }
