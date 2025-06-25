#version 460 core
in vec3 aPos;
in vec3 aNorm;
in vec2 aTextCoords;

out vec3 Norm;
out vec3 FragPos;
out vec2 TextCoords;

#include "./common/matrices_ssbo.glsl"

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);

  TextCoords = aTextCoords;
  FragPos = vec3(model * vec4(aPos, 1.0));
  Norm = mat3(transpose(inverse(model))) * aNorm;
}
