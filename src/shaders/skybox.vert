#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;

out vec3 Norm;
out vec3 FragPos;

layout(std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 model;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  FragPos = vec3(model * vec4(aPos, 1.0));
  Norm = mat3(transpose(inverse(model))) * aNorm;
}
