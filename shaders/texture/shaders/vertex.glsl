#version 450 core

const vec4 cVertices[6] = {
    vec4( 1, 1, 0, 1),
    vec4( 1,-1, 0, 1),
    vec4(-1,-1, 0, 1),

    vec4( 1, 1, 0, 1),
    vec4(-1,-1, 0, 1),
    vec4(-1, 1, 0, 1),
};

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  gl_Position = cVertices[gl_VertexID];
}
