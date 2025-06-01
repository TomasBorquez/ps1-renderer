#pragma once
#include "renderer.h"

typedef struct {
  VectorVecF3 vertices;
  VectorVecF3 vertexNormals;
  VectorVecF2 vertexTextures;
  VectorVecI3 faces;
} Model;

Model InitModel(String path);
VecF3 GetVertModel(Model *model, size_t faceIndex, size_t vertIndex);
VecF2 GetVertTextModel(Model *model, size_t faceIndex, size_t vertTextIndex);
VecF3 GetVertNormModel(Model *model, size_t faceIndex, size_t vertNormIndex);
