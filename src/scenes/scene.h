#pragma once

#include "gl.h"
#include "objects/obj.h"

#define MAX_OBJS 50

typedef struct {
  LightingData lightingData;

  Object objs[MAX_OBJS];
  i32 objsNum;
} SceneState;
extern SceneState sceneState;

SpotLight *SceneGetSpotLight(i32 index);
void ScenePushSpotLight(SpotLight spotLight);

PointLight *SceneGetPointLight(i32 index);
void ScenePushPointLight(PointLight pointLight);

void SceneSetDirLight(DirLight dirLight);

Object *SceneGetObject(i32 index);
void ScenePushObject(Object object);

void SceneUpdateShaders();
