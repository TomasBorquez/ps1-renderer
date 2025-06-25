#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>
#include <cglm.h>

#include "gl.h"
#include "shader.h"

Object LightObjCreate() {
  Object result = {.modelMat = GLM_MAT4_IDENTITY_INIT};

  GLCreateShader(&result, S("light-obj.vert"), S("light-obj.frag"));
  GLShaderUse(result.shader.id);

  f32 vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
    0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
  };

  GL(glGenVertexArrays(1, &result.VAO));
  GL(glGenBuffers(1, &result.VBO));

  GLBindVAO(result.VAO);
  GLBindVBO(result.VBO);
  GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

  GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), NULL));
  GL(glEnableVertexAttribArray(0));

  // Unbind
  GLUnbindVBO();
  GLUnbindVAO();

  return result;
}

void LightObjUse(Object *obj) {
  GLShaderUse(obj->shader.id);
  GLBindVAO(obj->VAO);
}

void LightObjDraw(Object *obj) {
  GLSetUniformMat4(obj, "model", obj->modelMat);
  GL(glDrawArrays(GL_TRIANGLES, 0, 36));
}

void LightObjDestroy(Object *obj) {
  GL(glDeleteVertexArrays(1, &obj->VAO));
  GL(glDeleteBuffers(1, &obj->VBO));
  GL(glDeleteProgram(obj->shader.id));
}
