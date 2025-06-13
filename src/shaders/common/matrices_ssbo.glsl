#ifndef MATRICES_SSBO_H
#define MATRICES_SSBO_H

/* Uniforms */
layout(std430, binding = 0) buffer Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 model; // NOTE: Changes so we need to pass as uniform
#endif
