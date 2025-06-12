#version 460 core

/* In */
in vec3 FragPos;
in vec3 Norm;
in vec2 TextCoords;

/* Out */
out vec4 FragColor;

/* Types Definitions */
struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  bool isActive;
};

struct SpotLight {
  vec3 position;
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float cutOff;
  float outerCutOff;

  float linear;
  float quadratic;

  bool isActive;
};

struct PointLight {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float linear;
  float quadratic;

  bool isActive;
};

struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_specular1;
  float shininess;
};

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
float LinearizeDepth(float depth);

/* Uniforms */
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform PointLight pointLight;

uniform Material material;

uniform vec3 viewPos;

uniform bool isNight;

/* State */
float near = 0.1;
float far = 10.0;

/* main */
void main() {
  vec3 norm = normalize(Norm);
  vec3 viewDir = normalize(viewPos - FragPos);

  vec4 textureColor = texture(material.texture_diffuse1, TextCoords);
  if (textureColor.a < 0.9) {
    discard;
  }

  vec3 result = vec3(0.0f);
  if (pointLight.isActive) {
    result += CalcPointLight(pointLight, norm, viewDir);
  }
  if (spotLight.isActive) {
    result += CalcSpotLight(spotLight, norm, viewDir);
  }
  if (dirLight.isActive) {
    result += CalcDirLight(dirLight, norm, viewDir);
  }

  if (isNight) {
    float fogDensity = 3.0;
    float depth = LinearizeDepth(gl_FragCoord.z);
    float depthVec = exp(-pow(depth * fogDensity, 2.0));
    vec3 fogColor = vec3(0.01, 0.01, 0.015);
    result = mix(fogColor, result, depthVec);
  } else {
    float fogDensity = 3.0;
    float depth = LinearizeDepth(gl_FragCoord.z);
    float depthVec = exp(-pow(depth * fogDensity, 2.0));
    vec3 fogColor = vec3(0.15, 0.1, 0.1);
    result = mix(fogColor, result, depthVec);
  }

  FragColor = vec4(result, 1.0);
}

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

float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0;
  return ((2.0 * near * far) / (far + near - z * (far - near))) / far;
}
