#include "camera.h"

// Default camera values
const f32 SENSITIVITY = 0.03f;
const f32 YAW = -90.0f;
const f32 PITCH = 0.0f;
const f32 SPEED = 5.0f;
const f32 ZOOM = 50.0f;

static void cameraUpdateCameraVectors(Camera *camera) {
  vec3 front;
  front[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
  front[1] = sin(glm_rad(camera->pitch));
  front[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
  glm_normalize(front);
  camera->front[0] = front[0];
  camera->front[1] = front[1];
  camera->front[2] = front[2];

  vec3 d;
  glm_cross(camera->front, camera->worldUp, d);
  glm_normalize(d);
  camera->right[0] = d[0];
  camera->right[1] = d[1];
  camera->right[2] = d[2];

  glm_cross(camera->right, camera->front, d);
  glm_normalize(d);
  camera->up[0] = d[0];
  camera->up[1] = d[1];
  camera->up[2] = d[2];
}

Camera CameraCreate(vec3 position, vec3 up, f32 yaw, f32 pitch) {
  Camera result = {0};

  result.position[0] = position[0];
  result.position[1] = position[1];
  result.position[2] = position[2];

  result.worldUp[0] = up[0];
  result.worldUp[1] = up[1];
  result.worldUp[2] = up[2];

  result.front[0] = 0.0f;
  result.front[1] = 0.0f;
  result.front[2] = -1.0f;

  result.yaw = yaw;
  result.pitch = pitch;

  result.movementSpeed = SPEED;
  result.mouseSensitivity = SENSITIVITY;
  result.fov = ZOOM;

  cameraUpdateCameraVectors(&result);
  return result;
}

void CameraGetViewMatrix(Camera *camera, mat4 destination) {
  vec3 center;
  glm_vec3_add(camera->position, camera->front, center);
  glm_lookat(camera->position, center, camera->up, destination);
}

void CameraProcessKeyboard(Camera *camera, CameraMovement direction, float deltaTime) {
  f32 oldYPos = camera->position[1];
  f32 velocity = camera->movementSpeed * deltaTime;

  if (direction == FORWARD) {
    glm_vec3_muladds(camera->front, velocity, camera->position);
  }

  if (direction == BACKWARD) {
    glm_vec3_muladds(camera->front, -velocity, camera->position);
  }

  if (direction == LEFT) {
    vec3 d;
    glm_cross(camera->front, camera->up, d);
    glm_normalize(d);
    glm_vec3_muladds(d, -velocity, camera->position);
  }

  if (direction == RIGHT) {
    vec3 d;
    glm_cross(camera->front, camera->up, d);
    glm_normalize(d);
    glm_vec3_muladds(d, velocity, camera->position);
  }

  camera->position[1] = oldYPos;
  if (direction == UP) {
    camera->position[1] += velocity;
  }

  if (direction == DOWN) {
    camera->position[1] -= velocity;
  }
}

void CameraProcessMouseMovement(Camera *camera, float xoffset, float yoffset) {
  xoffset *= camera->mouseSensitivity;
  yoffset *= camera->mouseSensitivity;

  camera->yaw += xoffset;
  camera->pitch += yoffset;

  if (camera->pitch > 89.0f) camera->pitch = 89.0f;
  if (camera->pitch < -89.0f) camera->pitch = -89.0f;

  cameraUpdateCameraVectors(camera);
}
