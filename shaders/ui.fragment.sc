$input v_texcoord0

#include "bgfx_shader.sh"

SAMPLER2D(s_ui, 1);

void main() {
  gl_FragColor = texture2D(s_ui, v_texcoord0);
}
