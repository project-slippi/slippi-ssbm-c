#include "CharStageBoxSelector.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

CSBoxSelector *CSBoxSelector_Init(GUI_GameSetup *gui) {
  CSBoxSelector *csbs = calloc(sizeof(CSBoxSelector));

  // Init box selector jobj
  JOBJSet *set = gui->jobjs[GUI_GameSetup_JOBJ_CSBoxSelector];
  csbs->gobj = JOBJ_LoadSet(0, set, 0, 0, 3, 1, 1, GObj_Anim);
  csbs->root_jobj = csbs->gobj->hsd_object;

  // Init icon
  csbs->icon = CSIcon_Init(gui);

  // Init state
  CSBoxSelector_ClearHover(csbs);

  return csbs;
}

void CSBoxSelector_Free(CSBoxSelector *bs) {
  // Not implemented because we only initialize a very limited amount of these
}

void CSBoxSelector_SetHover(CSBoxSelector *bs) {
  JOBJ *jobj = bs->root_jobj->child;
  jobj->flags = jobj->flags & ~JOBJ_HIDDEN;  // Clear hidden flag

  // Reset animation to start in the same place
  JOBJ_ReqAnimAll(bs->root_jobj, 0);

  bs->state->is_hover = true;
}

void CSBoxSelector_ClearHover(CSBoxSelector *bs) {
  JOBJ *jobj = bs->root_jobj->child;
  jobj->flags = jobj->flags | JOBJ_HIDDEN;  // Set hidden flag

  bs->state->is_hover = false;
}

void CSBoxSelector_SetPos(CSBoxSelector *bs, Vec3 p) {
  bs->root_jobj->trans = p;
  CSIcon_SetPos(bs->icon, p);
}