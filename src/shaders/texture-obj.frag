#version 460 core
in vec3 Norm;
in vec3 FragPos;
in vec2 TextCoords;

out vec4 FragColor;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};
uniform Material material;

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform Light light;

uniform vec3 viewPos;

void main() {
  // Ambient
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TextCoords));

  // Diffuse
  vec3 norm = normalize(Norm);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TextCoords));

  // Specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * vec3(texture(material.specular, TextCoords)));

  FragColor = vec4(ambient + diffuse + specular, 1.0);
}
