#version 460 core
in vec3 aPos;

#include "./common/matrices_ssbo.glsl"

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
