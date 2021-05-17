#ifndef BUTTON_H
#define BUTTON_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef enum Button_Material {
  Button_Material_Ok = 0,
  Button_Material_Redo = 1,
} Button_Material;

typedef struct Button_State {
  u8 is_hover;
  u8 is_visible;
} Button_State;

typedef struct Button {
  GOBJ *gobj;
  JOBJ *root_jobj;
  JOBJSet *jobj_set;
  Button_State state;
} Button;

Button *Button_Init(GUI_GameSetup *gui, GUI_GameSetup_JOBJ type);
void Button_Free(Button *btn);

void Button_SetVisibility(Button *btn, u8 is_visible);
void Button_SetHover(Button *btn, u8 is_hover);
void Button_SetMaterial(Button *btn, Button_Material matIdx);
void Button_SetPos(Button *btn, Vec3 p);

#endif