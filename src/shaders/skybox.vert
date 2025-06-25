#version 460 core
in vec3 aPos;
in vec3 aNorm;

out vec3 Norm;
out vec3 FragPos;

layout(std430, binding = 0) buffer Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 model;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  FragPos = vec3(model * vec4(aPos, 1.0));
  Norm = mat3(transpose(inverse(model))) * aNorm;
}
