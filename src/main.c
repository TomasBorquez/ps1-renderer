#define BASE_IMPLEMENTATION
#include "base.h"

#include <math.h>
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>
#include <cglm.h>

#include "gl.h"

#include "camera.h"
#include "renderer.h"
#include "gl.h"

#include "objects/model_object.c"
#include "objects/light_object.c"

i32 main() {
  InitRenderer(1000, 800);

  // Camera
  renderer.camera = CameraCreate((vec3){0.0f, 0.0f, 4.0f}, (vec3){0.0f, 1.0f, 0.0f}, -90.0f, 0.0f);
  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(renderer.camera.fov), (f32)renderer.width / (f32)renderer.height, 0.1f, 100.0f, projection);

  // Obj Creation
  Object modelObj = ModelObjCreate("./resources/backpack/backpack.obj", "./resources/backpack/");
  ShaderSetMat4(&modelObj, "projection", projection);

  Object lightObj = LightObjCreate();
  ShaderSetMat4(&lightObj, "projection", projection);

  // Lights
  SpotLight spotLight = {
    .ambient = {0.2f, 0.2f, 0.2f},
    .diffuse = {0.5f, 0.5f, 0.5f},
    .specular = {1.0f, 1.0f, 1.0f},
    .cutOff = cos(glm_rad(25)),
    .outerCutOff = cos(glm_rad(35)),
    .linear = 0.09f,
    .quadratic = 0.032f,
  };

  PointLight pointLight = {
    .position = {0.0f, 0.0f, 3.0f},
    .ambient = {0.7f, 0.3f, 0.3f},
    .specular = {0.7f, 0.3f, 0.3f},
    .diffuse = {0.5f, 0.5f, 0.5f},
    .linear = 0.09f,
    .quadratic = 0.032f,
  };
  while (!renderer.quit) {
    BeginDrawing();
    {
      ClearScreen(BLACK);
      f32 currTime = (f32)SDL_GetTicks() / 1000;

      // View Mat
      mat4 view = GLM_MAT4_IDENTITY_INIT;
      CameraGetViewMatrix(&renderer.camera, view);

      // LightObj
      LightObjUse(&lightObj, view);
      mat4 lightModel = GLM_MAT4_IDENTITY_INIT;
      pointLight.position[0] = sin(currTime * 2);
      ShaderSetVecF3(&lightObj, "lightColor", pointLight.ambient);

      glm_translate(lightModel, pointLight.position);
      glm_scale(lightModel, (vec3){0.2f, 0.2f, 0.2f});
      LightObjDraw(&lightObj, lightModel);
      { // ModelObj
        ModelObjUse(&modelObj, view);

        // View uniform
        ShaderSetVecF3(&modelObj, "viewPos", renderer.camera.position);

        // Spotlight uniform
        glm_vec3_copy(renderer.camera.position, spotLight.position);
        glm_vec3_copy(renderer.camera.front, spotLight.direction);
        ShaderSetSpotLight(&modelObj, &spotLight);

        // PointLight uniform
        ShaderSetPointLight(&modelObj, &pointLight);

        // Material
        ShaderSetF(&modelObj, "material.shininess", 32.0f);

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(model, (vec3){0.0, 0.0, 2.0f});
        glm_scale(model, (vec3){0.25f, 0.25f, 0.25f});
        ModelObjDraw(&modelObj, model);
      }
    }
    EndDrawing();
  }

  ModelObjDestroy(&modelObj);
  LightObjDestroy(&lightObj);
  DestroyRenderer();
}
