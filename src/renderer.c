#include "renderer.h"
#include "ui.h"
#include "gl.h"

#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_image/SDL_image.h>

#define FPS_INTERVAL 1
Renderer renderer = {0};
void RendererQuit(errno_t code) {
  DestroyImGui();

  if (GLContext.shaderID != 0) {
    GL(glDeleteProgram(GLContext.shaderID));
  }

  if (GLContext.VAO != 0) {
    GL(glDeleteVertexArrays(1, &GLContext.VAO));
  }

  if (GLContext.VBO != 0) {
    GL(glDeleteBuffers(1, &GLContext.VBO));
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

static void InitSDL(i32 width, i32 height) {
  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    LogError("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    RendererQuit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  renderer.width = width;
  renderer.height = height;
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
}

static void InitOpenGL() {
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    LogError("InitOpenGL: Failed, GLEW initialization failed: %s\n", glewGetErrorString(err));
    RendererQuit(1);
  }

  LogInfo("OpenGL Version: %s", glGetString(GL_VERSION));
  LogInfo("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

  GL(glViewport(0, 0, renderer.width, renderer.height));

  // Depth
  GL(glEnable(GL_DEPTH_TEST));
  GL(glDepthFunc(GL_LESS));

  // Blend
  GL(glEnable(GL_BLEND));
  GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  // Debugging
  GL(glEnable(GL_DEBUG_OUTPUT));
  GL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));

  // Face Culling
  // TODO: First implement WBOIT, Optimization
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
}

void InitRenderer(i32 width, i32 height) {
  InitSDL(width, height);
  InitOpenGL();
  InitImGui();

  renderer.clearColor = (ImVec4){0.0f, 0.0f, 0.0f, 1.0f};
  renderer.performanceFrequency = SDL_GetPerformanceFrequency();
  renderer.lastFPSUpdate = SDL_GetTicks();
}

void EventPoll() {
  while (SDL_PollEvent(&renderer.e) != 0) {
    ProcessUIEvent(&renderer.e);

    if (renderer.e.type == SDL_EVENT_QUIT) {
      renderer.quit = true;
    }

    if (renderer.e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && renderer.e.window.windowID == SDL_GetWindowID(renderer.window)) {
      renderer.quit = true;
    }

    if (renderer.e.type == SDL_EVENT_MOUSE_MOTION && !IsUIInputModeActive()) {
      ImGuiIO *io = igGetIO();
      if (!io->WantCaptureMouse) {
        SDL_MouseMotionEvent motion = renderer.e.motion;
        CameraProcessMouseMovement(&renderer.camera, motion.xrel, -motion.yrel);
      }
    }

    if (renderer.e.key.key == SDLK_ESCAPE) {
      renderer.quit = true;
    }

    if (renderer.e.type == SDL_EVENT_KEY_DOWN && !IsUIInputModeActive()) {
      if (renderer.e.key.key == SDLK_ESCAPE) {
        renderer.quit = true;
      }

      renderer.keys[renderer.e.key.scancode] = true;
    }

    if (renderer.e.type == SDL_EVENT_KEY_UP) {
      renderer.keys[renderer.e.key.scancode] = false;
    }
  }

  if (SDL_GetWindowFlags(renderer.window) & SDL_WINDOW_MINIMIZED) {
    SDL_Delay(10);
    return;
  }
}

void HandleInput() {
  if (IsUIInputModeActive()) {
    ImGuiIO *io = igGetIO();
    if (io->WantCaptureKeyboard) {
      return;
    }
  }

  if (renderer.keys[SDL_SCANCODE_W]) {
    CameraProcessKeyboard(&renderer.camera, FORWARD, renderer.deltaTime);
  }

  if (renderer.keys[SDL_SCANCODE_S]) {
    CameraProcessKeyboard(&renderer.camera, BACKWARD, renderer.deltaTime);
  }

  if (renderer.keys[SDL_SCANCODE_A]) {
    CameraProcessKeyboard(&renderer.camera, LEFT, renderer.deltaTime);
  }

  if (renderer.keys[SDL_SCANCODE_D]) {
    CameraProcessKeyboard(&renderer.camera, RIGHT, renderer.deltaTime);
  }

  if (renderer.keys[SDL_SCANCODE_SPACE]) {
    CameraProcessKeyboard(&renderer.camera, UP, renderer.deltaTime);
  }

  if (renderer.keys[SDL_SCANCODE_LCTRL]) {
    CameraProcessKeyboard(&renderer.camera, DOWN, renderer.deltaTime);
  }
}

void DestroyRenderer() {
  RendererQuit(0);
}

void ClearScreen(Color color) {
  if (renderer.clearColor.x != 0.0f || renderer.clearColor.y != 0.0f || renderer.clearColor.z != 0.0f) {
    GL(glClearColor(renderer.clearColor.x, renderer.clearColor.y, renderer.clearColor.z, renderer.clearColor.w));
  } else {
    GL(glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f));
  }
  GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void BeginDrawing() {
  EventPoll();
  HandleInput();
  u64 currTime = SDL_GetPerformanceCounter();
  renderer.deltaTime = (f64)(currTime - renderer.lastFrame) / renderer.performanceFrequency;
  renderer.lastFrame = currTime;
}

void EndDrawing() {
  RenderUI();
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
