#ifndef CHAR_STAGE_ICON_H
#define CHAR_STAGE_ICON_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef enum CSIcon_Material {
  CSIcon_Material_Empty,
  CSIcon_Material_Question,
  CSIcon_Material_Battlefield,
  CSIcon_Material_Yoshis,
  CSIcon_Material_Dreamland,
  CSIcon_Material_FinalDestination,
  CSIcon_Material_Fountain,
  CSIcon_Material_Pokemon,
  CSIcon_Material_Falcon,
  CSIcon_Material_DK,
  CSIcon_Material_Fox,
  CSIcon_Material_GW,
  CSIcon_Material_Kirby,
  CSIcon_Material_Bowser,
  CSIcon_Material_Link,
  CSIcon_Material_Luigi,
  CSIcon_Material_Mario,
  CSIcon_Material_Marth,
  CSIcon_Material_Mewtwo,
  CSIcon_Material_Ness,
  CSIcon_Material_Peach,
  CSIcon_Material_Pikachu,
  CSIcon_Material_ICs,
  CSIcon_Material_Puff,
  CSIcon_Material_Samus,
  CSIcon_Material_Yoshi,
  CSIcon_Material_Zelda,
  CSIcon_Material_Sheik,
  CSIcon_Material_Falco,
  CSIcon_Material_YLink,
  CSIcon_Material_Doc,
  CSIcon_Material_Roy,
  CSIcon_Material_Pichu,
  CSIcon_Material_Ganon,
} CSIcon_Material;

#define CSIcon_LAST_STAGE_MAT_IDX CSIcon_Material_Pokemon

typedef enum CSIcon_Select_State {
  CSIcon_Select_State_NotSelected,
  CSIcon_Select_State_Selected,
  CSIcon_Select_State_Disabled,
  CSIcon_Select_State_Blink,
} CSIcon_Select_State;

typedef struct CSIcon_State {
  CSIcon_Material material;
  CSIcon_Select_State select_state;
  u8 is_visible;
} CSIcon_State;

typedef struct CSIcon {
  GOBJ *gobj;
  JOBJ *root_jobj;
  JOBJSet *jobj_set;
  CSIcon_State state;
} CSIcon;

CSIcon *CSIcon_Init(GUI_GameSetup *gui);
void CSIcon_Free(CSIcon *icon);

void CSIcon_SetMaterial(CSIcon *icon, CSIcon_Material matIdx);
CSIcon_Material CSIcon_ConvertCharToMat(int charId);
CSIcon_Material CSIcon_ConvertStageToMat(int stageId);
int CSIcon_ConvertMatToChar(CSIcon_Material mat);
int CSIcon_ConvertMatToStage(CSIcon_Material mat);

void CSIcon_SetVisibility(CSIcon *icon, u8 is_visible);
void CSIcon_SetPos(CSIcon *icon, Vec3 p);
void CSIcon_SetSelectState(CSIcon *icon, CSIcon_Select_State state);

#endif