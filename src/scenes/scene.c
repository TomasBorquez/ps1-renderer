#include "scene.h"
#include "shader.h"

SceneState sceneState = {0};

/* SpotLight */
SpotLight *SceneGetSpotLight(i32 index) {
  return &sceneState.lightingData.spotLights[index];
}

void ScenePushSpotLight(SpotLight spotLight) {
  Assert(sceneState.lightingData.numSpotLights <= MAX_SPOT_LIGHTS, "PushSpotLight: failed, cannot push more than %d spotLights", MAX_SPOT_LIGHTS);
  sceneState.lightingData.spotLights[sceneState.lightingData.numSpotLights++] = spotLight;
}

/* PointLight */
PointLight *SceneGetPointLight(i32 index) {
  return &sceneState.lightingData.pointLights[index];
}

void ScenePushPointLight(PointLight pointLight) {
  Assert(sceneState.lightingData.numPointLights <= MAX_SPOT_LIGHTS, "PushPointLight: failed, cannot push more than %d pointLights", MAX_POINT_LIGHTS);
  sceneState.lightingData.pointLights[sceneState.lightingData.numPointLights++] = pointLight;
}

/* DirLight */
void SceneSetDirLight(DirLight dirLight) {
  sceneState.lightingData.numDirLights = 1;
  sceneState.lightingData.dirLight = dirLight;
}

/* Object */
Object *SceneGetObject(i32 index) {
  Assert(index < sceneState.objsNum, "SceneGetObject: failed, cannot get index %d out of bounds, length: %d", index, sceneState.objsNum);
  return &sceneState.objs[index];
}

void ScenePushObject(Object object) {
  Assert(sceneState.objsNum <= MAX_OBJS, "PushObject: failed, cannot push more than %d objects", MAX_OBJS);
  sceneState.objs[sceneState.objsNum++] = object;
}

/* Shaders */
void SceneUpdateShaders() {
  LogWarn("Reloading Shaders");
  for (i32 i = 0; i < sceneState.objsNum; i++) {
    Object *currObj = &sceneState.objs[i];
    GLCreateShader(currObj, currObj->shader.vertexPath, currObj->shader.fragmentPath);
  }
}
