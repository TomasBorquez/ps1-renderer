#define BASE_IMPLEMENTATION
#include "base.h"

#include <math.h>
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>
#include <cglm.h>

#include "camera.h"
#include "renderer.h"
#include "gl.h"

#include "objects/model_object.c"
#include "objects/light_object.c"
#include "objects/skybox.c"

i32 main() {
  InitRenderer(1920, 1080);

  // Camera
  renderer.camera = CameraCreate((vec3){0.017333f, 0.347597f, -16.877054f}, (vec3){0.0f, 1.0f, 0.0f}, 90.0f, 0.0f);
  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(renderer.camera.fov), (f32)renderer.width / (f32)renderer.height, 0.1f, 100.0f, projection);
  GLCreateSSBOs(projection);

  // Obj Creation
  Object modelObj = ModelObjCreate("./resources/school/school.obj", "./resources/school");
  Object lightObj = LightObjCreate();
  Object skyBox = SkyBoxCreate();

  // Lights
  AttenuationCoeffs attenuation = GetAttenuationCoeffs(50);
  SpotLight spotLight = {
    .ambient = {0.5f, 0.5f, 0.5f},
    .diffuse = {0.5f, 0.5f, 0.5f},
    .specular = {1.0f, 1.0f, 1.0f},
    .cutOff = cos(glm_rad(25.0)),
    .outerCutOff = cos(glm_rad(35.0)),
    .linear = attenuation.linear,
    .quadratic = attenuation.quadratic,
  };

  PointLight pointLight = {
    .position = {0.0f, 0.5f, 0.0f},
    .ambient = {0.3f, 0.1f, 0.1f},
    .specular = {0.3f, 0.1f, 0.1f},
    .diffuse = {0.3f, 0.1f, 0.1f},
    .linear = attenuation.linear,
    .quadratic = attenuation.quadratic,
  };

  DirLight dirLightNight = {
    .direction = {-0.2f, -1.0f, -0.3f},
    .ambient = {0.08f, 0.08f, 0.08f},
    .specular = {0.08f, 0.08f, 0.08f},
    .diffuse = {0.08f, 0.08f, 0.08f},
  };

  DirLight dirLightEvening = {
    .direction = {-0.2f, -1.0f, -0.3f},
    .ambient = {0.4f,  0.3f,  0.3f },
    .specular = {0.4f,  0.3f,  0.3f },
    .diffuse = {0.4f,  0.3f,  0.3f },
  };

  LightingData lightingData = {0};
  while (!renderer.quit) {
    BeginDrawing();
    {
      ClearScreen(BLACK);

      // View Mat
      mat4 view = GLM_MAT4_IDENTITY_INIT;
      CameraGetViewMatrix(&renderer.camera, view);
      GLUpdateView(view);

      // LightingData
      lightingData.numDirLights = 1;
      lightingData.numPointLights = 1;

      f32 currTime = (f32)SDL_GetTicks() / 1000;
      pointLight.position[2] = sin(currTime * 2) + 1.5;

      glm_vec3_copy(renderer.camera.position, lightingData.viewPos);
      glm_vec3_copy(renderer.camera.position, spotLight.position);
      glm_vec3_copy(renderer.camera.front, spotLight.direction);

      if (renderer.isNight) {
        lightingData.isNight = 1;
        lightingData.dirLight = dirLightNight;

        lightingData.numSpotLights = 1;
        lightingData.spotLights[0] = spotLight;
      } else {
        lightingData.isNight = 0;
        lightingData.dirLight = dirLightEvening;

        lightingData.numSpotLights = 0;
      }
      lightingData.pointLights[0] = pointLight;

      GLUpdateLightingSSBO(&lightingData);

      { // Sky Box
        SkyBoxUse(&skyBox);

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_scale(model, (vec3){70.0f, 50.0f, 70.0f});
        SkyBoxDraw(&skyBox, model);
      }
      { // LightObj
        LightObjUse(&lightObj);
        GLSetUniformVecF3(&lightObj, "lightColor", pointLight.ambient);

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(model, pointLight.position);
        glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});
        LightObjDraw(&lightObj, model);
      }
      { // ModelObj
        ModelObjUse(&modelObj);
        GLSetUniformF(&modelObj, "material.shininess", 32.0f);

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(model, (vec3){0.0, 0.0, 2.0f});
        glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});
        ModelObjDraw(&modelObj, model);
      }
    }
    EndDrawing();
  }

  // Cleanup
  ModelObjDestroy(&modelObj);
  LightObjDestroy(&lightObj);
  DestroyRenderer();
}
