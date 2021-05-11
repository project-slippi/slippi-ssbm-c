#ifndef CHAR_STAGE_BOX_SELECTOR_H
#define CHAR_STAGE_BOX_SELECTOR_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"
#include "CharStageIcon.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef struct CSBoxSelector_State {
  u8 is_hover;
} CSBoxSelector_State;

typedef struct CSBoxSelector {
  GOBJ *gobj;
  JOBJ *root_jobj;
  CSBoxSelector_State *state;
  CSIcon *icon;
} CSBoxSelector;

CSBoxSelector *CSBoxSelector_Init(GUI_GameSetup *gui);
void CSBoxSelector_Free(CSBoxSelector *bs);

void CSBoxSelector_SetHover(CSBoxSelector *bs);
void CSBoxSelector_ClearHover(CSBoxSelector *bs);

void CSBoxSelector_SetPos(CSBoxSelector *bs, Vec3 p);

#endif