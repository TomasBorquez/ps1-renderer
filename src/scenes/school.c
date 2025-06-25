#include "scene.h"

#include "renderer.h"
#include "objects/model_object.c"
#include "objects/light_object.c"
#include "objects/skybox.c"

void CreateCamera() {
  renderer.camera = CameraCreate((vec3){0.017333f, 0.347597f, -16.877054f}, (vec3){0.0f, 1.0f, 0.0f}, 90.0f, 0.0f);

  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(renderer.camera.fov), (f32)renderer.width / (f32)renderer.height, 0.1f, 100.0f, projection);
  GLCreateSSBOs(projection);
}

void SceneCreate() {
  CreateCamera();

  ScenePushObject(ModelObjCreate(S("./resources/school/school.obj")));
  ScenePushObject(SkyBoxCreate());
  ScenePushObject(LightObjCreate());

  AttenuationCoeffs attenuation = GetAttenuationCoeffs(32);
  SpotLight spotLight = {
    .ambient = {0.5f, 0.5f, 0.5f},
    .diffuse = {0.5f, 0.5f, 0.5f},
    .specular = {1.0f, 1.0f, 1.0f},
    .cutOff = cos(glm_rad(25.0)),
    .outerCutOff = cos(glm_rad(35.0)),
    .linear = attenuation.linear,
    .quadratic = attenuation.quadratic,
  };
  ScenePushSpotLight(spotLight);

  PointLight pointLight = {
    .position = {0.0f, 0.5f, 0.0f},
    .ambient = {0.3f, 0.1f, 0.1f},
    .specular = {0.3f, 0.1f, 0.1f},
    .diffuse = {0.3f, 0.1f, 0.1f},
    .linear = attenuation.linear,
    .quadratic = attenuation.quadratic,
  };
  ScenePushPointLight(pointLight);

  // ModelObj
  ObjTranslate(SceneGetObject(0), 0.0, 0.0, 2.0f);
  ObjScale(SceneGetObject(0), 0.5f, 0.5f, 0.5f);

  // SkyBox
  ObjScale(SceneGetObject(1), 100.0f, 50.0f, 100.0f);
}

static DirLight dirLightNight = {
  .direction = {-0.2f, -1.0f, -0.3f},
  .ambient = {0.1f,  0.1f,  0.12f},
  .specular = {0.1f,  0.1f,  0.12f},
  .diffuse = {0.1f,  0.1f,  0.12f},
};

static DirLight dirLightEvening = {
  .direction = {-0.2f, -1.0f, -0.3f},
  .ambient = {0.4f,  0.3f,  0.3f },
  .specular = {0.4f,  0.3f,  0.3f },
  .diffuse = {0.4f,  0.3f,  0.3f },
};

void SceneUpdate() {
  LightingData *lightData = &sceneState.lightingData;
  PointLight *pointLight = SceneGetPointLight(0);
  SpotLight *spotLight = SceneGetSpotLight(0);

  mat4 view = GLM_MAT4_IDENTITY_INIT;
  CameraGetViewMatrix(&renderer.camera, view);
  GLUpdateView(view);

  f32 currTime = (f32)SDL_GetTicks() / 1000;
  pointLight->position[2] = sin(currTime * 2) + 1.5;

  glm_vec3_copy(renderer.camera.position, lightData->viewPos);
  glm_vec3_copy(renderer.camera.position, spotLight->position);
  glm_vec3_copy(renderer.camera.front, spotLight->direction);

  lightData->isNight = renderer.isNight;
  if (renderer.isNight) {
    lightData->numSpotLights = 1;
    SceneSetDirLight(dirLightNight);
  } else {
    lightData->numSpotLights = 0;
    SceneSetDirLight(dirLightEvening);
  }

  GLUpdateLightingSSBO(&sceneState.lightingData);

  Object *modelObj = SceneGetObject(0);
  Object *skyBox = SceneGetObject(1);
  Object *lightObj = SceneGetObject(2);
  { /* Sky Box */
    SkyBoxUse(skyBox);
    SkyBoxDraw(skyBox);
  }
  { /* LightObj */
    LightObjUse(lightObj);
    GLSetUniformVecF3(lightObj, "lightColor", pointLight->ambient);

    ObjModelMatReset(lightObj);
    ObjTranslate(lightObj, pointLight->position[0], pointLight->position[1], pointLight->position[2]);
    ObjScale(lightObj, 0.2f, 0.2f, 0.2f);

    LightObjDraw(lightObj);
  }
  { /* ModelObj */
    ModelObjUse(modelObj);
    GLSetUniformF(modelObj, "material.shininess", 32.0f);
    ModelObjDraw(modelObj);
  }
}

void SceneDestroy() {
  Object *modelObj = SceneGetObject(0);
  Object *skyBox = SceneGetObject(1);
  Object *lightObj = SceneGetObject(2);

  ModelObjDestroy(modelObj);
  LightObjDestroy(lightObj);
  SkyBoxDestroy(skyBox);

  DestroyRenderer();
}
