#version 460 core
in vec3 Norm;
in vec3 FragPos;
in vec2 TextCoords;

out vec4 FragColor;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  // sampler2D emission;
  float shininess;
};
uniform Material material;

struct DirectionalLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct FlashLight {
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float linear;
  float quadratic;
};
uniform FlashLight light;

uniform vec3 viewPos;

void main() {
  const float lightConstant = 1.0f;

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

  vec3 textureResult = max(vec3(texture(material.specular, TextCoords)), 0.05);
  vec3 specular = light.specular * spec * textureResult;

  // spotlight (soft edges)
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = (light.cutOff - light.outerCutOff);
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  diffuse *= intensity;
  specular *= intensity;

  // Attenuation
  float distance = length(light.position - FragPos);
  float attenuation = 1.0 / (lightConstant + light.linear * distance + light.quadratic * (distance * distance));
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  FragColor = vec4(ambient + diffuse + specular, 1.0);
}
