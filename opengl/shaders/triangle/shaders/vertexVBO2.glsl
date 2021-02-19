#version 450 core

layout(location=0) in vec4 iPositions;
layout(location=1) in vec4 iColor;

out OUT {
  vec4 color;
};

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
  gl_Position = iPositions;
  color = iColor;
}
