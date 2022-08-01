#include "FlatTexture.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

static void _SetVisibility(FlatTexture *ft, u8 is_visible) {
  JOBJ *jobj = ft->root_jobj;

  if (is_visible) {
    jobj->flags &= ~JOBJ_HIDDEN;  // Clear hidden flag
  } else {
    jobj->flags |= JOBJ_HIDDEN;  // Set hidden flag
  }

  ft->state.is_visible = is_visible;
}

FlatTexture *FlatTexture_Init(GUI_GameSetup *gui) {
  FlatTexture *ft = calloc(sizeof(FlatTexture));

  // Init ft jobj
  JOBJSet *jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_FlatTexture];
  ft->gobj = JOBJ_LoadSet(0, jobj_set, 0, 0, 3, 1, 1, 0);
  ft->root_jobj = ft->gobj->hsd_object;

  // Init state
  _SetVisibility(ft, true);

  return ft;
}

void FlatTexture_Free(FlatTexture *ft) {
  // Not implemented because we only initialize a very limited amount of these
}

void FlatTexture_SetTexture(FlatTexture *ft, FlatTexture_Texture tex) {
  // Add set anim id 1 which allows us to change texture and animate
  JOBJ_ReqAnimAll(ft->root_jobj, tex);
  JOBJ_AnimAll(ft->root_jobj);
}

void FlatTexture_SetVisibility(FlatTexture *ft, u8 is_visible) {
  if (ft->state.is_visible == is_visible) {
    return;
  }

  _SetVisibility(ft, is_visible);
}

void FlatTexture_SetPos(FlatTexture *ft, Vec3 p) {
  ft->root_jobj->trans = p;
}

void FlatTexture_SetPosCorners(FlatTexture *ft, Vec3 tl, Vec3 tr, Vec3 bl, Vec3 br) {
  ft->root_jobj->child->trans = tr;
  ft->root_jobj->child->sibling->trans = bl;
  ft->root_jobj->child->sibling->sibling->trans = tl;
  ft->root_jobj->child->sibling->sibling->sibling->trans = br;
}

void FlatTexture_SetSize(FlatTexture *ft, float width, float height) {
  float desc_width_half = width / 2;
  float desc_height_half = height / 2;
  
  Vec3 tl = {-desc_width_half, desc_height_half, 0};
  Vec3 tr = {desc_width_half, desc_height_half, 0};
  Vec3 bl = {-desc_width_half, -desc_height_half, 0};
  Vec3 br = {desc_width_half, -desc_height_half, 0};
  FlatTexture_SetPosCorners(ft, tl, tr, bl, br);
}

void FlatTexture_SetColor(FlatTexture *ft, GXColor color) {
  ft->root_jobj->dobj->mobj->mat->diffuse = color;
}