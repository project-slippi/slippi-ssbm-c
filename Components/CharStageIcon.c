#include "CharStageIcon.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

CSIcon *CSIcon_Init(GUI_GameSetup *gui) {
  CSIcon *icon = calloc(sizeof(CSIcon));

  // Init icon jobj
  JOBJSet *set = gui->jobjs[GUI_GameSetup_JOBJ_CSIcon];
  icon->gobj = JOBJ_LoadSet(0, set, 0, 0, 3, 1, 1, 0);
  icon->root_jobj = icon->gobj->hsd_object;

  return icon;
}

void CSIcon_Free(CSIcon *icon) {
  // Not implemented because we only initialize a very limited amount of these
}

void CSIcon_SetMaterial(CSIcon *icon, CSIcon_Material matIdx) {
  JOBJ_ReqAnimAll(icon->root_jobj, matIdx);
  JOBJ_AnimAll(icon->root_jobj);
}

CSIcon_Material CSIcon_GetCharMaterial(int charId) {
  // TODO: Implement if we need
}

CSIcon_Material CSIcon_GetStageMaterial(int stageId) {
  // TODO: Implement if we need
}

void CSIcon_SetPos(CSIcon *icon, Vec3 p) {
  icon->root_jobj->trans = p;
}