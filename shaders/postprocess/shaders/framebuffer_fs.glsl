#version 450 core

uniform sampler2D uDiffuse;

in vec2 vsTexCoord;

layout(location = 0) out vec4 oColor;

void main() {
  oColor = texture(uDiffuse, vsTexCoord);
}
