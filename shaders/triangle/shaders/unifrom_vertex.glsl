#version 450 core

layout(std140) uniform Positions {
  vec4 uPositions[3];
};

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
  gl_Position = uPositions[gl_VertexID];
}

