#include "GameSetup.h"

#include "./m-ex/MexTK/mex.h"

static ArchiveInfo *gui_archive;
static GuiData *gui_assets;
static GOBJ *stc_buttons;

void Minor_Load(void *minor_data) {
  OSReport("minor load\n");

  // Set up input handler. Initialize at top to make sure it runs before anything else
  GOBJ *input_handler_gobj = GObj_Create(4, 0, 128);
  GObj_AddProc(input_handler_gobj, InputsThink, 0);

  // Load file
  gui_archive = File_Load("GameSetup_gui.dat");
  // gui_archive = File_Load("GmTou1p.dat");
  gui_assets = File_GetSymbol(gui_archive, "ScGamTour_scene_data");

  GOBJ *cam_gobj = GObj_Create(2, 3, 128);
  COBJ *cam_cobj = COBJ_LoadDesc(gui_assets->cobjs[0]);
  GObj_AddObject(cam_gobj, 1, cam_cobj);
  GOBJ_InitCamera(cam_gobj, CObjThink, 0);

  // Indicates which gx_links to render
  cam_gobj->cobj_links = (1 << 0) + (1 << 1) + (1 << 2);

  // create fog
  GOBJ *fog_gobj = GObj_Create(14, 2, 0);
  HSD_Fog *fog = Fog_LoadDesc(gui_assets->fog[0]);
  GObj_AddObject(fog_gobj, 4, fog);
  GObj_AddGXLink(fog_gobj, GXLink_Fog, 0, 128);

  // create lights
  GOBJ *light_gobj = GObj_Create(3, 4, 128);
  LOBJ *lobj = LObj_LoadAll(gui_assets->lights);
  GObj_AddObject(light_gobj, 2, lobj);
  GObj_AddGXLink(light_gobj, GXLink_LObj, 0, 128);

  // Load panel and frame
  JOBJ_LoadSet(0, gui_assets->jobjs[4], 0, 0, 3, 1, 1, GObj_Anim);

  // Load stage selectors
  float gap = 8;
  float xPosStart = -19.5;
  float yPos = -2;

  InitSelector(xPosStart, yPos);
  InitSelector(xPosStart + gap, yPos);
  InitSelector(xPosStart + 2 * gap, yPos);
  InitSelector(xPosStart + 3 * gap, yPos);
  InitSelector(xPosStart + 4 * gap, yPos);
  InitSelector(xPosStart + 5 * gap, yPos);

  // Load confirm/change buttons
  stc_buttons = JOBJ_LoadSet(0, gui_assets->jobjs[2], 0, 0, 3, 1, 1, GObj_Anim);

  // create background
  JOBJ_LoadSet(0, gui_assets->jobjs[5], 0, 0, 3, 1, 1, GObj_Anim);
}

GOBJ *InitSelector(float x, float y) {
  // Show cursor
  GOBJ *item = JOBJ_LoadSet(0, gui_assets->jobjs[6], 0, 0, 3, 1, 1, GObj_Anim);
  JOBJ *jobj_item = item->hsd_object;

  jobj_item->trans.X = x;
  jobj_item->trans.Y = y;
  jobj_item->scale.X = 0.727;
  jobj_item->scale.Y = 0.727;
  jobj_item->scale.Z = 0.727;
  // char foo[50];
  // sprintf(foo, "PosX: %f PosY: %f\n", jobj_item->trans.X, jobj_item->trans.Y);
  // OSReport(foo);

  // Create content material
  // item = JOBJ_LoadSet(0, gui_assets->jobjs[7], 0, 0, 3, 1, 0, 0);
  // jobj_item = item->hsd_object;

  // jobj_item->trans.X = x;
  // jobj_item->trans.Y = y;
}

void Minor_Think() {
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
  CObj_SetEraseColor(0, 0, 0, 255);
  CObj_EraseScreen(cobj, 1, 0, 1);
  CObj_RenderGXLinks(gobj, 7);
  CObj_EndCurrent();
}

static u32 frame_counter = 0;

void InputsThink(GOBJ *gobj) {
  u8 port = R13_U8(-0x5108);
  u64 scrollInputs = Pad_GetRapidHeld(port);  // long delay between initial triggers, then frequent
  u64 downInputs = Pad_GetDown(port);

  if (frame_counter == 0) {
    char prnt[50];
    sprintf(prnt, "Port: %d\n", port);
    OSReport(prnt);
  }

  JOBJ *btns_jobj = stc_buttons->hsd_object;

  // TODO: Use looping animation and only run this when switching to a new button selection
  if (frame_counter % 20 == 0) {
    // JOBJ_RemoveAnimAll(btns_jobj);
    JOBJ_AddSetAnim(btns_jobj, gui_assets->jobjs[2], 0);
    // JOBJ_RemoveAnimAll(btns_jobj->child->sibling->sibling->sibling->child);
    JOBJ_ReqAnimAll(btns_jobj->child->sibling->sibling->sibling, 0);
  }

  if (downInputs != 0) {
    char prnt[50];
    sprintf(prnt, "[%d] %d: 0x%llx\n", frame_counter, port, downInputs);
    OSReport(prnt);
  }

  if (scrollInputs & (HSD_BUTTON_RIGHT | HSD_BUTTON_DPAD_RIGHT)) {
    // Handle a right input
    SFX_PlayCommon(2);  // Play move SFX
  } else if (scrollInputs & (HSD_BUTTON_LEFT | HSD_BUTTON_DPAD_RIGHT)) {
    // Handle a left input
    SFX_PlayCommon(2);  // Play move SFX
  }

  frame_counter++;
}