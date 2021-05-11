#ifndef CHAR_STAGE_ICON_H
#define CHAR_STAGE_ICON_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef enum CSIcon_Material {
  CSIcon_Material_Empty = 0,
  CSIcon_Material_Question = 1,
  CSIcon_Material_Battlefield = 2,
  CSIcon_Material_Yoshis = 3,
  CSIcon_Material_Dreamland = 4,
  CSIcon_Material_FinalDestination = 5,
  CSIcon_Material_Fountain = 6,
  CSIcon_Material_Pokemon = 7,
  CSIcon_Material_Falcon = 8,
} CSIcon_Material;

typedef struct CSIcon {
  GOBJ *gobj;
  JOBJ *root_jobj;
} CSIcon;

CSIcon *CSIcon_Init(GUI_GameSetup *gui);
void CSIcon_Free(CSIcon *icon);

void CSIcon_SetMaterial(CSIcon *icon, CSIcon_Material matIdx);
CSIcon_Material CSIcon_GetCharMaterial(int charId);
CSIcon_Material CSIcon_GetStageMaterial(int stageId);

void CSIcon_SetPos(CSIcon *icon, Vec3 p);

#endif