#version 450 core

const vec4 cVertices[6] = {
    vec4( 1, 1, 0, 1),
    vec4( 1,-1, 0, 1),
    vec4(-1,-1, 0, 1),

    vec4( 1, 1, 0, 1),
    vec4(-1,-1, 0, 1),
    vec4(-1, 1, 0, 1),
};

const vec2 cTexUV[6] = {
    vec2( 1, 1),
    vec2( 1, 0),
    vec2( 0, 0),

    vec2( 1, 1),
    vec2( 0, 0),
    vec2( 0, 1),
};

out vec2 vsTexCoord;

out gl_PerVertex {
  vec4 gl_Position;
};

void main() {
  gl_Position = cVertices[gl_VertexID];
  vsTexCoord  = cTexUV[gl_VertexID];
}
