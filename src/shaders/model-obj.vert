#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTextCoords;

out vec3 Norm;
out vec3 FragPos;
out vec2 TextCoords;

layout(std430, binding = 0) buffer Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 model;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);

  TextCoords = aTextCoords;
  FragPos = vec3(model * vec4(aPos, 1.0));
  Norm = mat3(transpose(inverse(model))) * aNorm;
}
