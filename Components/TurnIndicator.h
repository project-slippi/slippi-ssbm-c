#ifndef TURN_INDICATOR_H
#define TURN_INDICATOR_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef enum TurnIndicator_Direction {
  TurnIndicator_Direction_LEFT,
  TurnIndicator_Direction_RIGHT,
} TurnIndicator_Direction;

typedef enum TurnIndicator_DisplayState {
  TurnIndicator_DisplayState_STATIC,
  TurnIndicator_DisplayState_ANIM_GREY,
  TurnIndicator_DisplayState_ANIM_YEL,
} TurnIndicator_DisplayState;

typedef struct TurnIndicator_State {
  TurnIndicator_DisplayState display_state;
} TurnIndicator_State;

typedef struct TurnIndicator {
  GOBJ *gobj;
  JOBJ *root_jobj;
  JOBJSet *jobj_set;
  TurnIndicator_State state;
} TurnIndicator;

TurnIndicator *TurnIndicator_Init(GUI_GameSetup *gui, TurnIndicator_Direction dir);
void TurnIndicator_Free(TurnIndicator *ti);

void TurnIndicator_SetDisplayState(TurnIndicator *ti, TurnIndicator_DisplayState ds);
void TurnIndicator_SetPos(TurnIndicator *ti, Vec3 p);

#endif