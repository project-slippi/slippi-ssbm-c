#ifndef CHAR_PICKER_DIALOG_H
#define CHAR_PICKER_DIALOG_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"
#include "CharStageIcon.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

#define CPD_LAST_INDEX 26   // Last index in char picker dialog JObj set
#define CKIND_RANDOM 26     // Index of Random in char picker dialog

typedef struct CharPickerDialog_State {
  Vec3 pos;
  u8 is_open;
  u8 char_selection_idx;
  u8 char_color_idx;
  u32 open_frame_count;
} CharPickerDialog_State;

typedef struct CharPickerDialog {
  GOBJ *gobj;
  JOBJ *root_jobj;
  JOBJSet *jobj_set;
  CSIcon *char_icons[CPD_LAST_INDEX + 1];
  CharPickerDialog_State state;

  void (*on_close)();
  u8 (*get_next_color)();
} CharPickerDialog;

void SelectRandomChar();

CharPickerDialog *CharPickerDialog_Init(GUI_GameSetup *gui, void *on_close, void *get_next_color);
void CharPickerDialog_Free(CharPickerDialog *cpd);

void CharPickerDialog_SetPos(CharPickerDialog *cpd, Vec3 pos);
void CharPickerDialog_OpenDialog(CharPickerDialog *cpd, u8 start_char_idx, u8 start_char_color);
void CharPickerDialog_CloseDialog(CharPickerDialog *cpd);

#endif