#include "CharStageBoxSelector.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

// Public functions
CSBoxSelector *CSBoxSelector_Init(GUI_GameSetup *gui) {
  CSBoxSelector *csbs = calloc(sizeof(CSBoxSelector));

  // Init box selector jobj
  JOBJSet *set = gui->jobjs[GUI_GameSetup_JOBJ_CSBoxSelector];
  csbs->gobj = JOBJ_LoadSet(0, set, 0, 0, 3, 1, 1, GObj_Anim);
  csbs->root_jobj = csbs->gobj->hsd_object;

  // Init icon
  csbs->icon = CSIcon_Init(gui);

  // Init X strike selected state
  JOBJSet *x_set = gui->jobjs[GUI_GameSetup_JOBJ_X];
  csbs->x_gobj = JOBJ_LoadSet(0, x_set, 0, 0, 3, 1, 1, GObj_Anim);
  csbs->x_jobj = csbs->x_gobj->hsd_object;

  // Init state
  CSBoxSelector_SetHover(csbs, false);
  CSBoxSelector_SetSelectState(csbs, CSBoxSelector_Select_State_NotSelected);

  return csbs;
}

void CSBoxSelector_Free(CSBoxSelector *bs) {
  // Not implemented because we only initialize a very limited amount of these
}

void CSBoxSelector_SetHover(CSBoxSelector *bs, u8 is_hover) {
  JOBJ *jobj = bs->root_jobj->child;

  if (is_hover) {
    jobj->flags = jobj->flags & ~JOBJ_HIDDEN;  // Clear hidden flag

    // Reset animation to start in the same place
    JOBJ_ReqAnimAll(bs->root_jobj, 0);
  } else {
    jobj->flags = jobj->flags | JOBJ_HIDDEN;  // Set hidden flag
  }

  bs->state.is_hover = is_hover;
}

void CSBoxSelector_SetPos(CSBoxSelector *bs, Vec3 p) {
  bs->root_jobj->trans = p;
  bs->x_jobj->trans = p;
  CSIcon_SetPos(bs->icon, p);
}

void CSBoxSelector_SetSelectState(CSBoxSelector *bs, CSBoxSelector_Select_State state) {
  // Reset state on all state JOBJs
  bs->x_jobj->flags = bs->x_jobj->flags | JOBJ_HIDDEN;  // Hide X

  if (state == CSBoxSelector_Select_State_X) {
    bs->x_jobj->flags = bs->x_jobj->flags & ~JOBJ_HIDDEN;
    JOBJ_ReqAnimAll(bs->x_jobj, 0);
  }

  bs->state.select_state = state;
}