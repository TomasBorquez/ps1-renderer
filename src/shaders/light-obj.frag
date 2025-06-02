#version 460 core
out vec4 FragColor;

void main() {
  vec3 lightColor = vec3(0.33f, 0.42f, 0.18f);
  vec3 toyColor = vec3(1.0f, 0.5f, 0.31f);
  FragColor = vec4(lightColor * toyColor, 0.0f);
}
