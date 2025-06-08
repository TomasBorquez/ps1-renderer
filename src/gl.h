#pragma once
#include <cglm.h>
#include <assimp/scene.h>

#include "base.h"
#include "objects/obj.h"

typedef struct {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
} DirLight;

typedef struct {
  vec3 position;
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  f32 cutOff;
  f32 outerCutOff;

  f32 linear;
  f32 quadratic;
} SpotLight;

typedef struct {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float linear;
  float quadratic;
} PointLight;

u32 ShaderCreate(String vertexShaderPath, String fragmentShaderPath);
void ShaderUse(u32 id);

void ShaderSetMat4(Object *obj, const char *name, mat4 value);

void ShaderSetVecF4(Object *obj, const char *name, vec4 value);
void ShaderSetVecF3(Object *obj, const char *name, vec3 value);
void ShaderSetVecF2(Object *obj, const char *name, vec2 value);
void ShaderSetB(Object *obj, const char *name, bool value);
void ShaderSetI(Object *obj, const char *name, i32 value);
void ShaderSetF(Object *obj, const char *name, f32 value);

void ShaderSetDirLight(Object *obj, DirLight *value);
void ShaderSetSpotLight(Object *obj, SpotLight *value);
void ShaderSetPointLight(Object *obj, PointLight *value);

u32 ShaderCreateTexture(char *texturePath);

typedef struct {
  f32 linear;
  f32 quadratic;
} AttenuationCoeffs;
AttenuationCoeffs GetAttenuationCoeffs(i32 distance);
