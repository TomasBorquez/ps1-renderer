#pragma once

#include <cglm.h>

#include "base.h"

typedef enum {
  // Modifies x
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,

  // Modifies y
  UP,
  DOWN
} CameraMovement;

typedef struct {
  vec3 position;
  vec3 front;
  vec3 up;
  vec3 right;
  vec3 worldUp;

  // Euler angles
  f32 yaw;
  f32 pitch;

  // Camera options
  f32 movementSpeed;
  f32 mouseSensitivity;
  f32 fov;
} Camera;

Camera CameraCreate(vec3 position, vec3 up, f32 yaw, f32 pitch);

void CameraGetViewMatrix(Camera *camera, mat4 destination);
void CameraProcessKeyboard(Camera *camera, CameraMovement direction, float deltaTime);
void CameraProcessMouseMovement(Camera *camera, float xoffset, float yoffset);
