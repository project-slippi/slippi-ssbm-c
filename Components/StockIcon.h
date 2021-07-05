#ifndef STOCK_ICON_H
#define STOCK_ICON_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef struct StockIcon_State {
  u8 is_visible;
  u8 char_id;
  u8 color_id;
} StockIcon_State;

typedef struct StockIcon {
  GOBJ *gobj;
  JOBJ *root_jobj;
  JOBJSet *jobj_set;
  StockIcon_State state;
} StockIcon;

StockIcon *StockIcon_Init(GUI_GameSetup *gui);
void StockIcon_Free(StockIcon *si);

void StockIcon_SetIcon(StockIcon *si, u8 charId, u8 charColor);

#endif