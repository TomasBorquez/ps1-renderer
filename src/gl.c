#include "gl.h"

#include <GL/glew.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>

typedef struct {
  String source;
  Arena arena;
} ShaderSource;

static ShaderSource readShader(String *sourcePath) {
  File stats;
  errno_t err = FileStats(sourcePath, &stats);
  if (err != SUCCESS) {
    LogError("FileStats: failed %d, for file %s", err, sourcePath->data);
    abort();
  }

  ShaderSource result = {0};
  result.arena = ArenaInit(stats.size * 1.5);
  system(F(&result.arena, "dos2unix %s", sourcePath->data).data);

  err = FileRead(&result.arena, sourcePath, &result.source);
  if (err != SUCCESS) {
    LogError("FileRead: failed %d, for file %s", err, sourcePath->data);
    abort();
  }

  return result;
}

static void checkCompileErrors(GLuint shader, String *type) {
  i32 success;
  char infoLog[1024];
  if (StrEqual(type, &S("PROGRAM"))) {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (success) {
      return;
    }

    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
    LogError("%s linking failed:\n %s", type->data, infoLog);
    abort();
  }

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (success) {
    return;
  }

  glGetShaderInfoLog(shader, 1024, NULL, infoLog);
  LogError("%s Shader compilation failed:\n %s", type->data, infoLog);
  abort();
}

// TODO: Add geometry shader, NULL if not wanted
// NOTE: This compiles and links the Shaders
u32 ShaderCreate(String vertexShaderPath, String fragmentShaderPath) {
  ShaderSource vertexShaderSource = readShader(&vertexShaderPath);
  defer {
    ArenaFree(&vertexShaderSource.arena);
  };

  ShaderSource fragmentShaderSource = readShader(&fragmentShaderPath);
  defer {
    ArenaFree(&fragmentShaderSource.arena);
  };

  // NOTE: Compile Vertex Shader
  u32 vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  defer {
    glDeleteShader(vertexShaderID);
  };

  glShaderSource(vertexShaderID, 1, (const char *const *)&vertexShaderSource.source.data, NULL);
  glCompileShader(vertexShaderID);
  checkCompileErrors(vertexShaderID, &S("VERTEX"));

  // NOTE: Compile Fragment Shader
  u32 fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  defer {
    glDeleteShader(fragmentShaderID);
  };

  glShaderSource(fragmentShaderID, 1, (const char *const *)&fragmentShaderSource.source.data, NULL);
  glCompileShader(fragmentShaderID);
  checkCompileErrors(fragmentShaderID, &S("FRAGMENT"));

  // NOTE: Link Fragment Shader
  u32 shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShaderID);
  glAttachShader(shaderProgram, fragmentShaderID);
  glLinkProgram(shaderProgram);
  checkCompileErrors(shaderProgram, &S("PROGRAM"));

  return shaderProgram;
}

void ShaderUse(u32 id) {
  glUseProgram(id);
}

void ShaderSetMat4(u32 id, const char *name, mat4 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, value[0]);
}

void ShaderSetVecF4(u32 id, const char *name, vec4 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform4f(uniformLocation, value[0], value[1], value[2], value[3]);
}

void ShaderSetVecF3(u32 id, const char *name, vec3 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform3f(uniformLocation, value[0], value[1], value[2]);
}

void ShaderSetVecF2(u32 id, const char *name, vec2 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform2f(uniformLocation, value[0], value[1]);
}

void ShaderSetB(u32 id, const char *name, bool value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform1i(uniformLocation, value);
}

void ShaderSetI(u32 id, const char *name, i32 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform1i(uniformLocation, value);
}

void ShaderSetF(u32 id, const char *name, f32 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform1f(uniformLocation, value);
}

u32 ShaderCreateTexture(char *texturePath) {
  u32 texture;
  SDL_Surface *imgTexture = IMG_Load(texturePath);
  assert(imgTexture != NULL && "Texture image failed to load");

  // NOTE: OpenGL has different y coords
  SDL_FlipSurface(imgTexture, SDL_FLIP_VERTICAL);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgTexture->w, imgTexture->h, 0, GL_RGB, GL_UNSIGNED_BYTE, imgTexture->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);

  SDL_DestroySurface(imgTexture);
  return texture;
}
