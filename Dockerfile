FROM archlinux/base

# Install prerequisites
RUN pacman -Syu --noconfirm
RUN pacman -S \
  gcc         \
  clang       \
  ninja       \
  cmake       \
  git         \
  sdl2        \
  mesa        \
  --noconfirm

# Clone project
RUN mkdir /source/build -p
WORKDIR /source
RUN git clone https://github.com/wow2006/GraphicsDemos.git src
ADD remove-cmake.patch /source/src
RUN cd src && git apply < remove-cmake.patch

ARG COMPILER=g++
ENV COMPILER=$COMPILER
ARG BUILD=Release
ENV BUILD=$BUILD

CMD cmake -G Ninja -DCMAKE_CXX_COMPILER=$COMPILER -DCMAKE_BUILD_TYPE=$BUILD -S /source/src -B /source/build && \
    cmake --build /source/build

