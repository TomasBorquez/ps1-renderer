#define BASE_IMPLEMENTATION
#include "base.h"
#include "renderer.h"

#include "scenes/school.c"

i32 main() {
  InitRenderer(1200, 900);
  SetTargetFPS(480);

  SceneCreate();
  while (!renderer.quit) {
    BeginDrawing();
    {
      ClearScreen(BLACK);
      SceneUpdate();
    }
    EndDrawing();
  }

  SceneDestroy();
}
