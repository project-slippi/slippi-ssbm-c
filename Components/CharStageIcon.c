#include "CharStageIcon.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

static void _SetSelectState(CSIcon *icon, CSIcon_Select_State state) {
  JOBJ_RemoveAnimAll(icon->root_jobj);

  // Set alpha for both mobjs
  float alpha = 1;
  if (state == CSIcon_Select_State_Disabled) {
    alpha = 0.3;
  }

  HSD_Material *bg_mat = icon->root_jobj->child->dobj->mobj->mat;
  HSD_Material *fg_mat = icon->root_jobj->child->dobj->next->mobj->mat;

  bg_mat->alpha = alpha;  // background
  fg_mat->alpha = alpha;  // foreground

  // Reset the colors of the background. Is there a way to reset to the file configuration?
  bg_mat->diffuse = (GXColor){128, 128, 128, 255};

  if (state == CSIcon_Select_State_Blink) {
    JOBJ_AddSetAnim(icon->root_jobj, icon->jobj_set, 0);
    JOBJ_ReqAnimAll(icon->root_jobj, 0);
  }

  icon->state.select_state = state;
}

CSIcon *CSIcon_Init(GUI_GameSetup *gui) {
  CSIcon *icon = calloc(sizeof(CSIcon));

  // Init icon jobj
  icon->jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_CSIcon];
  icon->gobj = JOBJ_LoadSet(0, icon->jobj_set, 0, 0, 3, 1, 0, GObj_Anim);
  icon->root_jobj = icon->gobj->hsd_object;

  // Init state
  _SetSelectState(icon, CSIcon_Select_State_NotSelected);

  return icon;
}

void CSIcon_Free(CSIcon *icon) {
  // Not implemented because we only initialize a very limited amount of these
}

void CSIcon_SetMaterial(CSIcon *icon, CSIcon_Material matIdx) {
  JOBJ_AddSetAnim(icon->root_jobj, icon->jobj_set, 1);
  JOBJ_ReqAnimAll(icon->root_jobj, matIdx);
  JOBJ_AnimAll(icon->root_jobj);

  // Restore prior active animation here? This isn't strictly necessary because we usually
  // set the material upon initializing and don't worry about it again. But without restoring,
  // setting the material will cause the animation to end
  JOBJ_RemoveAnimAll(icon->root_jobj);

  icon->state.material = matIdx;
}

CSIcon_Material CSIcon_ConvertCharToMat(int charId) {
  switch (charId) {
    case CKIND_FALCON:
      return CSIcon_Material_Falcon;
    case CKIND_DK:
      return CSIcon_Material_DK;
    case CKIND_FOX:
      return CSIcon_Material_Fox;
    case CKIND_GAW:
      return CSIcon_Material_GW;
    case CKIND_KIRBY:
      return CSIcon_Material_Kirby;
    case CKIND_BOWSER:
      return CSIcon_Material_Bowser;
    case CKIND_LINK:
      return CSIcon_Material_Link;
    case CKIND_LUIGI:
      return CSIcon_Material_Luigi;
    case CKIND_MARIO:
      return CSIcon_Material_Mario;
    case CKIND_MARTH:
      return CSIcon_Material_Marth;
    case CKIND_MEWTWO:
      return CSIcon_Material_Mewtwo;
    case CKIND_NESS:
      return CSIcon_Material_Ness;
    case CKIND_PEACH:
      return CSIcon_Material_Peach;
    case CKIND_PIKACHU:
      return CSIcon_Material_Pikachu;
    case CKIND_ICECLIMBERS:
      return CSIcon_Material_ICs;
    case CKIND_JIGGLYPUFF:
      return CSIcon_Material_Puff;
    case CKIND_SAMUS:
      return CSIcon_Material_Samus;
    case CKIND_YOSHI:
      return CSIcon_Material_Yoshi;
    case CKIND_ZELDA:
      return CSIcon_Material_Zelda;
    case CKIND_SHEIK:
      return CSIcon_Material_Sheik;
    case CKIND_FALCO:
      return CSIcon_Material_Falco;
    case CKIND_YOUNGLINK:
      return CSIcon_Material_YLink;
    case CKIND_DRMARIO:
      return CSIcon_Material_Doc;
    case CKIND_ROY:
      return CSIcon_Material_Roy;
    case CKIND_PICHU:
      return CSIcon_Material_Pichu;
    case CKIND_GANONDORF:
      return CSIcon_Material_Ganon;
  }

  return CSIcon_Material_Empty;
}

int CSIcon_ConvertMatToChar(CSIcon_Material mat) {
  switch (mat) {
    case CSIcon_Material_Falcon:
      return CKIND_FALCON;
    case CSIcon_Material_DK:
      return CKIND_DK;
    case CSIcon_Material_Fox:
      return CKIND_FOX;
    case CSIcon_Material_GW:
      return CKIND_GAW;
    case CSIcon_Material_Kirby:
      return CKIND_KIRBY;
    case CSIcon_Material_Bowser:
      return CKIND_BOWSER;
    case CSIcon_Material_Link:
      return CKIND_LINK;
    case CSIcon_Material_Luigi:
      return CKIND_LUIGI;
    case CSIcon_Material_Mario:
      return CKIND_MARIO;
    case CSIcon_Material_Marth:
      return CKIND_MARTH;
    case CSIcon_Material_Mewtwo:
      return CKIND_MEWTWO;
    case CSIcon_Material_Ness:
      return CKIND_NESS;
    case CSIcon_Material_Peach:
      return CKIND_PEACH;
    case CSIcon_Material_Pikachu:
      return CKIND_PIKACHU;
    case CSIcon_Material_ICs:
      return CKIND_ICECLIMBERS;
    case CSIcon_Material_Puff:
      return CKIND_JIGGLYPUFF;
    case CSIcon_Material_Samus:
      return CKIND_SAMUS;
    case CSIcon_Material_Yoshi:
      return CKIND_YOSHI;
    case CSIcon_Material_Zelda:
      return CKIND_ZELDA;
    case CSIcon_Material_Sheik:
      return CKIND_SHEIK;
    case CSIcon_Material_Falco:
      return CKIND_FALCO;
    case CSIcon_Material_YLink:
      return CKIND_YOUNGLINK;
    case CSIcon_Material_Doc:
      return CKIND_DRMARIO;
    case CSIcon_Material_Roy:
      return CKIND_ROY;
    case CSIcon_Material_Pichu:
      return CKIND_PICHU;
    case CSIcon_Material_Ganon:
      return CKIND_GANONDORF;
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