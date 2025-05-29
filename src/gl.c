#include "gl.h"
#include <GL/glew.h>

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
  result.arena = ArenaInit(stats.size);
  err = FileRead(&result.arena, sourcePath, &result.source);
  if (err != SUCCESS) {
    LogError("FileRead: failed %d, for file %s", err, sourcePath->data);
    abort();
  }

  return result;
}

// TODO: Add geometry shader, NULL if not wanted
// NOTE: This compiles and links the Shaders
u32 CreateShaders(String vertexShaderPath, String fragmentShaderPath) {
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
