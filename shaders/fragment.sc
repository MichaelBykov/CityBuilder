$input v_normal, v_texcoord0

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
  vec3 lightColor = vec3(u_sunColor);
  vec3 lightDir = -normalize(vec3(u_sunDirection));
  
  float diff = max(dot(v_normal, lightDir), 0.0);
  vec4 diffuse = vec4(diff * lightColor, 1.0);
  
  gl_FragColor = (u_ambient + diffuse) * texture2D(s_albedo, v_texcoord0 * u_textureTile);
}
