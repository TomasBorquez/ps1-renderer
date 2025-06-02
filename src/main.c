#include <math.h>

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>

#include <cglm.h>

#define BASE_IMPLEMENTATION
#include "base.h"

#include "gl.h"
#include "renderer.h"

#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 800
#define FPS_INTERVAL 1

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_GLContext glContext;
  bool quit;
  SDL_Event e;
  bool keys[SDL_SCANCODE_COUNT];

  // OpenGL
  u32 shaderProgram;
  u32 VAO;
  u32 VBO;

  // Camera
  vec3 cameraPosition;
  vec3 cameraFront;
  vec3 cameraUp;
  f32 yaw;
  f32 pitch;

  // Temp Frame Buffers
  char textBuffer[600];

  // FPS
  size_t frameCount;
  u32 lastFPSUpdate;
  i32 FPS;

  f32 deltaTime;
  u64 performanceFrequency;
  u64 lastFrame;
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

  SDL_SetWindowRelativeMouseMode(renderer.window, true);

  renderer.glContext = SDL_GL_CreateContext(renderer.window);
  if (!renderer.glContext) {
    LogError("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
    RendererQuit(1);
  }

  InitOpenGL();

  renderer.performanceFrequency = SDL_GetPerformanceFrequency();
  renderer.lastFPSUpdate = SDL_GetTicks();
}

void MoveCamera(SDL_MouseMotionEvent motion) {
  float xoffset = motion.xrel;
  float yoffset = -motion.yrel;

  const float sensitivity = 0.03f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;
  renderer.yaw += xoffset;
  renderer.pitch += yoffset;
  if (renderer.pitch > 89.0f) renderer.pitch = 89.0f;
  if (renderer.pitch < -89.0f) renderer.pitch = -89.0f;

  renderer.cameraFront[0] = cos(glm_rad(renderer.yaw)) * cos(glm_rad(renderer.pitch));
  renderer.cameraFront[1] = sin(glm_rad(renderer.pitch));
  renderer.cameraFront[2] = sin(glm_rad(renderer.yaw)) * cos(glm_rad(renderer.pitch));
  glm_normalize(renderer.cameraFront);
}

void EventPoll() {
  while (SDL_PollEvent(&renderer.e) != 0) {
    if (renderer.e.type == SDL_EVENT_QUIT) {
      renderer.quit = true;
    }

    if (renderer.e.type == SDL_EVENT_MOUSE_MOTION) {
      MoveCamera(renderer.e.motion);
    }

    if (renderer.e.key.key == SDLK_ESCAPE) {
      renderer.quit = true;
    }

    if (renderer.e.type == SDL_EVENT_KEY_DOWN) {
      if (renderer.e.key.key == SDLK_ESCAPE) {
        renderer.quit = true;
      }
      renderer.keys[renderer.e.key.scancode] = true;
    }

    if (renderer.e.type == SDL_EVENT_KEY_UP) {
      renderer.keys[renderer.e.key.scancode] = false;
    }
  }
}

void HandleInput() {
  f32 cameraSpeed = renderer.deltaTime * 5.f;

  if (renderer.keys[SDL_SCANCODE_A]) {
    vec3 d;
    glm_cross(renderer.cameraFront, renderer.cameraUp, d);
    glm_normalize(d);
    glm_vec3_muladds(d, -cameraSpeed, renderer.cameraPosition);
  }

  if (renderer.keys[SDL_SCANCODE_D]) {
    vec3 d;
    glm_cross(renderer.cameraFront, renderer.cameraUp, d);
    glm_normalize(d);
    glm_vec3_muladds(d, cameraSpeed, renderer.cameraPosition);
  }

  if (renderer.keys[SDL_SCANCODE_W]) {
    glm_vec3_muladds(renderer.cameraFront, cameraSpeed, renderer.cameraPosition);
  }

  if (renderer.keys[SDL_SCANCODE_S]) {
    glm_vec3_muladds(renderer.cameraFront, -cameraSpeed, renderer.cameraPosition);
  }

  if (renderer.keys[SDL_SCANCODE_J]) {
    glm_vec3_muladds(renderer.cameraFront, -cameraSpeed, renderer.cameraPosition);
  }

  if (renderer.keys[SDL_SCANCODE_P]) {
    glm_vec3_muladds(renderer.cameraFront, -cameraSpeed, renderer.cameraPosition);
  }

  // FPS like, .y stays 0
  renderer.cameraPosition[1] = 0;
}

void DestroyRenderer() {
  RendererQuit(0);
}

void ClearScreen(Color color) {
  glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BeginDrawing() {
  EventPoll();
  HandleInput();
  u64 currTime = SDL_GetPerformanceCounter();
  renderer.deltaTime = (f32)(currTime - renderer.lastFrame) / renderer.performanceFrequency;
  renderer.lastFrame = currTime;
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

  u32 shaderProgram = CreateShader(S("./src/triangle.vert"), S("./src/triangle.frag"));

  // NOTE : VAO and VBO
  float vertices[] = {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

                      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

                      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

                      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

                      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

  u32 VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // NOTE: Bind VAO first since we need configuration inside VAO
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), NULL);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)(3 * sizeof(f32)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  u32 texture;
  SDL_Surface *imgTexture = IMG_Load("./resources/wall.jpg");
  assert(imgTexture != NULL && "Wall texture image should never be null");
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

  UseShader(shaderProgram);

  vec3 cubePositions[] = {
    {0.0f,  0.0f,  0.0f  },
    {2.0f,  5.0f,  -15.0f},
    {-1.5f, -2.2f, -2.5f },
    {-3.8f, -2.0f, -12.3f},
    {2.4f,  -0.4f, -3.5f },
    {-1.7f, 3.0f,  -7.5f },
    {1.3f,  -2.0f, -2.5f },
    {1.5f,  2.0f,  -2.5f },
    {1.5f,  0.2f,  -1.5f },
    {-1.3f, 1.0f,  -1.5f }
  };

  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(50.0f), 800.0f / 800.0f, 0.1f, 100.0f, projection);
  SetMat4Shader(shaderProgram, "projection", projection);

  renderer.cameraPosition[0] = 0.0f;
  renderer.cameraPosition[1] = 0.0f;
  renderer.cameraPosition[2] = 3.0f;

  renderer.cameraFront[0] = 0.0f;
  renderer.cameraFront[1] = 0.0f;
  renderer.cameraFront[2] = -1.0f;

  renderer.cameraUp[0] = 0.0f;
  renderer.cameraUp[1] = 1.0f;
  renderer.cameraUp[2] = 0.0f;

  renderer.yaw = -90.0f;

  vec3 cameraTarget = {0.0f, 0.0f, 0.0f};

  // WARNING: The name direction vector is not the best chosen name,
  // since it is actually pointing in the reverse direction of what it is targeting.
  vec3 cameraDirection;
  glm_vec3_sub(renderer.cameraPosition, cameraTarget, cameraDirection);
  glm_normalize(cameraDirection);

  vec3 cameraRight;
  glm_cross(renderer.cameraUp, cameraDirection, cameraRight);
  glm_normalize(cameraRight);

  vec3 cameraUp;
  glm_cross(cameraDirection, cameraRight, cameraUp);
  while (!renderer.quit) {
    BeginDrawing();
    {
      ClearScreen(BLACK);

      UseShader(shaderProgram);
      glBindVertexArray(VAO);

      mat4 view = GLM_MAT4_IDENTITY_INIT;
      vec3 center;
      glm_vec3_add(renderer.cameraPosition, renderer.cameraFront, center);
      glm_lookat(renderer.cameraPosition, center, renderer.cameraUp, view);

      SetMat4Shader(shaderProgram, "view", view);
      for (size_t i = 0; i < sizeof(cubePositions) / sizeof(vec3); i++) {
        vec3 *currCube = &cubePositions[i];
        mat4 model = GLM_MAT4_IDENTITY_INIT;

        f32 currTime = (f32)SDL_GetTicks() / 1000;
        glm_translate(model, *currCube);
        glm_rotate(model, currTime + i, (vec3){1.0f, 0.3f, 0.5f});
        SetMat4Shader(shaderProgram, "model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }
    EndDrawing();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  DestroyRenderer();
  return 0;
}
