#pragma once
#include "base.h"
#include <cglm.h>

u32 CreateShader(String vertexShaderPath, String fragmentShaderPath);

void UseShader(u32 id);

void SetMat4Shader(u32 id, const char *name, mat4 value);

void SetVecF4Shader(u32 id, const char *name, vec4 value);
void SetVecF3Shader(u32 id, const char *name, vec3 value);
void SetVecF2Shader(u32 id, const char *name, vec2 value);
void SetBShader(u32 id, const char *name, bool value);
void SetIShader(u32 id, const char *name, i32 value);
void SetFShader(u32 id, const char *name, f32 value);
