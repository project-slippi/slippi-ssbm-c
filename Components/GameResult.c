#include "GameResult.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

GameResult *GameResult_Init(GUI_GameSetup *gui) {
  GameResult *gr = calloc(sizeof(GameResult));

  // Init gr jobj
  JOBJSet *jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_GameResult];
  gr->gobj = JOBJ_LoadSet(0, jobj_set, 0, 0, 3, 1, 1, 0);
  gr->root_jobj = gr->gobj->hsd_object;

  return gr;
}

void GameResult_Free(GameResult *gr) {
  // Not implemented because we only initialize a very limited amount of these
}

void GameResult_SetResult(GameResult *gr, GameResult_Result res) {
  JOBJ_ReqAnimAll(gr->root_jobj, res);
  JOBJ_AnimAll(gr->root_jobj);
}

void GameResult_SetPos(GameResult *gr, Vec3 p) {
  gr->root_jobj->trans = p;
  JOBJ_SetMtxDirtySub(gr->root_jobj);
}