#version 450 core

in IN {
  vec4 color;
};

layout(location = 0) out vec4 oColor;

void main() {
  oColor = color;
}
