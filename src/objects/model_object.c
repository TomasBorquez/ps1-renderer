#include <stddef.h>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>
#include <cglm.h>

#include "base.h"
#include "gl.h"
#include "obj.h"

Object ModelObjCreate(char *path, char *directory) {
  Object result = {0};
  result.shaderID = ShaderCreate(S("./src/shaders/model-obj.vert"), S("./src/shaders/model-obj.frag"));
  ShaderUse(result.shaderID);
  result.model = LoadModel(path, directory);
  return result;
}

void ModelObjUse(Object *obj, mat4 view) {
  ShaderUse(obj->shaderID);
  ShaderSetMat4(obj, "view", view);
}

void ModelObjDraw(Object *obj, mat4 modelMat) {
  ShaderSetMat4(obj, "model", modelMat);

  Model model = obj->model;
  for (i32 i = 0; i < model.meshes.length; i++) {
    Mesh *currMesh = VecAt(model.meshes, i);
    MeshDraw(currMesh, obj);
  }
}

void ModelObjDestroy(Object *obj) {
  glDeleteProgram(obj->shaderID);

  Model model = obj->model;
  for (i32 i = 0; i < model.meshes.length; i++) {
    Mesh *currMesh = VecAt(model.meshes, i);
    glDeleteVertexArrays(1, &currMesh->VAO);
    glDeleteBuffers(1, &currMesh->VBO);
    glDeleteBuffers(1, &currMesh->EBO);
  }
}
