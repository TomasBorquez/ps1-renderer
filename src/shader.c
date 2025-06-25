#include "shader.h"

#include "gl.h"
#define MAX_PATH_BUFFER 1024

typedef struct {
  String source;
  Arena *arena;
} ShaderSource;

static const String basePath = S("./src/shaders");

static ShaderSource readShader(String sourcePath) {
  Assert(basePath.length + sourcePath.length <= MAX_PATH_BUFFER, "readShader: failed, path is too long, full path %s/%s", basePath.data, sourcePath.data);
  char pathBuffer[MAX_PATH_BUFFER];
  snprintf(pathBuffer, MAX_PATH_BUFFER, "%s/%s", basePath.data, sourcePath.data);
  sourcePath = s(pathBuffer);

  LogInfo("Processing shader: %s", sourcePath.data);

  File stats;
  errno_t err = FileStats(sourcePath, &stats);
  if (err != SUCCESS) {
    LogError("FileStats: failed %d, for file %s", err, sourcePath.data);
    abort();
  }

  ShaderSource result = {0};
  result.arena = ArenaCreate(stats.size * 4);

  String fileSource;
  err = FileRead(result.arena, sourcePath, &fileSource);
  if (err != SUCCESS) {
    LogError("FileRead: failed %d, for file %s", err, sourcePath.data);
    abort();
  }

  StringVector splitFile = StrSplitNewLine(result.arena, fileSource);
  StringBuilder builder = StringBuilderReserve(result.arena, fileSource.length * 2);
  VecForEach(splitFile, line) {
    bool isInclude = StrStartsWith(*line, S("#include"));
    if (!isInclude) {
      StringBuilderAppend(result.arena, &builder, line);
      StringBuilderAppend(result.arena, &builder, &S("\n"));
      continue;
    }

    size_t start = 0;
    size_t end = 0;

    for (size_t i = 0; i < line->length; i++) {
      char currChar = line->data[i];
      if (currChar != '"') {
        continue;
      }

      if (start == 0) {
        start = i + 1;
        continue;
      }

      if (end == 0) {
        end = i;
        break;
      }
    }

    Assert(start > 0 && end > 0,
           "CreateShader: failed, #include usage is incorrect, usage:\n"
           "#include \"light.glsl\"\n\n"
           "Instead it was:\n"
           "%s\n\n"
           "File: %s\n",
           line->data,
           sourcePath.data);

    String path = StrSlice(result.arena, *line, start, end);
    ShaderSource include = readShader(path);
    StringBuilderAppend(result.arena, &builder, &include.source);

    ArenaFree(include.arena);
  }

  result.source = builder.buffer;

  return result;
}

static errno_t checkCompileErrors(GLuint shader, String type, String *filePath) {
  i32 success;
  char infoLog[1024];
  if (StrEq(type, S("PROGRAM"))) {
    GL(glGetProgramiv(shader, GL_LINK_STATUS, &success));
    if (success) {
      return SUCCESS;
    }

    GL(glGetProgramInfoLog(shader, 1024, NULL, infoLog));
    LogError("%s linking failed:\n %s", type.data, infoLog);
    return 1;
  }

  GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
  if (success) {
    return SUCCESS;
  }

  GL(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
  LogError("%s Shader compilation failed for file %s:\n %s", type.data, filePath->data, infoLog);
  return 1;
}

void GLCreateShader(Object *obj, String vertexShaderPath, String fragmentShaderPath) {
  ShaderSource vertexShaderSource = readShader(vertexShaderPath);
  ShaderSource fragmentShaderSource = readShader(fragmentShaderPath);
  errno_t err;

  // Flags
  bool isFirstTime = obj->shader.id == 0;

  // Compile Vertex Shader
  u32 vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GL(glShaderSource(vertexShaderID, 1, (const char *const *)&vertexShaderSource.source.data, NULL));
  GL(glCompileShader(vertexShaderID));
  err = checkCompileErrors(vertexShaderID, S("VERTEX"), &vertexShaderPath);
  if (err != SUCCESS) {
    Assert(!isFirstTime, "");
    return;
  }

  // Compile Fragment Shader
  u32 fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  GL(glShaderSource(fragmentShaderID, 1, (const char *const *)&fragmentShaderSource.source.data, NULL));
  GL(glCompileShader(fragmentShaderID));
  err = checkCompileErrors(fragmentShaderID, S("FRAGMENT"), &fragmentShaderPath);
  if (err != SUCCESS) {
    Assert(!isFirstTime, "");
    return;
  }

  // Link Fragment Shader
  u32 shaderProgram = glCreateProgram();
  GL(glAttachShader(shaderProgram, vertexShaderID));
  GL(glAttachShader(shaderProgram, fragmentShaderID));
  GL(glLinkProgram(shaderProgram));
  err = checkCompileErrors(shaderProgram, S("PROGRAM"), NULL);
  if (err != SUCCESS) {
    Assert(!isFirstTime, "");
    return;
  }

  // Clear state
  ArenaFree(vertexShaderSource.arena);
  ArenaFree(fragmentShaderSource.arena);
  GL(glDeleteShader(vertexShaderID));
  GL(glDeleteShader(fragmentShaderID));

  // Assign
  if (shaderProgram != 0) {
    obj->shader.id = shaderProgram;
    obj->shader.vertexPath = vertexShaderPath;
    obj->shader.fragmentPath = fragmentShaderPath;
  }
}
