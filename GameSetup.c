#include "GameSetup.h"

#include "./Components/CharStageBoxSelector.h"
#include "./Components/CharStageIcon.h"
#include "./m-ex/MexTK/mex.h"
#include "Files.h"

#define MAX_SELECTORS 10

static ArchiveInfo *gui_archive;
static GUI_GameSetup *gui_assets;
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
  GObj_AddProc(cam_gobj, MainMenu_CamRotateThink, 5);

  // Indicates which gx_links to render
  cam_gobj->cobj_links = (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4);

  // store cobj to static pointer, needed for MainMenu_CamRotateThink
  void **stc_cam_cobj = (R13 + (-0x4ADC));
  *stc_cam_cobj = gui_assets->cobjs[0];

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

  // create background
  JOBJ_LoadSet(0, gui_assets->jobjs[5], 0, 0, 3, 1, 1, GObj_Anim);

  // Load panel and frame
  JOBJ_LoadSet(0, gui_assets->jobjs[4], 0, 0, 3, 1, 1, GObj_Anim);

  // Init selectors
  InitSelectors();

  // Prepare text
  Text *txt = Text_CreateText(0, 0);
  txt->kerning = 1;
  txt->align = 0;  // align left
  txt->scale = (Vec2){0.01, 0.01};

  int id = Text_AddSubtext(txt, -2000, 0, "OTest");
  Text_SetScale(txt, id, 6, 6);

  // Load confirm/change buttons
  stc_buttons = JOBJ_LoadSet(0, gui_assets->jobjs[2], 0, 0, 3, 1, 1, GObj_Anim);

  // Test x
  // GOBJ *x_gobj = JOBJ_LoadSet(0, gui_assets->jobjs[8], 0, 0, 3, 1, 1, GObj_Anim);
  // JOBJ *x_jobj = x_gobj->hsd_object;
  // x_jobj->trans.Z = 5;
}

void InitSelectors() {
  int count = 6;
  CSIcon_Material iconMats[] = {
      CSIcon_Material_Battlefield,
      CSIcon_Material_Yoshis,
      CSIcon_Material_Dreamland,
      CSIcon_Material_FinalDestination,
      CSIcon_Material_Fountain,
      CSIcon_Material_Pokemon,
      CSIcon_Material_Falcon,
  };

  // Do nothing if there are no selectors
  if (count == 0 || count > MAX_SELECTORS) {
    return;
  }

  float gap = 8;
  float yPos = 5.5;
  float xPos = 0;

  // If odd number of items, the middle point should be in between two items
  if (count % 2 == 0) {
    xPos += gap / 2;
  }

  // Move to x pos for first item
  xPos -= gap * (int)(count / 2);

  // Initialize the box selectors
  CSBoxSelector *selectors[MAX_SELECTORS];
  for (int i = 0; i < count; i++) {
    CSBoxSelector *csbs = CSBoxSelector_Init(gui_assets);
    CSIcon_SetMaterial(csbs->icon, iconMats[i]);
    CSBoxSelector_SetPos(csbs, (Vec3){xPos, yPos, 0});

    selectors[i] = csbs;
    xPos += gap;
  }

  // Hover the first item
  CSBoxSelector_SetHover(selectors[0]);
  CSBoxSelector_SetSelectState(selectors[1], CSBoxSelector_Select_State_X);
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