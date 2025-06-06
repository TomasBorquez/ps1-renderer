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
} Model;

typedef struct {
  u32 shaderID;
  u32 VAO;
  u32 VBO;
  u32 EBO;
  u32 textures[16];
  Model model;
} Object;

Model LoadModel(char *path, char *directory);
void MeshDraw(Mesh *mesh, Object *obj);
