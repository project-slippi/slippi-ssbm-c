#include "StockIcon.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

StockIcon *StockIcon_Init(GUI_GameSetup *gui) {
  StockIcon *si = calloc(sizeof(StockIcon));

  // Init si jobj
  JOBJSet *jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_StockIcon];
  si->gobj = JOBJ_LoadSet(0, jobj_set, 0, 0, 3, 1, 1, 0);
  si->root_jobj = si->gobj->hsd_object;

  return si;
}

void StockIcon_Free(StockIcon *si) {
  // Not implemented because we only initialize a very limited amount of these
}

void StockIcon_SetIcon(StockIcon *si, u8 charId, u8 charColor) {
  // Calculate the frame necessary for this character
  u32 adjCharId = charId;
  if (charId == CKIND_SHEIK) {
    adjCharId = 29;  // Sheik is 29 for some reason for animations
  } else if (charId > CKIND_SHEIK) {
    adjCharId--;  // If larger ID than sheik, subtract 1, everything is offset
  }

  JOBJ_ReqAnimAll(si->root_jobj, adjCharId + (30 * charColor));
  JOBJ_AnimAll(si->root_jobj);
}