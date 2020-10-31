#version 450 core

layout(location=0) in vec4 iPositions;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
  gl_Position = iPositions;
}
