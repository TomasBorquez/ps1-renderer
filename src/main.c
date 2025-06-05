#include <math.h>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>
#include <cglm.h>

#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 800
#define BASE_IMPLEMENTATION
#include "base.h"

#include "gl.h"
#include "camera.h"
#include "renderer.h"

#include "objects/texture_object.c"
#include "objects/light_object.c"

i32 main() {
  InitRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

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

  renderer.camera = CameraCreate((vec3){0.0f, 0.0f, 4.0f}, (vec3){0.0f, 1.0f, 0.0f}, -90.0f, 0.0f);
  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(renderer.camera.fov), 800.0f / 800.0f, 0.1f, 100.0f, projection);

  Object textObj = TextureObjCreate();
  ShaderSetMat4(&textObj, "projection", projection);

  Object lightObj = LightObjCreate();
  ShaderSetMat4(&lightObj, "projection", projection);
  while (!renderer.quit) {
    BeginDrawing();
    {
      ClearScreen(BLACK);
      f32 currTime = (f32)SDL_GetTicks() / 1000;

      // View mat
      mat4 view = GLM_MAT4_IDENTITY_INIT;
      CameraGetViewMatrix(&renderer.camera, view);

      // LightObj
      LightObjUse(&lightObj, view);
      mat4 lightModel = GLM_MAT4_IDENTITY_INIT;
      // vec3 lightPos = {0.0f, 0.0f, 5.0f};
      vec3 lightPos = {sin(currTime) * 5, 0.0f, 5.0f};

      glm_translate(lightModel, lightPos);
      glm_scale(lightModel, (vec3){0.2f, 0.2f, 0.2f});
      LightObjDraw(&lightObj, lightModel);

      // TextureObj
      TextureObjUse(&textObj, view);
      for (size_t i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++) {
        vec3 *currCube = &cubePositions[i];
        mat4 staticModel = GLM_MAT4_IDENTITY_INIT;

        glm_translate(staticModel, *currCube);
        glm_rotate(staticModel, currTime + i, (vec3){1.0f, 0.3f, 0.5f});

        ShaderSetVecF3(&textObj, "light.position", renderer.camera.position);
        ShaderSetVecF3(&textObj, "light.direction", renderer.camera.front);
        ShaderSetF(&textObj, "light.cutOff", cos(glm_rad(25)));
        ShaderSetF(&textObj, "light.outerCutOff", cos(glm_rad(35)));

        ShaderSetVecF3(&textObj, "light.ambient", (vec3){0.2f, 0.2f, 0.2f});
        ShaderSetVecF3(&textObj, "light.diffuse", (vec3){0.5f, 0.5f, 0.5f});
        ShaderSetVecF3(&textObj, "light.specular", (vec3){1.0f, 1.0f, 1.0f});

        ShaderSetF(&textObj, "light.linear", 0.09f);
        ShaderSetF(&textObj, "light.quadratic", 0.032f);

        ShaderSetF(&textObj, "material.shininess", 32.0f);

        ShaderSetVecF3(&textObj, "viewPos", renderer.camera.position);

        TextureObjDraw(&textObj, staticModel);
      }
    }
    EndDrawing();
  }

  TextureObjDestroy(&textObj);
  LightObjDestroy(&lightObj);
  DestroyRenderer();
}
