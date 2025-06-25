#version 460 core
out vec4 FragColor;

#include "./common/light_ssbo.glsl"
#include "./common/fog.glsl"

/* Consts */
const float near = 0.1;
const float far = 10.0;

void main() {
  vec3 result = vec3(0);
  vec3 fogColor;
  float fogDensity = 3.5;
  if (isNight == 1) {
    fogDensity = 3.5;
    fogColor = vec3(0.013, 0.013, 0.015);
  } else {
    fogDensity = 3.5;
    fogColor = vec3(0.15, 0.1, 0.1);
  }
  float depth = LinearizeDepth(gl_FragCoord.z, near, far);
  float depthVec = exp(-pow(depth * fogDensity, 2.0));
  result = mix(fogColor, result, depthVec);

  FragColor = vec4(result, 1.0f);
}
