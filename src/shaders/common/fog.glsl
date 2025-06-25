#ifndef FOG_H
#define FOG_H

float LinearizeDepth(float depth, float near, float far) {
  float z = depth * 2.0 - 1.0;
  return ((2.0 * near * far) / (far + near - z * (far - near))) / far;
}

#endif
