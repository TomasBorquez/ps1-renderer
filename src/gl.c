#include <GL/glew.h>

#include "gl.h"

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

// TODO: Add geometry shader, NULL if not wanted
// NOTE: This compiles and links the Shaders
u32 CreateShader(String vertexShaderPath, String fragmentShaderPath) {
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

  i32 success;
  glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
    LogError("Vertex Shader compilation failed:\n %s", infoLog);
    abort();
  }

  // NOTE: Compile Fragment Shader
  u32 fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  defer {
    glDeleteShader(fragmentShaderID);
  };

  glShaderSource(fragmentShaderID, 1, (const char *const *)&fragmentShaderSource.source.data, NULL);
  glCompileShader(fragmentShaderID);

  glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
    LogError("Frament Shader compilation failed, %s", infoLog);
    abort();
  }

  // NOTE: Link Fragment Shader
  u32 shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShaderID);
  glAttachShader(shaderProgram, fragmentShaderID);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    LogError("Shader Program linking failed, %s", infoLog);
    abort();
  }

  return shaderProgram;
}

void UseShader(u32 id) {
  glUseProgram(id);
}

void SetMat4Shader(u32 id, const char *name, mat4 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, value[0]);
}

void SetVecF4Shader(u32 id, const char *name, vec4 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform4f(uniformLocation, value[0], value[1], value[2], value[3]);
}

void SetVecF3Shader(u32 id, const char *name, vec3 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform3f(uniformLocation, value[0], value[1], value[2]);
}

void SetVecF2Shader(u32 id, const char *name, vec2 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform2f(uniformLocation, value[0], value[1]);
}

void SetBShader(u32 id, const char *name, bool value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform1i(uniformLocation, value);
}

void SetIShader(u32 id, const char *name, i32 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform1i(uniformLocation, value);
}

void SetFShader(u32 id, const char *name, f32 value) {
  i32 uniformLocation = glGetUniformLocation(id, name);
  assert(uniformLocation != -1 && "UniformLocation does not exist");
  glUniform1f(uniformLocation, value);
}
