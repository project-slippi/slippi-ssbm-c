#include "CharStageBoxSelector.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

static void _SetHover(CSBoxSelector *bs, u8 is_hover) {
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

static void _SetSelectState(CSBoxSelector *bs, CSBoxSelector_Select_State state) {
  // Init flags that may be changed depending on state
  CSIcon_Select_State icon_ss = CSIcon_Select_State_NotSelected;

  // Reset state on all state JOBJs
  bs->x_jobj->flags |= JOBJ_HIDDEN;                      // Hide X
  bs->root_jobj->child->sibling->flags &= ~JOBJ_HIDDEN;  // Show border

  // Show X for the X states
  if (state == CSBoxSelector_Select_State_Selected_X) {
    bs->x_jobj->flags &= ~JOBJ_HIDDEN;
    JOBJ_ReqAnimAll(bs->x_jobj, 0);
    bs->x_jobj->dobj->mobj->mat->diffuse = (GXColor){0xFF, 0x0, 0x0, 0xFF};
  } else if (state == CSBoxSelector_Select_State_Disabled_X) {
    bs->x_jobj->flags &= ~JOBJ_HIDDEN;
    JOBJ_ReqAnimAll(bs->x_jobj, 4);
    bs->x_jobj->dobj->mobj->mat->diffuse = (GXColor){0x87, 0x87, 0x87, 0xFF};
  }

  bs->state.is_selected = state == CSBoxSelector_Select_State_Selected ||
                          state == CSBoxSelector_Select_State_Selected_X;
  bs->state.is_disabled = state == CSBoxSelector_Select_State_Disabled ||
                          state == CSBoxSelector_Select_State_Disabled_X;

  // If disabled, hide border and set transparency
  if (bs->state.is_disabled) {
    bs->root_jobj->child->sibling->flags |= JOBJ_HIDDEN;  // Hide border
    icon_ss = CSIcon_Select_State_Disabled;
  }
  CSIcon_SetSelectState(bs->icon, icon_ss);

  // Commit new state
  bs->state.select_state = state;
}

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

  // Attach positions of dependent JOBJs
  JOBJ_AttachPosition(csbs->icon->root_jobj, csbs->root_jobj);
  JOBJ_AttachPosition(csbs->x_jobj, csbs->root_jobj);

  // Init state
  _SetHover(csbs, false);
  _SetSelectState(csbs, CSBoxSelector_Select_State_NotSelected);

  return csbs;
}

void CSBoxSelector_Free(CSBoxSelector *bs) {
  // Not implemented because we only initialize a very limited amount of these
}

void CSBoxSelector_SetHover(CSBoxSelector *bs, u8 is_hover) {
  if (is_hover == bs->state.is_hover) {
    return;
  }

  _SetHover(bs, is_hover);
}

void CSBoxSelector_SetPos(CSBoxSelector *bs, Vec3 p) {
  bs->root_jobj->trans = p;

  // Don't think the logic below is needed anymore since we are attaching positions in constructor
  // bs->x_jobj->trans = p;
  // CSIcon_SetPos(bs->icon, p);
}

void CSBoxSelector_SetSelectState(CSBoxSelector *bs, CSBoxSelector_Select_State state) {
  if (state == bs->state.select_state) {
    return;
  }

  _SetSelectState(bs, state);
}