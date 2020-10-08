#pragma once
// STL
#include <array>
// SDL
#include <SDL2/SDL.h>
// GLM
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

[[maybe_unused]] constexpr auto SYSTEM_UP = glm::vec3{0, 1, 0};
constexpr auto MOUSE_HISTORY_BUFFER_SIZE = 10U;
constexpr auto MOUSE_FILTER_WEIGHT = 1.0F;

struct FPSCamera {
public:
  void updateRotation(const glm::vec3& angles);

  glm::vec2 filterMouseMoves(glm::vec2 delta);

  glm::mat4 view() const;

  glm::vec3 mSpeed{0.5, 0.5, 0.5};
  glm::vec3 mPosition{0, 0, 10};
  glm::vec3 mTarget{};
  glm::vec3 mUp{0, 1, 0};

  std::array<glm::vec2, MOUSE_HISTORY_BUFFER_SIZE> mMouseHistory;
};

void FPSCamera::update([[maybe_unused]] float delta, const uint8_t* pStatus) {
  const auto forward = glm::normalize(mTarget - mPosition);
  if(pStatus[SDL_SCANCODE_W]) {
    mTarget   += forward;
    mPosition += forward;
  }
  if(pStatus[SDL_SCANCODE_S]) {
    mTarget   -= forward;
    mPosition -= forward;
  }
  if(pStatus[SDL_SCANCODE_A]) {
  }
  if(pStatus[SDL_SCANCODE_D]) {
  }
}

inline glm::mat4 FPSCamera::view() const {
  return glm::lookAt(mPosition, mTarget, mUp);
}

inline glm::mat4 FPSCamera::view() const { return rotationMatrix * glm::lookAt(mPosition, mTarget, mUp); }
