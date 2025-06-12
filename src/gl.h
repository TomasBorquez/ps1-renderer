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

#define MAX_UNIFORM_COUNT 200
typedef struct {
  String key;
  i32 value;
} Uniform;

typedef struct {
  u32 shaderID;
  u32 VAO;
  u32 VBO;
  u32 UBO;
  u32 MatricesUBO;
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
void GLBindUBO(u32 id);

/* Create */
u32 GLCreateTexture(char *texturePath);
u32 GLCreateShader(String vertexShaderPath, String fragmentShaderPath);
void GLCreateUBOs(mat4 projection);

/* Update */
void GLUpdateView(mat4 view);

/* Uniform */
void GLSetUniformMat4(Object *obj, const char *name, mat4 value);
void GLSetUniformVecF4(Object *obj, const char *name, vec4 value);
void GLSetUniformVecF3(Object *obj, const char *name, vec3 value);
void GLSetUniformVecF2(Object *obj, const char *name, vec2 value);
void GLSetUniformB(Object *obj, const char *name, bool value);
void GLSetUniformI(Object *obj, const char *name, i32 value);
void GLSetUniformF(Object *obj, const char *name, f32 value);

/* Uniform Light */
void GLSetUniformDirLight(Object *obj, DirLight *value);
void GLSetUniformSpotLight(Object *obj, SpotLight *value);
void GLSetUniformPointLight(Object *obj, PointLight *value);

typedef struct {
  f32 linear;
  f32 quadratic;
} AttenuationCoeffs;
AttenuationCoeffs GetAttenuationCoeffs(i32 distance);

#if 1
#  define GL(call)                                                                                        \
    do {                                                                                                  \
      call;                                                                                               \
      GLenum error = glGetError();                                                                        \
      if (error != GL_NO_ERROR) {                                                                         \
        LogError("OpenGL Error at %s:%d in %s() - Error: 0x%x", __FILE__, __LINE__, __FUNCTION__, error); \
                                                                                                          \
        /* Clear any additional errors that might be queued */                                            \
        while (glGetError() != GL_NO_ERROR) {}                                                            \
      }                                                                                                   \
    } while (0)
#else
#  define GL(call) call
#endif
