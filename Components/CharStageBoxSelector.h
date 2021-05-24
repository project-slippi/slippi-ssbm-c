#ifndef CHAR_STAGE_BOX_SELECTOR_H
#define CHAR_STAGE_BOX_SELECTOR_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"
#include "CharStageIcon.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef enum CSBoxSelector_Select_State {
  CSBoxSelector_Select_State_NotSelected = 0,
  CSBoxSelector_Select_State_X = 1,
  CSBoxSelector_Select_State_Selected = 2,
  CSBoxSelector_Select_State_Disabled = 3,
} CSBoxSelector_Select_State;

typedef struct CSBoxSelector_State {
  u8 is_hover;
  u8 select_state;
} CSBoxSelector_State;

typedef struct CSBoxSelector {
  GOBJ *gobj;
  JOBJ *root_jobj;
  GOBJ *x_gobj;
  JOBJ *x_jobj;
  CSBoxSelector_State state;
  CSIcon *icon;
} CSBoxSelector;

CSBoxSelector *CSBoxSelector_Init(GUI_GameSetup *gui);
void CSBoxSelector_Free(CSBoxSelector *bs);

void CSBoxSelector_SetHover(CSBoxSelector *bs, u8 is_hover);
void CSBoxSelector_SetSelectState(CSBoxSelector *bs, CSBoxSelector_Select_State state);

void CSBoxSelector_SetPos(CSBoxSelector *bs, Vec3 p);

#endif