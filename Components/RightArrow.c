#include "RightArrow.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

static void _OffsetJointPos(JOBJ *jobj, Vec3 prev, Vec3 new) {
  jobj->trans.X = jobj->trans.X - prev.X + new.X;
  jobj->trans.Y = jobj->trans.Y - prev.Y + new.Y;
  jobj->trans.Z = jobj->trans.Z - prev.Z + new.Z;
}

static void _SetDisplayState(RightArrow *ra, RightArrow_Display_State state) {
  JOBJ_RemoveAnimAll(ra->root_jobj);

  float alpha = 1;
  if (state == RightArrow_Display_State_DIM) {
    alpha = 0.3;
  }

  HSD_Material *mat = ra->root_jobj->dobj->mobj->mat;

  // Set alpha and color to reset things
  mat->alpha = alpha;
  mat->diffuse = (GXColor){128, 128, 128, 255};

  // If blinking, start the animation
  if (state == RightArrow_Display_State_BLINK) {
    JOBJ_AddSetAnim(ra->root_jobj, ra->jobj_set, 0);
    JOBJ_ReqAnimAll(ra->root_jobj, 0);  // Start on frame 0
  }

  ra->state.display_state = state;
}

RightArrow *RightArrow_Init(GUI_GameSetup *gui) {
  RightArrow *ra = calloc(sizeof(RightArrow));

  // Init ra jobj
  ra->jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_RightArrow];
  ra->gobj = JOBJ_LoadSet(0, ra->jobj_set, 0, 0, 3, 1, 0, GObj_Anim);
  ra->root_jobj = ra->gobj->hsd_object;

  // Init state
  _SetDisplayState(ra, RightArrow_Display_State_NORMAL);

  return ra;
}

void RightArrow_Free(RightArrow *ra) {
  // Not implemented because we only initialize a very limited amount of these
}

void RightArrow_SetPos(RightArrow *ra, Vec3 p) {
  ra->root_jobj->trans = p;
}

void RightArrow_SetOffsetPos(RightArrow *ra, Vec3 ofst) {
  _OffsetJointPos(ra->root_jobj->child, ra->state.offset_pos, ofst);
  _OffsetJointPos(ra->root_jobj->child->sibling, ra->state.offset_pos, ofst);
  _OffsetJointPos(ra->root_jobj->child->sibling->sibling, ra->state.offset_pos, ofst);
  _OffsetJointPos(ra->root_jobj->child->sibling->sibling->sibling, ra->state.offset_pos, ofst);

  ra->state.offset_pos = ofst;
}

void RightArrow_SetDisplayState(RightArrow *ra, RightArrow_Display_State state) {
  if (ra->state.display_state == state) {
    return;
  }

  _SetDisplayState(ra, state);
}