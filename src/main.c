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

#include "objects/static_object.c"
#include "objects/model_object.c"
#include "objects/light_object.c"

i32 main() {
  // InitRenderer(1600, 900);
  InitRenderer(1920, 1080);

  // Camera
  renderer.camera = CameraCreate((vec3){0.017333f, 0.347597f, -16.877054f}, (vec3){0.0f, 1.0f, 0.0f}, 90.0f, 0.0f);
  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(renderer.camera.fov), (f32)renderer.width / (f32)renderer.height, 0.1f, 100.0f, projection);

  // Obj Creation
  Object modelObj = ModelObjCreate("./resources/school_test/school_test.obj", "./resources/school_test");
  ShaderSetMat4(&modelObj, "projection", projection);

  Object lightObj = LightObjCreate();
  ShaderSetMat4(&lightObj, "projection", projection);

  Object skyBox = StaticObjCreate();
  ShaderSetMat4(&skyBox, "projection", projection);

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
    .ambient = {0.03f, 0.03f, 0.03f},
    .specular = {0.03f, 0.03f, 0.03f},
    .diffuse = {0.03f, 0.03f, 0.03f},
  };

  DirLight dirLightEvening = {
    .direction = {-0.2f, -1.0f, -0.3f},
    .ambient = {0.4f,  0.3f,  0.3f },
    .specular = {0.4f,  0.3f,  0.3f },
    .diffuse = {0.4f,  0.3f,  0.3f },
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
      pointLight.position[2] = sin(currTime * 2) + 1.5;
      ShaderSetVecF3(&lightObj, "lightColor", pointLight.ambient);

      glm_translate(lightModel, pointLight.position);
      glm_scale(lightModel, (vec3){0.2f, 0.2f, 0.2f});
      // LightObjDraw(&lightObj, lightModel);
      { // Sky Box
        StaticObjUse(&skyBox, view);
        ShaderSetB(&skyBox, "fog", !renderer.night);

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_scale(model, (vec3){70.0f, 50.0f, 70.0f});
        StaticObjDraw(&skyBox, model);
      }
      { // ModelObj
        ModelObjUse(&modelObj, view);

        // View uniform
        ShaderSetVecF3(&modelObj, "viewPos", renderer.camera.position);

        // Dir Light
        glm_vec3_copy(renderer.camera.position, spotLight.position);
        glm_vec3_copy(renderer.camera.front, spotLight.direction);
        ShaderSetB(&modelObj, "fog", !renderer.night);
        if (renderer.night) {
          ShaderSetDirLight(&modelObj, &dirLightEvening);
        } else {
          ShaderSetDirLight(&modelObj, &dirLightNight);
          ShaderSetSpotLight(&modelObj, &spotLight);
        }

        // PointLight uniform
        // ShaderSetPointLight(&modelObj, &pointLight);

        // Material
        ShaderSetF(&modelObj, "material.shininess", 32.0f);

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(model, (vec3){0.0, 0.0, 2.0f});
        glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});
        ModelObjDraw(&modelObj, model);
      }
    }
    EndDrawing();
  }

  ModelObjDestroy(&modelObj);
  LightObjDestroy(&lightObj);
  DestroyRenderer();
}
