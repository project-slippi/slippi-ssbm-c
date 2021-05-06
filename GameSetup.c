#include "GameSetup.h"

#include "./m-ex/MexTK/mex.h"

static ArchiveInfo *gui_archive;
static GmTou1Data *tou1_assets;

void Minor_Load(void *minor_data) {
  OSReport("minor load\n");

  gui_archive = File_Load("GmTou1p.dat");
  tou1_assets = File_GetSymbol(gui_archive, "ScGamTour_scene_data");

  GOBJ *cam_gobj = GObj_Create(2, 3, 128);
  COBJ *cam_cobj = COBJ_LoadDesc(tou1_assets->cobjs[0]);
  GObj_AddObject(cam_gobj, 1, cam_cobj);
  GOBJ_InitCamera(cam_gobj, CObjThink, 0);

  // Indicates which gx_links to render
  cam_gobj->cobj_links = (1 << 0) + (1 << 1) + (1 << 2);

  GOBJ *light_gobj = GObj_Create(3, 4, 128);
  LOBJ *lobj = LObj_LoadAll(tou1_assets->lights[0]);
  GObj_AddObject(light_gobj, 2, lobj);
  GObj_AddGXLink(light_gobj, GXLink_LObj, 0, 128);

  JOBJ_LoadSet(0, tou1_assets->jobjs[4], 0, 0, 3, 1, 1, GObj_Anim);
}

void Minor_Think() {
  OSReport("minor think\n");
  return;
}

void Minor_Exit(void *minor_data) {
  OSReport("minor exit\n");
  return;
}

void CObjThink(GOBJ *gobj) {
  COBJ *cobj = gobj->hsd_object;

  if (!CObj_SetCurrent(cobj)) {
    return;
  }

  // HSD_Fog *fog = stc_fog;
  // CObj_SetEraseColor(fog->color.r, fog->color.g, fog->color.b, fog->color.a);
  CObj_EraseScreen(cobj, 1, 0, 1);
  CObj_RenderGXLinks(gobj, 7);
  CObj_EndCurrent();
}