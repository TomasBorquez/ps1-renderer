#pragma once
#include <cglm.h>
#include <assimp/scene.h>

#include "base.h"
#include "objects/obj.h"

typedef struct {
  vec3 direction; // 12 bytes
  f32 _pad0;      // 4 bytes -> total 16

  vec3 ambient; // 12 bytes
  f32 _pad1;    // 4 bytes -> total 16

  vec3 diffuse; // 12 bytes
  f32 _pad2;    // 4 bytes -> total 16

  vec3 specular; // 12 bytes
  f32 _pad3;     // 4 bytes -> total 16
} DirLight;      // Total: 64 bytes

typedef struct {
  vec3 position; // 12 bytes
  f32 _pad0;     // 4 bytes -> 16

  vec3 direction; // 12 bytes
  f32 _pad1;      // 4 bytes -> 16

  vec3 ambient; // 12 bytes
  f32 _pad2;    // 4 bytes -> 16

  vec3 diffuse; // 12 bytes
  f32 _pad3;    // 4 bytes -> 16

  vec3 specular; // 12 bytes
  f32 cutOff;    // 4 bytes -> 16

  f32 outerCutOff; // 4 bytes
  f32 linear;      // 4 bytes
  f32 quadratic;   // 4 bytes
  f32 _pad4;       // 4 bytes -> 16
} SpotLight;       // Total: 96 bytes

typedef struct {
  vec3 position; // 12 bytes
  f32 _pad0;     // 4 bytes -> 16

  vec3 ambient; // 12 bytes
  f32 _pad1;    // 4 bytes -> 16

  vec3 diffuse; // 12 bytes
  f32 _pad2;    // 4 bytes -> 16

  vec3 specular; // 12 bytes
  f32 linear;    // 4 bytes -> 16

  f32 quadratic;  // 4 bytes
  float _pad3[3]; // 12 bytes -> 16
} PointLight;     // Total: 80 bytes

#define MAX_SPOT_LIGHTS 4
#define MAX_POINT_LIGHTS 10
typedef struct {
  vec3 viewPos; // 12 bytes
  f32 _pad0;    // 4 bytes -> 16

  i32 isNight;        // 4 bytes
  i32 numDirLights;   // 4 bytes
  i32 numSpotLights;  // 4 bytes
  i32 numPointLights; // 4 bytes -> 16

  // Light data
  DirLight dirLight;                        // 64 bytes
  SpotLight spotLights[MAX_SPOT_LIGHTS];    // 96 * 4 = 384 bytes
  PointLight pointLights[MAX_POINT_LIGHTS]; // 80 * 10 = 800 bytes
} LightingData;                             // Total: 1280 bytes

#define MAX_UNIFORM_COUNT 200
typedef struct {
  String key;
  i32 value;
} Uniform;

typedef struct {
  u32 shaderID;
  u32 VAO;
  u32 VBO;
  u32 SSBO;
  u32 MatricesSSBO;
  u32 LightingSSBO;
  Uniform UniformHashMap[MAX_UNIFORM_COUNT];
  size_t UniformHashMapCount;
} OpenGLContext;
extern OpenGLContext GLContext;

/* Bind */
void GLUnbindShader();
void GLUnbindVAO();
void GLUnbindVBO();
void GLUnbindEBO();
void GLUnbindUBO();

void GLShaderUse(u32 id);
void GLBindVAO(u32 id);
void GLBindVBO(u32 id);
void GLBindEBO(u32 id);
void GLBindSSBO(u32 id);

/* Create */
u32 GLCreateTexture(char *texturePath);
void GLCreateSSBOs(mat4 projection);

/* Update */
void GLUpdateView(mat4 view);
void GLUpdateLightingSSBO(LightingData *lightingData);

/* Uniform */
void GLSetUniformMat4(Object *obj, const char *name, mat4 value);
void GLSetUniformVecF4(Object *obj, const char *name, vec4 value);
void GLSetUniformVecF3(Object *obj, const char *name, vec3 value);
void GLSetUniformVecF2(Object *obj, const char *name, vec2 value);
void GLSetUniformB(Object *obj, const char *name, bool value);
void GLSetUniformI(Object *obj, const char *name, i32 value);
void GLSetUniformF(Object *obj, const char *name, f32 value);

typedef struct {
  f32 linear;
  f32 quadratic;
} AttenuationCoeffs;
AttenuationCoeffs GetAttenuationCoeffs(i32 distance);

#if 1

#  define GL_ERROR_CHECK()                                                                                \
    do {                                                                                                  \
      GLenum error = glGetError();                                                                        \
      if (error != GL_NO_ERROR) {                                                                         \
        LogError("OpenGL Error at %s:%d in %s() - Error: 0x%x", __FILE__, __LINE__, __FUNCTION__, error); \
                                                                                                          \
        /* Clear any additional errors that might be queued */                                            \
        while (glGetError() != GL_NO_ERROR) {}                                                            \
      }                                                                                                   \
    } while (0)

#  define GL(call)      \
    do {                \
      call;             \
      GL_ERROR_CHECK(); \
    } while (0)

#else
#  define GL(call) call
#  define GL_ERROR_CHECK()
#endif
