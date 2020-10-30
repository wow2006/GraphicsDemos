#version 450 core

layout(location = 0) out vec4 oColor;

layout(location = 0) uniform sampler2D iTexture;

in vertex_out {
    vec2 UV;
};

void main() {
  oColor = texture2D(iTexture, UV);
}
