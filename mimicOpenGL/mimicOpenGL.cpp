// STL
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
// SDL2
#include <SDL2/SDL.h>
// Internal
#include "Image.hpp"
#include "Line.hpp"

template<class t>
struct Vec3 {
  union {
    struct {
      t x, y, z;
    };
    struct {
      t ivert, iuv, inorm;
    };
    t raw[3];
  };
  Vec3() : x(0), y(0), z(0) {}
  Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
  inline Vec3<t> operator^(const Vec3<t> &v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
  inline Vec3<t> operator+(const Vec3<t> &v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
  inline Vec3<t> operator-(const Vec3<t> &v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
  inline Vec3<t> operator*(float f) const { return Vec3<t>(x * f, y * f, z * f); }
  inline t operator*(const Vec3<t> &v) const { return x * v.x + y * v.y + z * v.z; }
  float norm() const { return std::sqrt(x * x + y * y + z * z); }
  Vec3<t> &normalize(t l = 1) {
    *this = (*this) * (l / norm());
    return *this;
  }
  template<class>
  friend std::ostream &operator<<(std::ostream &s, Vec3<t> &v);
};

using Vec3f = Vec3<float>;

auto parseObj(const std::string &filename, std::vector<Vec3f> &verts_, std::vector<std::vector<int> > &faces_) -> bool {
  std::ifstream in(filename);
  if(in.fail()) {
    return false;
  }

  std::string line;
  while(!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;
    if(!line.compare(0, 2, "v ")) {
      iss >> trash;
      Vec3f v;
      for(int i = 0; i < 3; i++)
        iss >> v.raw[i];
      verts_.push_back(v);
    } else if(!line.compare(0, 2, "f ")) {
      std::vector<int> f;
      int itrash, idx;
      iss >> trash;
      while(iss >> idx >> trash >> itrash >> trash >> itrash) {
        idx--;  // in wavefront obj all indices start at 1, not zero
        f.push_back(idx);
      }
      faces_.push_back(f);
    }
  }
  return true;
}

constexpr auto SDL_SUCCESS = 0;

auto main(int argc, char *argv[]) -> int {
  if(SDL_Init(SDL_INIT_VIDEO) != SDL_SUCCESS) {
    std::cerr << "Can not initialize SDL\n";
    return EXIT_FAILURE;
  }

  constexpr auto width = 640U;
  constexpr auto height = 480U;
  auto pWindow = SDL_CreateWindow("mimicOpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
  auto pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
  auto pTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 480);
  auto image = Image(width, height);

  std::vector<Vec3f> verts_;
  std::vector<std::vector<int> > faces_;
  if(!parseObj("african_head.obj", verts_, faces_)) {
    std::cerr << "Can not parse ply file\n";
    return EXIT_FAILURE;
  }

  for(int i = 0; i < verts_.size(); i++) {
    const auto &face = faces_[i];
    for(int j = 0; j < 3; j++) {
      Vec3f v0 = verts_[face[j]];
      Vec3f v1 = verts_[face[(j + 1) % 3]];
      int x0 = (v0.x + 1.) * width / 2.;
      int y0 = (v0.y + 1.) * height / 2.;
      int x1 = (v1.x + 1.) * width / 2.;
      int y1 = (v1.y + 1.) * height / 2.;
      mimicOpenGL::line4(x0, y0, x1, y1, {255, 255, 255}, image);
    }
  }

  auto bRunning = true;
  while(bRunning) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    if(event.type == SDL_QUIT) {
      bRunning = false;
    }

    SDL_UpdateTexture(pTexture, nullptr, image.m_pData, width * sizeof(Uint32));
    SDL_RenderClear(pRenderer);
    SDL_RenderCopy(pRenderer, pTexture, NULL, NULL);
    SDL_RenderPresent(pRenderer);
  }

  SDL_DestroyTexture(pTexture);
  SDL_DestroyRenderer(pRenderer);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();

  return EXIT_SUCCESS;
}
