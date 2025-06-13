#version 460 core

/* In */
in vec3 FragPos;
in vec3 Norm;
in vec2 TextCoords;

/* Out */
out vec4 FragColor;

/* Type Definitions */
struct DirLight {
  vec3 direction; // 12 bytes
  float _pad0;    // 4 bytes -> total 16

  vec3 ambient; // 12 bytes
  float _pad1;  // 4 bytes -> total 16

  vec3 diffuse; // 12 bytes
  float _pad2;  // 4 bytes -> total 16

  vec3 specular; // 12 bytes
  float _pad3;   // 4 bytes -> total 16
}; // Total: 64 bytes

struct SpotLight {
  vec3 position; // 12 bytes
  float _pad0;   // 4 bytes -> 16

  vec3 direction; // 12 bytes
  float _pad1;    // 4 bytes -> 16

  vec3 ambient; // 12 bytes
  float _pad2;  // 4 bytes -> 16

  vec3 diffuse; // 12 bytes
  float _pad3;  // 4 bytes -> 16

  vec3 specular; // 12 bytes
  float cutOff;  // 4 bytes -> 16

  float outerCutOff; // 4 bytes
  float linear;      // 4 bytes
  float quadratic;   // 4 bytes
  float _pad4;       // 4 bytes -> 16
}; // Total: 96 bytes

struct PointLight {
  vec3 position; // 12 bytes
  float _pad0;   // 4 bytes -> 16

  vec3 ambient; // 12 bytes
  float _pad1;  // 4 bytes -> 16

  vec3 diffuse; // 12 bytes
  float _pad2;  // 4 bytes -> 16

  vec3 specular; // 12 bytes
  float linear;  // 4 bytes -> 16

  float quadratic; // 4 bytes
  vec3 _pad3;      // 12 bytes -> 16
}; // Total: 80 bytes

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
#define MAX_SPOT_LIGHTS 4
#define MAX_POINT_LIGHTS 10
layout(std430, binding = 1) buffer LightingData {
  vec3 viewPos; // 12 bytes
  float _pad0;  // 4 bytes -> 16

  int isNight;        // 4 bytes
  int numDirLights;   // 4 bytes
  int numSpotLights;  // 4 bytes
  int numPointLights; // 4 bytes -> 16

  // Light data
  DirLight dirLight;                        // 64 bytes
  SpotLight spotLights[MAX_SPOT_LIGHTS];    // 96 * 4 = 384 bytes
  PointLight pointLights[MAX_POINT_LIGHTS]; // 80 * 10 = 800 bytes
}; // Total: 1280 bytes
uniform Material material;

void main() {
  vec3 norm = normalize(Norm);
  vec3 viewDir = normalize(vec3(viewPos) - FragPos);
  vec4 textureColor = texture(material.texture_diffuse1, TextCoords);
  if (textureColor.a < 0.9) {
    discard;
  }

  vec3 result = vec3(0.0f);

  if (numDirLights > 0) {
    result += CalcDirLight(dirLight, norm, viewDir);
  }
  for (int i = 0; i < numSpotLights && i < MAX_SPOT_LIGHTS; i++) {
    result += CalcSpotLight(spotLights[i], norm, viewDir);
  }
  for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; i++) {
    result += CalcPointLight(pointLights[i], norm, viewDir);
  }

  if (isNight == 1) {
    float fogDensity = 3.5;
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

float near = 0.1;
float far = 10.0;
float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0;
  return ((2.0 * near * far) / (far + near - z * (far - near))) / far;
}
