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

static auto parseObj(const std::string &filename, std::vector<Vec3f> &verts_, std::vector<std::vector<int> > &faces_) -> bool {
  std::ifstream in(filename);
  if(in.fail()) {
    std::cerr << "Can not parse \"" << filename << "\" file\n";
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

auto main(int argc, char* argv[]) -> int {
  std::string inputFileName;
  if(argc != 2) {
    std::cerr << "Usage:\n\t./" << argv[0] << " input.ply\n";
    return EXIT_FAILURE;
  }
  inputFileName = argv[1];

  if(SDL_Init(SDL_INIT_VIDEO) != SDL_SUCCESS) {
    std::cerr << "Can not initialize SDL\n";
    return EXIT_FAILURE;
  }

  constexpr auto WIDTH        = 640U;
  constexpr auto HEIGHT       = 480U;
  constexpr auto WINDOW_FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
  auto pWindow   = SDL_CreateWindow("mimicOpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    WIDTH, HEIGHT, WINDOW_FLAGS);
  auto pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
  auto pTexture  = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 480);
  auto image     = Image(WIDTH, HEIGHT);

  std::vector<Vec3f> verts_;
  std::vector<std::vector<int> > faces_;
  if(!parseObj(inputFileName, verts_, faces_)) {
    return EXIT_FAILURE;
  }

  for(uint32_t i = 0; i < verts_.size(); i++) {
    const auto &face = faces_[i];
    for(uint32_t j = 0; j < 3; j++) {
      const Vec3f v0 = verts_[static_cast<uint32_t>(face[j])];
      const Vec3f v1 = verts_[static_cast<uint32_t>(face[(j + 1) % 3])];
      const int x0   = static_cast<int>((v0.x + 1.F) * WIDTH / 2.F);
      const int y0   = static_cast<int>((v0.y + 1.F) * HEIGHT / 2.F);
      const int x1   = static_cast<int>((v1.x + 1.F) * WIDTH / 2.F);
      const int y1   = static_cast<int>((v1.y + 1.F) * HEIGHT / 2.F);
      vec3_8u white;
      white.x = 255;
      white.y = 255;
      white.z = 255;
      mimicOpenGL::line4(x0, y0, x1, y1, white, image);
    }
  }

  auto bRunning = true;
  while(bRunning) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    if(event.type == SDL_QUIT) {
      bRunning = false;
    }

    SDL_UpdateTexture(pTexture, nullptr, image.m_pData, WIDTH * sizeof(Uint32));
    SDL_RenderClear(pRenderer);
    SDL_RenderCopy(pRenderer, pTexture, nullptr, nullptr);
    SDL_RenderPresent(pRenderer);
  }

  SDL_DestroyTexture(pTexture);
  SDL_DestroyRenderer(pRenderer);
  SDL_DestroyWindow(pWindow);
  SDL_Quit();

  return EXIT_SUCCESS;
}

