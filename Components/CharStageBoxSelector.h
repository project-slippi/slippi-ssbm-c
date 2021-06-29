#ifndef CHAR_STAGE_BOX_SELECTOR_H
#define CHAR_STAGE_BOX_SELECTOR_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"
#include "CharStageIcon.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef enum CSBoxSelector_Select_State {
  CSBoxSelector_Select_State_NotSelected,
  CSBoxSelector_Select_State_Selected_X,
  CSBoxSelector_Select_State_Selected,
  CSBoxSelector_Select_State_Disabled,
  CSBoxSelector_Select_State_Disabled_X,
} CSBoxSelector_Select_State;

typedef struct CSBoxSelector_State {
  u8 is_hover;
  u8 is_selected;
  u8 is_disabled;
  u8 is_visible;
  CSBoxSelector_Select_State select_state;
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
void CSBoxSelector_SetVisibility(CSBoxSelector *bs, u8 is_visible);
void CSBoxSelector_SetPos(CSBoxSelector *bs, Vec3 p);

#endif