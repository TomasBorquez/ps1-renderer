#version 460 core
layout(location = 0) in vec3 aPos;

layout(std430, binding = 0) buffer Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 model;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
