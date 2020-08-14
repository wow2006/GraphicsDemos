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
  void update([[maybe_unused]] float delta, const uint8_t* pStatus);

  glm::mat4 view() const;

private:
  glm::vec3 mPosition{10, 10, 10};
  glm::vec3 mTarget{};
  glm::vec3 mUp{0, 1, 0};

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

