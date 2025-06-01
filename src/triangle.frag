#version 460 core
in vec2 textCoords;

out vec4 FragColor;

uniform sampler2D texture1;

void main() {
  FragColor = texture(texture1, textCoords);
}
