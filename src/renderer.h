#pragma once

#include <SDL3/SDL.h>
#include <cglm.h>

#include "camera.h"
#include "base.h"
#include "ui.h"

typedef struct {
  u8 r;
  u8 g;
  u8 b;
  u8 a;
} Color;

typedef struct {
  // SDL
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_GLContext glContext;
  SDL_Event e;

  // General
  i32 width;
  i32 height;
  ImVec4 clearColor;

  // Camera
  Camera camera;

  // Temp Frame Buffers
  char textBuffer[600];

  // FPS
  size_t frameCount;
  u32 lastFPSUpdate;
  i32 FPS;
  f64 deltaTime;
  u64 performanceFrequency;
  u64 lastFrame;

  // Flags
  bool isNight;
  bool quit;
  bool keys[SDL_SCANCODE_COUNT];
} Renderer;
extern Renderer renderer;

void InitRenderer(i32 width, i32 height);

void DestroyRenderer();
void RendererQuit(errno_t code);

void EventPoll();
void HandleInput();
void ClearScreen(Color color);

void BeginDrawing();
void EndDrawing();

#define BLACK (Color){10, 10, 10, 255}
#define WHITE (Color){240, 240, 240, 255}
#define GREEN (Color){0, 255, 0, 255}
#define RED (Color){255, 0, 0, 255}
#define BLUE (Color){64, 128, 255, 255}
#define YELLOW (Color){255, 200, 0, 255}
