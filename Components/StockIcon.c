#include "StockIcon.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

static void _SetIcon(StockIcon *si, u8 charId, u8 charColor) {
  OSReport("[%X] Setting icon: %d - %d\n", si, charId, charColor);
  si->state.char_id = charId;
  si->state.color_id = charColor;

  // Calculate the frame necessary for this character
  u32 adjCharId = charId;
  if (charId == CKIND_SHEIK) {
    adjCharId = 29;  // Sheik is 29 for some reason for animations
  } else if (charId > CKIND_SHEIK) {
    adjCharId--;  // If larger ID than sheik, subtract 1, everything is offset
  }

  JOBJ_AddSetAnim(si->root_jobj, si->jobj_set, 0);
  JOBJ_ReqAnimAll(si->root_jobj, adjCharId + (30 * charColor));
  JOBJ_AnimAll(si->root_jobj);
  JOBJ_RemoveAnimAll(si->root_jobj);
}

StockIcon *StockIcon_Init(GUI_GameSetup *gui) {
  StockIcon *si = calloc(sizeof(StockIcon));

  // Init si jobj
  si->jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_StockIcon];
  si->gobj = JOBJ_LoadSet(0, si->jobj_set, 0, 0, 3, 1, 0, 0);
  si->root_jobj = si->gobj->hsd_object;

  // Init state
  _SetIcon(si, 0, 0);

  return si;
}

void StockIcon_Free(StockIcon *si) {
  // Not implemented because we only initialize a very limited amount of these
}

void StockIcon_SetIcon(StockIcon *si, u8 charId, u8 charColor) {
  if (charId == si->state.char_id && charColor == si->state.color_id) {
    return;
  }

  _SetIcon(si, charId, charColor);
}