#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>
#include <cglm.h>
#include <stddef.h>

#include "gl.h"
#include "shader.h"

Object ModelObjCreate(String path) {
  Object result = {.modelMat = GLM_MAT4_IDENTITY_INIT};
  GLCreateShader(&result, S("model-obj.vert"), S("model-obj.frag"));
  GLShaderUse(result.shader.id);
  GLSetUniformF(&result, "material.shininess", 32.0f);

  result.model = LoadModel(path);
  return result;
}

void ModelObjUse(Object *obj) {
  GLShaderUse(obj->shader.id);
}

void ModelObjDraw(Object *obj) {
  GLSetUniformMat4(obj, "model", obj->modelMat);

  Model model = obj->model;
  for (size_t i = 0; i < model.meshes.length; i++) {
    Mesh *currMesh = VecAtPtr(model.meshes, i);
    MeshDraw(currMesh, obj);
  }
}

void ModelObjDestroy(Object *obj) {
  GL(glDeleteProgram(obj->shader.id));

  Model model = obj->model;
  for (size_t i = 0; i < model.meshes.length; i++) {
    Mesh *currMesh = VecAtPtr(model.meshes, i);
    GL(glDeleteVertexArrays(1, &currMesh->VAO));
    GL(glDeleteBuffers(1, &currMesh->VBO));
    GL(glDeleteBuffers(1, &currMesh->EBO));
  }
}
