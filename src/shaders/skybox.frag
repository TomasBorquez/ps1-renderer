#version 460 core
out vec4 FragColor;

uniform bool isNight;

float near = 0.1;
float far = 10.0;

float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0;
  return ((2.0 * near * far) / (far + near - z * (far - near))) / far;
}

void main() {
  vec3 result = vec3(0.0);

  if (isNight) {
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
