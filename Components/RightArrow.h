#ifndef RIGHT_ARROW_H
#define RIGHT_ARROW_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef enum RightArrow_Display_State {
  RightArrow_Display_State_NORMAL,
  RightArrow_Display_State_DIM,
  RightArrow_Display_State_BLINK,
} RightArrow_Display_State;

typedef struct RightArrow_State {
  RightArrow_Display_State display_state;
  Vec3 offset_pos;
} RightArrow_State;

typedef struct RightArrow {
  GOBJ *gobj;
  JOBJ *root_jobj;
  JOBJSet *jobj_set;
  RightArrow_State state;
} RightArrow;

RightArrow *RightArrow_Init(GUI_GameSetup *gui);
void RightArrow_Free(RightArrow *ra);

void RightArrow_SetDisplayState(RightArrow *ra, RightArrow_Display_State state);
void RightArrow_SetPos(RightArrow *ra, Vec3 p);
void RightArrow_SetOffsetPos(RightArrow *ra, Vec3 ofst);

#endif