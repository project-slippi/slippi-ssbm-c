#ifndef FLAT_TEXTURE_H
#define FLAT_TEXTURE_H

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef enum FlatTexture_Texture {
  FlatTexture_Texture_YOUR_CHAR_LABEL,
  FlatTexture_Texture_OPP_CHAR_LABEL,
  FlatTexture_Texture_STRIKE1_LABEL,
  FlatTexture_Texture_STRIKE23_LABEL,
  FlatTexture_Texture_STRIKE4_LABEL,
  FlatTexture_Texture_BAN_LABEL,
  FlatTexture_Texture_STAGE_LABEL,
} FlatTexture_Texture;

typedef struct FlatTexture_State {
  u8 is_visible;
} FlatTexture_State;

typedef struct FlatTexture {
  GOBJ *gobj;
  JOBJ *root_jobj;
  FlatTexture_State state;
} FlatTexture;

FlatTexture *FlatTexture_Init(GUI_GameSetup *gui);
void FlatTexture_Free(FlatTexture *ft);

void FlatTexture_SetVisibility(FlatTexture *ft, u8 is_visible);
void FlatTexture_SetTexture(FlatTexture *ft, FlatTexture_Texture tex);
void FlatTexture_SetPos(FlatTexture *ft, Vec3 p);
void FlatTexture_SetPosCorners(FlatTexture *ft, Vec3 tl, Vec3 tr, Vec3 bl, Vec3 br);

#endif