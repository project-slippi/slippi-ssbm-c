#include "CharStageIcon.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

static void _SetSelectState(CSIcon *icon, CSIcon_Select_State state) {
  // Set alpha for both mobjs
  float alpha = 1;
  if (state == CSIcon_Select_State_Disabled) {
    alpha = 0.3;
  }
  icon->root_jobj->child->dobj->mobj->mat->alpha = alpha;        // bg
  icon->root_jobj->child->dobj->next->mobj->mat->alpha = alpha;  // material

  icon->state.select_state = state;
}

CSIcon *CSIcon_Init(GUI_GameSetup *gui) {
  CSIcon *icon = calloc(sizeof(CSIcon));

  // Init icon jobj
  JOBJSet *set = gui->jobjs[GUI_GameSetup_JOBJ_CSIcon];
  icon->gobj = JOBJ_LoadSet(0, set, 0, 0, 3, 1, 1, 0);
  icon->root_jobj = icon->gobj->hsd_object;

  // Init state
  _SetSelectState(icon, CSIcon_Select_State_NotSelected);

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
  switch (charId) {
    case CKIND_FALCON:
      return CSIcon_Material_Falcon;
  }

  return CSIcon_Material_Empty;
}

int CSIcon_ConvertMatToChar(CSIcon_Material mat) {
  switch (mat) {
    case CSIcon_Material_Falcon:
      return CKIND_FALCON;
  }

  return 0;
}

CSIcon_Material CSIcon_ConvertStageToMat(int stageId) {
  switch (stageId) {
    case GRKINDEXT_IZUMI:
      return CSIcon_Material_Fountain;
    case GRKINDEXT_PSTAD:
      return CSIcon_Material_Pokemon;
    case GRKINDEXT_STORY:
      return CSIcon_Material_Yoshis;
    case GRKINDEXT_OLDPU:
      return CSIcon_Material_Dreamland;
    case GRKINDEXT_BATTLE:
      return CSIcon_Material_Battlefield;
    case GRKINDEXT_FD:
      return CSIcon_Material_FinalDestination;
  }

  return CSIcon_Material_Empty;
}

int CSIcon_ConvertMatToStage(CSIcon_Material mat) {
  switch (mat) {
    case CSIcon_Material_Fountain:
      return GRKINDEXT_IZUMI;
    case CSIcon_Material_Pokemon:
      return GRKINDEXT_PSTAD;
    case CSIcon_Material_Yoshis:
      return GRKINDEXT_STORY;
    case CSIcon_Material_Dreamland:
      return GRKINDEXT_OLDPU;
    case CSIcon_Material_Battlefield:
      return GRKINDEXT_BATTLE;
    case CSIcon_Material_FinalDestination:
      return GRKINDEXT_FD;
  }

  return 0;
}

void CSIcon_SetPos(CSIcon *icon, Vec3 p) {
  icon->root_jobj->trans = p;
}

void CSIcon_SetSelectState(CSIcon *icon, CSIcon_Select_State state) {
  if (icon->state.select_state == state) {
    return;
  }

  _SetSelectState(icon, state);
}