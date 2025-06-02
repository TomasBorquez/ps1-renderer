#version 460 core
in vec2 textCoords;

out vec4 FragColor;

uniform sampler2D texture1;

void main() {
  vec3 lightColor = vec3(0.33f, 0.42f, 0.18f);
  vec3 toyColor = vec3(1.0f, 0.5f, 0.31f);
  vec4 result = vec4(lightColor * toyColor, 0.0f);
  FragColor = texture(texture1, textCoords);
}
