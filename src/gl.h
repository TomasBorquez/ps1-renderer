#pragma once
#include <cglm.h>

#include "base.h"
#include "objects/object.h"

u32 ShaderCreate(String vertexShaderPath, String fragmentShaderPath);
void ShaderUse(u32 id);

void ShaderSetMat4(Object *obj, const char *name, mat4 value);

void ShaderSetVecF4(u32 id, const char *name, vec4 value);
void ShaderSetVecF3(Object *obj, const char *name, vec3 value);
void ShaderSetVecF2(u32 id, const char *name, vec2 value);
void ShaderSetB(u32 id, const char *name, bool value);
void ShaderSetI(u32 id, const char *name, i32 value);
void ShaderSetF(Object *obj, const char *name, f32 value);

u32 ShaderCreateTexture(char *texturePath, bool png);
