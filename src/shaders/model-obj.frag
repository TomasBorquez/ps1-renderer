#version 460 core

in vec3 FragPos;
in vec3 Norm;
in vec2 TextCoords;

out vec4 FragColor;

#include "./common/light_ssbo.glsl"
#include "./common/lighting.glsl"
#include "./common/fog.glsl"

/* Consts */
const float near = 0.1;
const float far = 10.0;

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

  vec3 fogColor;
  float fogDensity;
  if (isNight == 1) {
    fogDensity = 3.0;
    fogColor = vec3(0.01, 0.01, 0.015);
  } else {
    fogDensity = 3.5;
    fogColor = vec3(0.15, 0.1, 0.1);
  }
  float depth = LinearizeDepth(gl_FragCoord.z, near, far);
  float depthVec = exp(-pow(depth * fogDensity, 2.0));
  result = mix(fogColor, result, depthVec);

  FragColor = vec4(result, 1.0);
}
