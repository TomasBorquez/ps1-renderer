#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>
#include <cglm.h>

#include "base.h"
#include "gl.h"
#include "obj.h"

Object LightObjCreate() {
  Object result = {0};

  result.shaderID = ShaderCreate(S("./src/shaders/light-obj.vert"), S("./src/shaders/light-obj.frag"));
  ShaderUse(result.shaderID);

  f32 vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

    -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

    0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

    -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
  };

  glGenVertexArrays(1, &result.VAO);
  glGenBuffers(1, &result.VBO);

  glBindVertexArray(result.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, result.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), NULL);
  glEnableVertexAttribArray(0);

  // Unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return result;
}

void LightObjUse(Object *obj, mat4 view) {
  ShaderUse(obj->shaderID);
  glBindVertexArray(obj->VAO);
  ShaderSetMat4(obj, "view", view);
}

void LightObjDraw(Object *obj, mat4 model) {
  ShaderSetMat4(obj, "model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void LightObjDestroy(Object *obj) {
  glDeleteVertexArrays(1, &obj->VAO);
  glDeleteBuffers(1, &obj->VBO);
  glDeleteProgram(obj->shaderID);
}
