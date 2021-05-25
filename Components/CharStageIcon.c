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
  icon->state.material = matIdx;
}

CSIcon_Material CSIcon_ConvertCharToMat(int charId) {
  // TODO: Implement if we need
}

int CSIcon_ConvertMatToChar(CSIcon_Material mat) {
  // TODO: Implement if we need
}

CSIcon_Material CSIcon_ConvertStageToMat(int stageId) {
  switch (stageId) {
    case 0x2:
      return CSIcon_Material_Fountain;
    case 0x3:
      return CSIcon_Material_Pokemon;
    case 0x8:
      return CSIcon_Material_Yoshis;
    case 0x1C:
      return CSIcon_Material_Dreamland;
    case 0x1F:
      return CSIcon_Material_Battlefield;
    case 0x20:
      return CSIcon_Material_FinalDestination;
  }

  return CSIcon_Material_Empty;
}

int CSIcon_ConvertMatToStage(CSIcon_Material mat) {
  switch (mat) {
    case CSIcon_Material_Fountain:
      return 0x2;
    case CSIcon_Material_Pokemon:
      return 0x3;
    case CSIcon_Material_Yoshis:
      return 0x8;
    case CSIcon_Material_Dreamland:
      return 0x1C;
    case CSIcon_Material_Battlefield:
      return 0x1F;
    case CSIcon_Material_FinalDestination:
      return 0x20;
  }

  return 0;
}

void CSIcon_SetPos(CSIcon *icon, Vec3 p) {
  icon->root_jobj->trans = p;
}