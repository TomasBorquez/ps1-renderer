#version 460 core
in vec3 Norm;
in vec3 FragPos;
in vec2 TextCoords;

out vec4 FragColor;

struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_diffuse2;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};
uniform Material material;

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform DirLight dirLight;

struct SpotLight {
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float linear;
  float quadratic;
};
uniform SpotLight spotLight;

struct PointLight {
  vec3 position;

  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform PointLight pointLight;

uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);

void main() {
  vec3 norm = normalize(Norm);
  vec3 viewDir = normalize(viewPos - FragPos);

  vec3 result = CalcPointLight(pointLight, norm, viewDir);
  // result += CalcSpotLight(spotLight, norm, viewDir);

  FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);
  // Diffuse
  float diff = max(dot(normal, lightDir), 0.0);

  // Specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  // Result
  vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TextCoords)) * vec3(texture(material.texture_diffuse2, TextCoords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TextCoords));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TextCoords));
  return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
  const float lightConstant = 1.0f;

  vec3 lightDir = normalize(light.position - FragPos);
  // Diffuse
  float diff = max(dot(normal, lightDir), 0.0);

  // Specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  // Attenuation
  float distance = length(light.position - FragPos);
  float attenuation = 1.0 / (lightConstant + light.linear * distance + light.quadratic * (distance * distance));

  // Result
  vec3 diffuse1 = vec3(texture(material.texture_diffuse1, TextCoords));
  vec3 diffuse2 = vec3(texture(material.texture_diffuse2, TextCoords));
  vec3 blendedDiffuse = mix(diffuse1, diffuse2, 0.2); // 50/50 blend

  // Result
  vec3 ambient = light.ambient * blendedDiffuse;
  vec3 diffuse = light.diffuse * diff * blendedDiffuse;
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TextCoords));
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
  const float lightConstant = 1.0f;

  vec3 lightDir = normalize(light.position - FragPos);

  // Diffuse
  float diff = max(dot(normal, lightDir), 0.0);

  // Specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  // Attenuation
  float distance = length(light.position - FragPos);
  float attenuation = 1.0 / (lightConstant + light.linear * distance + light.quadratic * (distance * distance));

  // SpotLight
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  vec3 diffuse1 = vec3(texture(material.texture_diffuse1, TextCoords));
  vec3 diffuse2 = vec3(texture(material.texture_diffuse2, TextCoords));
  // vec3 blendedDiffuse = max(mix(diffuse1, diffuse2, 0.2), 0.5f); // 50/50 blend
  vec3 blendedDiffuse = vec3(1.0f); // 50/50 blend

  // Result
  vec3 ambient = light.ambient * blendedDiffuse;
  vec3 diffuse = light.diffuse * diff * blendedDiffuse;
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TextCoords));
  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;
  specular *= attenuation * intensity;
  return (ambient + diffuse + specular);
}
