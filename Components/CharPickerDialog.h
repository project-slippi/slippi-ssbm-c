#ifndef CHAR_PICKER_DIALOG_H
#define CHAR_PICKER_DIALOG_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"
#include "CharStageIcon.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef struct CharPickerDialog_State {
  u8 is_open;
} CharPickerDialog_State;

typedef struct CharPickerDialog {
  GOBJ *gobj;
  JOBJ *root_jobj;
  JOBJSet *jobj_set;
  CSIcon *char_icons[CKIND_GANONDORF + 1];
  CharPickerDialog_State state;
} CharPickerDialog;

CharPickerDialog *CharPickerDialog_Init(GUI_GameSetup *gui);
void CharPickerDialog_Free(CharPickerDialog *cpd);

void CharPickerDialog_SetPos(CharPickerDialog *cpd, Vec3 pos);
void CharPickerDialog_OpenDialog(CharPickerDialog *cpd);

#endif