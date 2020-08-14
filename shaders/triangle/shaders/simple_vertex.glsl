#version 450 core

const vec4 cPositions[3] = {
  vec4( 0.25, -0.25, 0.5, 1.0),
  vec4(-0.25, -0.25, 0.5, 1.0),
  vec4( 0.25,  0.25, 0.5, 1.0)
};

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() {
  gl_Position = cPositions[gl_VertexID];
}

