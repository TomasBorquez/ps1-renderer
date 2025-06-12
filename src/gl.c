#include "gl.h"

#include <GL/glew.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>

typedef struct {
  String source;
  Arena *arena;
} ShaderSource;
OpenGLContext GLContext = {0};

static ShaderSource readShader(String sourcePath) {
  File stats;
  errno_t err = FileStats(sourcePath, &stats);
  if (err != SUCCESS) {
    LogError("FileStats: failed %d, for file %s", err, sourcePath.data);
    abort();
  }

  ShaderSource result = {0};
  result.arena = ArenaCreate(stats.size * 2);
  system(F(result.arena, "dos2unix %s", sourcePath.data).data);

  err = FileRead(result.arena, sourcePath, &result.source);
  if (err != SUCCESS) {
    LogError("FileRead: failed %d, for file %s", err, sourcePath.data);
    abort();
  }

  return result;
}

static void checkCompileErrors(GLuint shader, String type, String *filePath) {
  i32 success;
  char infoLog[1024];
  if (StrEq(type, S("PROGRAM"))) {
    GL(glGetProgramiv(shader, GL_LINK_STATUS, &success));
    if (success) {
      return;
    }

    GL(glGetProgramInfoLog(shader, 1024, NULL, infoLog));
    LogError("%s linking failed:\n %s", type.data, infoLog);
    abort();
  }

  GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
  if (success) {
    return;
  }

  GL(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
  LogError("%s Shader compilation failed for file %s:\n %s", type.data, filePath->data, infoLog);
  abort();
}

u32 GLCreateShader(String vertexShaderPath, String fragmentShaderPath) {
  ShaderSource vertexShaderSource = readShader(vertexShaderPath);
  ShaderSource fragmentShaderSource = readShader(fragmentShaderPath);

  // Compile Vertex Shader
  u32 vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GL(glShaderSource(vertexShaderID, 1, (const char *const *)&vertexShaderSource.source.data, NULL));
  GL(glCompileShader(vertexShaderID));
  checkCompileErrors(vertexShaderID, S("VERTEX"), &vertexShaderPath);

  // Compile Fragment Shader
  u32 fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  GL(glShaderSource(fragmentShaderID, 1, (const char *const *)&fragmentShaderSource.source.data, NULL));
  GL(glCompileShader(fragmentShaderID));
  checkCompileErrors(fragmentShaderID, S("FRAGMENT"), &fragmentShaderPath);

  // Link Fragment Shader
  u32 shaderProgram = glCreateProgram();
  GL(glAttachShader(shaderProgram, vertexShaderID));
  GL(glAttachShader(shaderProgram, fragmentShaderID));
  GL(glLinkProgram(shaderProgram));
  checkCompileErrors(shaderProgram, S("PROGRAM"), NULL);

  // Clear state
  ArenaFree(vertexShaderSource.arena);
  ArenaFree(fragmentShaderSource.arena);
  GL(glDeleteShader(vertexShaderID));
  GL(glDeleteShader(fragmentShaderID));

  return shaderProgram;
}

void GLShaderUse(u32 id) {
  if (GLContext.shaderID != id) {
    GLContext.shaderID = id;
    GL(glUseProgram(GLContext.shaderID));
  }
}

void GLBindVAO(u32 id) {
  if (GLContext.VAO != id) {
    GLContext.VAO = id;
    GL(glBindVertexArray(GLContext.VAO));
  }
}

void GLBindVBO(u32 id) {
  if (GLContext.VBO != id) {
    GLContext.VBO = id;
    GL(glBindBuffer(GL_ARRAY_BUFFER, GLContext.VBO));
  }
}

void GLBindEBO(u32 id) {
  GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
}

void GLBindUBO(u32 id) {
  if (GLContext.UBO != id) {
    GLContext.UBO = id;
    GL(glBindBuffer(GL_UNIFORM_BUFFER, GLContext.UBO));
  }
}

void GLUnbindShader() {
  GLContext.shaderID = 0;
  GL(glUseProgram(0));
}

void GLUnbindVAO() {
  GLContext.VAO = 0;
  GL(glBindVertexArray(0));
}

void GLUnbindVBO() {
  GLContext.VBO = 0;
  GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void GLUnbindEBO() {
  // GLContext.EBO = 0;
  GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void GLUnbindUBO() {
  GLContext.UBO = 0;
  GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

// TODO: Free uniform data on destroy
i32 GLGetUniformLocation(Object *obj, const char *name) {
  Assert(obj != NULL, "ShaderGetUniformLocation: Shader is not NULL, uniform name: %s", name);
  GLShaderUse(obj->shaderID);
  i32 uniformLocation = glGetUniformLocation(obj->shaderID, name);
  Assert(uniformLocation != -1, "ShaderGetUniformLocation: failed, name: %s does not exist", name);
  return uniformLocation;
}

void GLSetUniformMat4(Object *obj, const char *name, mat4 value) {
  u32 uniformLocation = GLGetUniformLocation(obj, name);
  GL(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, value[0]));
}

void GLSetUniformVecF4(Object *obj, const char *name, vec4 value) {
  u32 uniformLocation = GLGetUniformLocation(obj, name);
  GL(glUniform4f(uniformLocation, value[0], value[1], value[2], value[3]));
}

void GLSetUniformVecF3(Object *obj, const char *name, vec3 value) {
  u32 uniformLocation = GLGetUniformLocation(obj, name);
  GL(glUniform3f(uniformLocation, value[0], value[1], value[2]));
}

void GLSetUniformVecF2(Object *obj, const char *name, vec2 value) {
  u32 uniformLocation = GLGetUniformLocation(obj, name);
  GL(glUniform2f(uniformLocation, value[0], value[1]));
}

void GLSetUniformB(Object *obj, const char *name, bool value) {
  u32 uniformLocation = GLGetUniformLocation(obj, name);
  GL(glUniform1i(uniformLocation, value));
}

void GLSetUniformI(Object *obj, const char *name, i32 value) {
  u32 uniformLocation = GLGetUniformLocation(obj, name);
  GL(glUniform1i(uniformLocation, value));
}

void GLSetUniformF(Object *obj, const char *name, f32 value) {
  u32 uniformLocation = GLGetUniformLocation(obj, name);
  GL(glUniform1f(uniformLocation, value));
}

void GLSetUniformDirLight(Object *obj, DirLight *light) {
  GLSetUniformVecF3(obj, "dirLight.direction", light->direction);

  GLSetUniformVecF3(obj, "dirLight.ambient", light->ambient);
  GLSetUniformVecF3(obj, "dirLight.diffuse", light->diffuse);
  GLSetUniformVecF3(obj, "dirLight.specular", light->specular);

  GLSetUniformB(obj, "dirLight.isActive", true);
}

void GLSetUniformSpotLight(Object *obj, SpotLight *light) {
  GLSetUniformVecF3(obj, "spotLight.position", light->position);
  GLSetUniformVecF3(obj, "spotLight.direction", light->direction);

  GLSetUniformVecF3(obj, "spotLight.ambient", light->ambient);
  GLSetUniformVecF3(obj, "spotLight.diffuse", light->diffuse);
  GLSetUniformVecF3(obj, "spotLight.specular", light->specular);

  GLSetUniformF(obj, "spotLight.cutOff", light->cutOff);
  GLSetUniformF(obj, "spotLight.outerCutOff", light->outerCutOff);

  GLSetUniformF(obj, "spotLight.linear", light->linear);
  GLSetUniformF(obj, "spotLight.quadratic", light->quadratic);

  GLSetUniformB(obj, "spotLight.isActive", true);
}

void GLSetUniformPointLight(Object *obj, PointLight *light) {
  GLSetUniformVecF3(obj, "pointLight.position", light->position);

  GLSetUniformVecF3(obj, "pointLight.ambient", light->ambient);
  GLSetUniformVecF3(obj, "pointLight.specular", light->specular);
  GLSetUniformVecF3(obj, "pointLight.diffuse", light->diffuse);

  GLSetUniformF(obj, "pointLight.linear", light->linear);
  GLSetUniformF(obj, "pointLight.quadratic", light->quadratic);

  GLSetUniformB(obj, "pointLight.isActive", true);
}

u32 GLCreateTexture(char *texturePath) {
  u32 textureID;
  SDL_Surface *imgTexture = IMG_Load(texturePath);
  Assert(imgTexture != NULL, "GLCreateTexture: texture image failed to load for path: %s", texturePath);

  SDL_FlipSurface(imgTexture, SDL_FLIP_VERTICAL);

  GL(glGenTextures(1, &textureID));
  GL(glBindTexture(GL_TEXTURE_2D, textureID));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

  // Auto-detect format based on bytes per pixel
  GLenum format, internalFormat;
  if (imgTexture->format == SDL_PIXELFORMAT_RGBA32 || imgTexture->format == SDL_PIXELFORMAT_ARGB32 || imgTexture->format == SDL_PIXELFORMAT_BGRA32 || imgTexture->format == SDL_PIXELFORMAT_ABGR32) {
    format = GL_RGBA;
    internalFormat = GL_RGBA; // Keep alpha in GPU memory
  } else {
    format = GL_RGB;
    internalFormat = GL_RGB;
  }

  GL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imgTexture->w, imgTexture->h, 0, format, GL_UNSIGNED_BYTE, imgTexture->pixels));
  GL(glGenerateMipmap(GL_TEXTURE_2D));
  SDL_DestroySurface(imgTexture);
  return textureID;
}

void GLCreateUBOs(mat4 projection) {
  GL(glGenBuffers(1, &GLContext.MatricesUBO));

  GL(glBindBuffer(GL_UNIFORM_BUFFER, GLContext.MatricesUBO));
  GL(glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4) * 2, NULL, GL_STATIC_DRAW));
  GL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, GLContext.MatricesUBO));

  GL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), projection));
}

void GLUpdateView(mat4 view) {
  GLBindUBO(GLContext.UBO);
  GL(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), view));
}

AttenuationCoeffs GetAttenuationCoeffs(i32 distance) {
  if (distance <= 7) {
    return (AttenuationCoeffs){0.7f, 1.8f};
  }
  if (distance <= 13) {
    return (AttenuationCoeffs){0.35f, 0.44f};
  }
  if (distance <= 20) {
    return (AttenuationCoeffs){0.22f, 0.20f};
  }
  if (distance <= 32) {
    return (AttenuationCoeffs){0.14f, 0.07f};
  }
  if (distance <= 50) {
    return (AttenuationCoeffs){0.09f, 0.032f};
  }
  if (distance <= 65) {
    return (AttenuationCoeffs){0.07f, 0.017f};
  }
  if (distance <= 100) {
    return (AttenuationCoeffs){0.045f, 0.0075f};
  }
  if (distance <= 160) {
    return (AttenuationCoeffs){0.027f, 0.0028f};
  }
  if (distance <= 200) {
    return (AttenuationCoeffs){0.022f, 0.0019f};
  }
  if (distance <= 325) {
    return (AttenuationCoeffs){0.014f, 0.0007f};
  }
  if (distance <= 600) {
    return (AttenuationCoeffs){0.007f, 0.0002f};
  }

  // > 600
  return (AttenuationCoeffs){0.0014f, 0.000007f};
}
