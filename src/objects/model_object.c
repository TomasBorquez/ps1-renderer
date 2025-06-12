#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>
#include <cglm.h>
#include <stddef.h>

#include "gl.h"

Object ModelObjCreate(char *path, char *directory) {
  Object result = {0};
  result.shaderID = GLCreateShader(S("./src/shaders/model-obj.vert"), S("./src/shaders/model-obj.frag"));
  GLShaderUse(result.shaderID);
  result.model = LoadModel(path, directory);
  return result;
}

void ModelObjUse(Object *obj) {
  GLShaderUse(obj->shaderID);
}

void ModelObjDraw(Object *obj, mat4 modelMat) {
  GLSetUniformMat4(obj, "model", modelMat);

  Model model = obj->model;
  for (size_t i = 0; i < model.meshes.length; i++) {
    Mesh *currMesh = VecAtPtr(model.meshes, i);
    MeshDraw(currMesh, obj);
  }
}

void ModelObjDestroy(Object *obj) {
  GL(glDeleteProgram(obj->shaderID));

  Model model = obj->model;
  for (size_t i = 0; i < model.meshes.length; i++) {
    Mesh *currMesh = VecAtPtr(model.meshes, i);
    GL(glDeleteVertexArrays(1, &currMesh->VAO));
    GL(glDeleteBuffers(1, &currMesh->VBO));
    GL(glDeleteBuffers(1, &currMesh->EBO));
  }
}
