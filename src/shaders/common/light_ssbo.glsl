#ifndef LIGHT_SSBO_H
#define LIGHT_SSBO_H

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
#endif
