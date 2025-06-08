#version 460 core
in vec3 Norm;
in vec3 FragPos;

out vec4 FragColor;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
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

uniform bool fog;

float LinearizeDepth(float depth);

float near = 0.1;
float far = 10.0;
void main() {
  vec3 result = vec3(0.0);

  if (fog) {
    float fogDensity = 3.0;
    float depth = LinearizeDepth(gl_FragCoord.z);
    float depthVec = exp(-pow(depth * fogDensity, 2.0));
    vec3 fogColor = vec3(0.01, 0.01, 0.015);
    result = mix(fogColor, result, depthVec);
  } else {
    result = vec3(0.3, 0.1, 0.1);
    float fogDensity = 3.0;
    float depth = LinearizeDepth(gl_FragCoord.z);
    float depthVec = exp(-pow(depth * fogDensity, 2.0));
    vec3 fogColor = vec3(0.15, 0.1, 0.1);
    result = mix(fogColor, result, depthVec);
  }

  FragColor = vec4(result, 1.0f);
}

float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0;
  return ((2.0 * near * far) / (far + near - z * (far - near))) / far;
}
