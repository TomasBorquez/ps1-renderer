#pragma once
#include "base.h"
#include <cglm.h>

u32 ShaderCreate(String vertexShaderPath, String fragmentShaderPath);
void ShaderUse(u32 id);

void ShaderSetMat4(u32 id, const char *name, mat4 value);

void ShaderSetVecF4(u32 id, const char *name, vec4 value);
void ShaderSetVecF3(u32 id, const char *name, vec3 value);
void ShaderSetVecF2(u32 id, const char *name, vec2 value);
void ShaderSetB(u32 id, const char *name, bool value);
void ShaderSetI(u32 id, const char *name, i32 value);
void ShaderSetF(u32 id, const char *name, f32 value);

u32 ShaderCreateTexture(char *texturePath);
