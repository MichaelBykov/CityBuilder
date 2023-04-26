$input v_normal, v_texcoord0, v_color0

#include "bgfx_shader.sh"

uniform vec4 u_ambient;
uniform vec4 u_sunColor;
uniform vec4 u_sunDirection;
uniform vec2 u_textureTile;

SAMPLER2D(s_albedo, 0);
SAMPLER2D(s_bump, 1);

vec3 reflect(vec3 incident, vec3 normal) {
  return incident - 2.0f * dot(incident, normal) * normal;
}

void main() {
  gl_FragColor = texture2D(s_albedo, v_texcoord0 * u_textureTile) * v_color0;
}
