#include "ui.h"
#include "gl.h"
#include <GL/glew.h>
#include <cimgui_impl.h>

static bool showDebugWindow = true;
static bool uiInputMode = false;
void InitImGui(void) {
  igCreateContext(NULL);
  ImGuiIO *ioptr = igGetIO();
  ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ioptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  // Start without capturing input
  ioptr->WantCaptureKeyboard = false;
  ioptr->WantCaptureMouse = false;

  ImGui_ImplSDL3_InitForOpenGL(renderer.window, renderer.glContext);
  ImGui_ImplOpenGL3_Init("#version 460 core");
  igStyleColorsDark(NULL);

  LogInfo("ImGui initialized successfully");
}

void DestroyImGui(void) {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  igDestroyContext(NULL);
}

void ProcessUIEvent(SDL_Event *event) {
  if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_F3) {
    uiInputMode = !uiInputMode;
    SDL_SetWindowRelativeMouseMode(renderer.window, !uiInputMode);

    if (!uiInputMode) {
      ImGuiIO *io = igGetIO();
      io->WantCaptureKeyboard = false;
      io->WantCaptureMouse = false;
      igSetWindowFocus_Str(NULL);
    }
  }

  if (uiInputMode) {
    ImGui_ImplSDL3_ProcessEvent(event);
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    ImGuiIO *io = igGetIO();
    bool imguiWantsKeyboard = uiInputMode && io->WantCaptureKeyboard;
    if (!imguiWantsKeyboard) {
      if (event->key.key == SDLK_F1) {
        renderer.isNight = true;
      }
      if (event->key.key == SDLK_F2) {
        renderer.isNight = false;
      }
    }
  }
}

void RenderUI(void) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  igNewFrame();

  ImGuiWindowFlags window_flags = 0;
  if (!uiInputMode) {
    window_flags |= ImGuiWindowFlags_NoInputs;
    window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
  }

  igBegin("Debug Info", &showDebugWindow, window_flags);
  igText("FPS: %d", renderer.FPS);
  igText("Delta Time: %.3f ms", renderer.deltaTime * 1000.0);
  if (renderer.FPS > 0) {
    igText("Frame Time: %.3f ms", 1000.0f / renderer.FPS);
  }
  igSeparator();
  igText("Camera Position: (%.2f, %.2f, %.2f)", renderer.camera.position[0], renderer.camera.position[1], renderer.camera.position[2]);
  igText("Camera Front: (%.2f, %.2f, %.2f)", renderer.camera.front[0], renderer.camera.front[1], renderer.camera.front[2]);
  igSeparator();
  igCheckbox("Night Mode", &renderer.isNight);
  igSeparator();
  igColorEdit3("Clear Color", (float *)&renderer.clearColor, 0);
  igSeparator();
  igText("Controls:");
  igText("WASD - Move camera");
  igText("Space/Ctrl - Up/Down");
  igText("Mouse - Look around");
  igText("F1/F2 - Toggle night mode");
  igText("F3 - Toggle UI mode");
  igText("ESC - Quit");
  igEnd();

  igRender();
  ImGuiIO *ioptr = igGetIO();
  GL(glViewport(0, 0, (int)ioptr->DisplaySize.x, (int)ioptr->DisplaySize.y));
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

  if (ioptr->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
    SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
    igUpdatePlatformWindows();
    igRenderPlatformWindowsDefault(NULL, NULL);
    SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
  }
}

bool IsUIInputModeActive(void) {
  return uiInputMode;
}
