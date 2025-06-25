#pragma once
#include <cglm.h>

#include "base.h"

typedef struct {
  vec3 position;
  vec3 normal;
  vec2 texCoords;
} Vertex;

typedef struct {
  u32 id;
  char *type;
  char *path;
} Texture;

VEC_TYPE(VectorVertex, Vertex);
VEC_TYPE(VectorU32, u32);
VEC_TYPE(VectorTexture, Texture);

typedef struct {
  VectorVertex vertices;
  VectorU32 indices;
  VectorTexture textures;
  u32 VAO;
  u32 VBO;
  u32 EBO;
} Mesh;

VEC_TYPE(VectorMesh, Mesh);

typedef struct {
  VectorTexture texturesLoaded;
  VectorMesh meshes;
  char *directory;
  const struct aiScene *scene;
} Model;

typedef struct {
  u32 id;
  String vertexPath;
  String fragmentPath;
} Shader;

typedef struct {
  u32 VAO;
  u32 VBO;
  u32 EBO;
  mat4 modelMat;
  Model model;
  Shader shader;
} Object;

Model LoadModel(String path);
void MeshDraw(Mesh *mesh, Object *obj);

typedef enum {
  x,
  y,
  z,
} Axis;
void ObjRotate(Object *obj, f32 angle, Axis axis);

void ObjTranslate(Object *obj, f32 x, f32 y, f32 z);

void ObjScale(Object *obj, f32 x, f32 y, f32 z);

void ObjModelMatReset(Object *obj);
