#version 450 core

const vec4 cPositions[6] = {
  vec4(-1.0, 1.0, 0.0, 1.0),
  vec4( 1.0, 1.0, 0.0, 1.0),
  vec4( 1.0,-1.0, 0.0, 1.0),

  vec4(-1.0, 1.0, 0.0, 1.0),
  vec4( 1.0,-1.0, 0.0, 1.0),
  vec4(-1.0,-1.0, 0.0, 1.0),
};

const vec2 cUV[6] = {
  vec2( 0.0, 1.0),
  vec2( 0.1, 0.1),
  vec2( 1.0,-1.0),

  vec2( 0.0, 1.0),
  vec2( 1.0,-1.0),
  vec2( 0.0, 0.0)
};

out vertex_out {
    vec2 UV;
};

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() {
  gl_Position = cPositions[gl_VertexID];
  UV = cUV[gl_VertexID];
}
