#define BASE_IMPLEMENTATION
#include "base.h"

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <math.h>

#include "gl.h"
#include "model.h"
// #include "renderer.h"

#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 800
#define FPS_INTERVAL 1

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_GLContext glContext;
  bool quit;
  SDL_Event e;

  // OpenGL
  u32 shaderProgram;
  u32 VAO;
  u32 VBO;

  // Temp Frame Buffers
  char textBuffer[600];

  // FPS
  size_t frameCount;
  u32 lastFPSUpdate;
  i32 FPS;
} Renderer;

Renderer renderer = {0};

void RendererQuit(errno_t code) {
  if (renderer.shaderProgram != 0) {
    glDeleteProgram(renderer.shaderProgram);
  }

  if (renderer.VAO != 0) {
    glDeleteVertexArrays(1, &renderer.VAO);
  }

  if (renderer.VBO != 0) {
    glDeleteBuffers(1, &renderer.VBO);
  }

  if (renderer.renderer != NULL) {
    SDL_DestroyRenderer(renderer.renderer);
  }

  if (renderer.glContext != NULL) {
    SDL_GL_DestroyContext(renderer.glContext);
  }

  if (renderer.window != NULL) {
    SDL_DestroyWindow(renderer.window);
  }

  SDL_Quit();

  if (code != -1) {
    exit(code);
  }
}

void CheckOpenGLError(const char *stmt, const char *fname, int line) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    LogError("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
    RendererQuit(-1);
  }
}

void InitOpenGL() {
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    LogError("GLEW initialization failed: %s\n", glewGetErrorString(err));
    RendererQuit(1);
  }

  LogInfo("OpenGL Version: %s", glGetString(GL_VERSION));
  LogInfo("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void InitRenderer(i32 width, i32 height) {
  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    LogError("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    RendererQuit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  renderer.window = SDL_CreateWindow("OpenGL Renderer", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!renderer.window) {
    LogError("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    RendererQuit(1);
  }

  renderer.glContext = SDL_GL_CreateContext(renderer.window);
  if (!renderer.glContext) {
    LogError("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
    RendererQuit(1);
  }

  InitOpenGL();

  renderer.lastFPSUpdate = SDL_GetTicks();
}

void EventPoll() {
  while (SDL_PollEvent(&renderer.e) != 0) {
    if (renderer.e.type == SDL_EVENT_QUIT) {
      renderer.quit = true;
    }
    if (renderer.e.type == SDL_EVENT_KEY_DOWN) {
      if (renderer.e.key.key == SDLK_ESCAPE) {
        renderer.quit = true;
      }
    }
  }
}

void DestroyRenderer() {
  RendererQuit(0);
}

void ClearScreen(Color color) {
  glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void BeginDrawing() {
  EventPoll();
}

void EndDrawing() {
  SDL_GL_SwapWindow(renderer.window);

  renderer.frameCount++;
  u32 currentTime = SDL_GetTicks();
  if (currentTime - renderer.lastFPSUpdate >= FPS_INTERVAL * 1000) {
    renderer.FPS = renderer.frameCount / FPS_INTERVAL;
    renderer.frameCount = 0;
    renderer.lastFPSUpdate = currentTime;
    LogInfo("FPS: %d", renderer.FPS);
  }
}

int main() {
  InitRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);

  u32 shaderProgram = CreateShaders(S("./src/triangle.vert"), S("./src/triangle.frag"));

  // NOTE : VAO and VBO
  Model model = InitModel(S("./resources/diablo_pose.obj"));

  VectorF32 vertices = {0};
  for (i32 i = 0; i < model.faces.length / 3; i++) {
    VecF3 a = GetVertModel(&model, i, 0);
    VecPush(vertices, a.x);
    VecPush(vertices, a.y);
    VecPush(vertices, a.z);

    VecF3 b = GetVertModel(&model, i, 1);
    VecPush(vertices, b.x);
    VecPush(vertices, b.y);
    VecPush(vertices, b.z);

    VecF3 c = GetVertModel(&model, i, 2);
    VecPush(vertices, c.x);
    VecPush(vertices, c.y);
    VecPush(vertices, c.z);
  }

  u32 VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // NOTE: Bind VAO first since we need configuration inside VAO
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.length * sizeof(f32), vertices.data, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), NULL);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!renderer.quit) {
    BeginDrawing();
    {
      ClearScreen(BLACK);

      glUseProgram(shaderProgram);

      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, vertices.length / 3);
      // glDrawArrays(GL_TRIANGLES, 0, 3);
      glBindVertexArray(0);
    }
    EndDrawing();
  }

  DestroyRenderer();
  return 0;
}
