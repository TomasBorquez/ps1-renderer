#include <math.h>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>
#include <cglm.h>

#define BASE_IMPLEMENTATION
#include "base.h"

#include "gl.h"
#include "camera.h"
#include "renderer.h"

#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 800
#include "texture_box.c"

i32 main() {
  InitRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

  RenderObject textObj = CreateTextureBox();
  renderer.camera = CameraCreate((vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, 1.0f, 0.0f}, -90.0f, 0.0f);

  vec3 cubePositions[] = {
    {0.0f,  0.0f,  0.0f  },
    {2.0f,  5.0f,  -15.0f},
    {-1.5f, -2.2f, -2.5f },
    {-3.8f, -2.0f, -12.3f},
    {2.4f,  -0.4f, -3.5f },
    {-1.7f, 3.0f,  -7.5f },
    {1.3f,  -2.0f, -2.5f },
    {1.5f,  2.0f,  -2.5f },
    {1.5f,  0.2f,  -1.5f },
    {-1.3f, 1.0f,  -1.5f }
  };

  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(renderer.camera.fov), 800.0f / 800.0f, 0.1f, 100.0f, projection);
  ShaderSetMat4(textObj.shaderID, "projection", projection);
  while (!renderer.quit) {
    BeginDrawing();
    {
      ClearScreen(BLACK);

      mat4 view = GLM_MAT4_IDENTITY_INIT;
      CameraGetViewMatrix(&renderer.camera, view);
      UseTextureBox(&textObj, view);
      for (size_t i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++) {
        vec3 *currCube = &cubePositions[i];
        mat4 model = GLM_MAT4_IDENTITY_INIT;

        f32 currTime = (f32)SDL_GetTicks() / 1000;
        glm_translate(model, *currCube);
        glm_rotate(model, currTime + i, (vec3){1.0f, 0.3f, 0.5f});

        DrawTextureBox(&textObj, model);
      }
    }
    EndDrawing();
  }

  DestroyTextureBox(&textObj);
  DestroyRenderer();
}
