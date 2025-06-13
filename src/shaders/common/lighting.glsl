#ifndef LIGHTING_H
#define LIGHTING_H

/* Type Definitions */
struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_specular1;
  float shininess;
};

/* Uniforms */
uniform Material material;

/* Functions */
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);

  // Diffuse
  float diff = max(dot(normal, lightDir), 0.0);

  // Specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  // Use your current texture setup
  vec3 materialDiffuse = vec3(texture(material.texture_diffuse1, TextCoords));
  vec3 materialSpecular = vec3(texture(material.texture_specular1, TextCoords));

  // Result
  vec3 ambient = light.ambient * materialDiffuse;
  vec3 diffuse = light.diffuse * diff * materialDiffuse;
  vec3 specular = light.specular * spec * materialSpecular;

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

  // Use your current texture setup
  vec3 materialDiffuse = vec3(texture(material.texture_diffuse1, TextCoords));
  vec3 materialSpecular = vec3(texture(material.texture_specular1, TextCoords));

  // Result
  vec3 ambient = light.ambient * materialDiffuse;
  vec3 diffuse = light.diffuse * diff * materialDiffuse;
  vec3 specular = light.specular * spec * materialSpecular;

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

  // SpotLight intensity
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  // Use your current texture setup
  vec3 materialDiffuse = vec3(texture(material.texture_diffuse1, TextCoords));
  vec3 materialSpecular = vec3(texture(material.texture_specular1, TextCoords));

  // Result
  vec3 ambient = light.ambient * materialDiffuse;
  vec3 diffuse = light.diffuse * diff * materialDiffuse;
  vec3 specular = light.specular * spec * materialSpecular;

  // Apply attenuation and intensity
  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;
  specular *= attenuation * intensity;

  return (ambient + diffuse + specular);
}
#endif
