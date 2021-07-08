#ifndef GAME_RESULT_H
#define GAME_RESULT_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef enum GameResult_Result {
  GameResult_Result_EMPTY,
  GameResult_Result_WIN,
  GameResult_Result_LOSS,
} GameResult_Result;

typedef struct GameResult_State {
  u8 is_visible;
} GameResult_State;

typedef struct GameResult {
  GOBJ *gobj;
  JOBJ *root_jobj;
  GameResult_State state;
} GameResult;

GameResult *GameResult_Init(GUI_GameSetup *gui);
void GameResult_Free(GameResult *gr);

void GameResult_SetResult(GameResult *gr, GameResult_Result res);
void GameResult_SetPos(GameResult *gr, Vec3 p);

#endif