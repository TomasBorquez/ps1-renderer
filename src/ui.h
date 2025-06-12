#pragma once
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#define igGetIO igGetIO_Nil

#include "renderer.h"

void InitImGui(void);
void DestroyImGui(void);
void RenderUI(void);
void ProcessUIEvent(SDL_Event *event);
bool IsUIInputModeActive(void);
