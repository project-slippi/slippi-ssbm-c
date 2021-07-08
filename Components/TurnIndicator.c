#include "TurnIndicator.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

static void _SetDisplayState(TurnIndicator *ti, TurnIndicator_DisplayState ds) {
  JOBJ_RemoveAnimAll(ti->root_jobj);

  HSD_Material *big_arrow_mat = ti->root_jobj->child->dobj->mobj->mat;
  HSD_Material *small_arrow_mat = ti->root_jobj->child->sibling->dobj->mobj->mat;

  // Reset the colors of the background. Is there a way to reset to the file configuration?
  big_arrow_mat->diffuse = (GXColor){128, 128, 128, 255};
  small_arrow_mat->diffuse = (GXColor){128, 128, 128, 255};

  if (ds == TurnIndicator_DisplayState_ANIM_YEL) {
    big_arrow_mat->diffuse = (GXColor){254, 202, 52, 255};
    small_arrow_mat->diffuse = (GXColor){254, 202, 52, 255};
    JOBJ_AddSetAnim(ti->root_jobj, ti->jobj_set, 0);
    JOBJ_ReqAnimAll(ti->root_jobj, 0);
  } else if (ds == TurnIndicator_DisplayState_ANIM_GRAY) {
    JOBJ_AddSetAnim(ti->root_jobj, ti->jobj_set, 0);
    JOBJ_ReqAnimAll(ti->root_jobj, 0);
  } else {
    // Reset position
    JOBJ_AddSetAnim(ti->root_jobj, ti->jobj_set, 0);
    JOBJ_ReqAnimAll(ti->root_jobj, 0);
    JOBJ_AnimAll(ti->root_jobj);
    JOBJ_RemoveAnimAll(ti->root_jobj);
  }

  ti->state.display_state = ds;
}

TurnIndicator *TurnIndicator_Init(GUI_GameSetup *gui, TurnIndicator_Direction dir) {
  TurnIndicator *ti = calloc(sizeof(TurnIndicator));

  // Init ti jobj
  ti->jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_TurnIndicator];
  ti->gobj = JOBJ_LoadSet(0, ti->jobj_set, 0, 0, 3, 1, 1, GObj_Anim);
  ti->root_jobj = ti->gobj->hsd_object;

  // Set direction
  ti->root_jobj->rot.Z = dir == TurnIndicator_Direction_LEFT ? M_PI : 0;

  // Init state
  _SetDisplayState(ti, TurnIndicator_DisplayState_STATIC);

  return ti;
}

void TurnIndicator_Free(TurnIndicator *ti) {
  // Not implemented because we only initialize a very limited amount of these
}

void TurnIndicator_SetDisplayState(TurnIndicator *ti, TurnIndicator_DisplayState ds) {
  if (ti->state.display_state == ds) {
    return;
  }

  _SetDisplayState(ti, ds);
}

void TurnIndicator_SetPos(TurnIndicator *ti, Vec3 p) {
  ti->root_jobj->trans = p;
  JOBJ_SetMtxDirtySub(ti->root_jobj);
}