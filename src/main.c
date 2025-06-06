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
  InitRenderer(1800, 900);

  renderer.camera = CameraCreate((vec3){0.0f, 0.0f, 4.0f}, (vec3){0.0f, 1.0f, 0.0f}, -90.0f, 0.0f);
  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(renderer.camera.fov), (f32)renderer.width / (f32)renderer.height, 0.1f, 100.0f, projection);

  Object modelObj = ModelObjCreate("./resources/backpack/backpack.obj", "./resources/backpack/");
  ShaderSetMat4(&modelObj, "projection", projection);

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
      vec3 lightPos = {sin(currTime * 2), 0.0f, 3.0f};

      glm_translate(lightModel, lightPos);
      glm_scale(lightModel, (vec3){0.2f, 0.2f, 0.2f});
      LightObjDraw(&lightObj, lightModel);

      { // ModelObj
        ModelObjUse(&modelObj, view);

        // View uniform
        ShaderSetVecF3(&modelObj, "viewPos", renderer.camera.position);

        // Spotlight uniform
        ShaderSetVecF3(&modelObj, "spotLight.position", renderer.camera.position);
        ShaderSetVecF3(&modelObj, "spotLight.direction", renderer.camera.front);
        ShaderSetF(&modelObj, "spotLight.cutOff", cos(glm_rad(25)));
        ShaderSetF(&modelObj, "spotLight.outerCutOff", cos(glm_rad(35)));

        ShaderSetVecF3(&modelObj, "spotLight.ambient", (vec3){0.2f, 0.2f, 0.2f});
        ShaderSetVecF3(&modelObj, "spotLight.diffuse", (vec3){0.5f, 0.5f, 0.5f});
        ShaderSetVecF3(&modelObj, "spotLight.specular", (vec3){1.0f, 1.0f, 1.0f});

        ShaderSetF(&modelObj, "spotLight.linear", 0.09f);
        ShaderSetF(&modelObj, "spotLight.quadratic", 0.032f);

        ShaderSetB(&modelObj, "spotLight.isActive", true);

        // PointLight uniform
        ShaderSetVecF3(&modelObj, "pointLight.position", lightPos);

        ShaderSetF(&modelObj, "pointLight.linear", 0.09f);
        ShaderSetF(&modelObj, "pointLight.quadratic", 0.032f);

        ShaderSetVecF3(&modelObj, "pointLight.ambient", (vec3){0.5f, 0.3f, 0.3f});
        ShaderSetVecF3(&modelObj, "pointLight.specular", (vec3){0.5f, 0.3f, 0.3f});
        ShaderSetVecF3(&modelObj, "pointLight.diffuse", (vec3){0.5f, 0.5f, 0.5f});

        ShaderSetB(&modelObj, "pointLight.isActive", true);

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
