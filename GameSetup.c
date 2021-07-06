#include "GameSetup.h"

#include "./Components/Button.h"
#include "./Components/CharStageBoxSelector.h"
#include "./Components/CharStageIcon.h"
#include "./Components/StockIcon.h"
#include "./m-ex/MexTK/mex.h"
#include "Files.h"

static ArchiveInfo *gui_archive;
static GUI_GameSetup *gui_assets;
static GameSetup_Data *data;

void Minor_Load(GameSetup_SceneData *minor_data) {
  OSReport("minor load\n");

  data = calloc(sizeof(GameSetup_Data));
  data->scene_data = minor_data;

  // If this is a tiebreak, just set match selections and terminate scene to go back into a game
  if (minor_data->is_tiebreak) {
    data->process_type = GameSetup_Process_Type_SKIP;
    data->state.is_complete = true;
    data->state.should_terminate = true;

    u8 optionNone = ExiSlippi_SelectionOption_UNSET;
    SetMatchSelections(0, 0, optionNone, 0, optionNone);

    return;
  }

  // Init location to store data
  if (minor_data->cur_game == 1) {
    data->process_type = GameSetup_Process_Type_STAGE_STRIKING;
  } else {
    data->process_type = GameSetup_Process_Type_COUNTERPICKING;
  }

  // Allocate some memory used throughout
  data->match_state = ExiSlippi_LoadMatchState(0);  // Fetch initial match state
  data->fetch_query = calloc(sizeof(ExiSlippi_FetchStep_Query));
  data->fetch_resp = calloc(sizeof(ExiSlippi_FetchStep_Response));
  data->complete_query = calloc(sizeof(ExiSlippi_CompleteStep_Query));

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
  JOBJ_LoadSet(0, gui_assets->jobjs[GUI_GameSetup_JOBJ_Background], 0, 0, 3, 1, 1, GObj_Anim);

  // Load panel and frame
  JOBJ_LoadSet(0, gui_assets->jobjs[GUI_GameSetup_JOBJ_Panels], 0, 0, 3, 1, 1, GObj_Anim);

  // Init selectors
  InitAllSelectorJobjs();

  // Prepare text
  data->text = Text_CreateText(0, 0);
  data->text->kerning = 1;
  data->text->align = 1;  // align center
  data->text->use_aspect = 1;
  data->text->scale = (Vec2){0.01, 0.01};

  data->timer_subtext_id = Text_AddSubtext(data->text, 0, -1880, "0:30");
  Text_SetScale(data->text, data->timer_subtext_id, 6, 6);

  // Load confirm/change buttons
  data->buttons[0] = Button_Init(gui_assets);
  data->buttons[1] = Button_Init(gui_assets);
  Button_SetPos(data->buttons[0], (Vec3){-4.8, -3, 0});
  Button_SetPos(data->buttons[1], (Vec3){4.8, -3, 0});
  Button_SetMaterial(data->buttons[0], Button_Material_Ok);
  Button_SetMaterial(data->buttons[1], Button_Material_Redo);
  data->button_count = 2;

  // Initialize description message
  float desc_width_half = 45 / 2;
  float desc_height_half = 1.8 / 2;
  data->description = FlatTexture_Init(gui_assets);
  Vec3 tl = {-desc_width_half, desc_height_half, 0};
  Vec3 tr = {desc_width_half, desc_height_half, 0};
  Vec3 bl = {-desc_width_half, -desc_height_half, 0};
  Vec3 br = {desc_width_half, -desc_height_half, 0};
  FlatTexture_SetPosCorners(data->description, tl, tr, bl, br);
  FlatTexture_SetPos(data->description, (Vec3){0, -3, 0});

  // Init steps
  InitSteps();

  // Initialize dialog last to make sure it's on top of everything
  data->char_picker_dialog = CharPickerDialog_Init(gui_assets, OnCharSelectionComplete, GetNextColor);
  CharPickerDialog_SetPos(data->char_picker_dialog, (Vec3){0, -9, 0});

  // Prepare the state and displays
  InitState();
  PrepareCurrentStep();
  UpdateTimeline();
}

void Minor_Think() {
  // If current step is completed, process finished
  // TODO: Add some kind of delay/display to indicate which stage was selected
  if (data->state.should_terminate) {
    ExiSlippi_LoadMatchState(data->match_state);

    // Don't exit the scene until both players are ready (needed to make sure RNG is synced)
    if (!data->match_state->is_local_player_ready || !data->match_state->is_remote_player_ready) {
      return;
    }

    // TODO: Play an animation on selected stage and play a sound
    Scene_ExitMinor();
  }
}

void Minor_Exit(GameSetup_SceneData *minor_data) {
  OSReport("minor exit\n");
}

void InitState() {
  data->state.selector_idx = -1;  // Start with nothing selected, will update for the selector
  data->state.selected_values_count = 0;
}

static FlatTexture *InitStepLabel(CSIcon *icon, FlatTexture_Texture tex) {
  FlatTexture *label = FlatTexture_Init(gui_assets);
  FlatTexture_SetTexture(label, tex);
  JOBJ_AttachPosition(label->root_jobj, icon->root_jobj);
  return label;
}

static RightArrow *InitStepArrow(CSIcon *icon) {
  RightArrow *arrow = RightArrow_Init(gui_assets);
  JOBJ_AttachPosition(arrow->root_jobj, icon->root_jobj);
  return arrow;
}

void InitStrikingSteps() {
  ExiSlippi_MatchState_Response *match_state = data->match_state;

  data->step_count = 5;
  data->steps = calloc(data->step_count * sizeof(GameSetup_Step));
  data->state.step_idx = 0;

  u8 isFirst = match_state->local_player_idx == 0;

  FlatTexture_Texture labelYourChar = FlatTexture_Texture_YOUR_CHAR_LABEL;
  FlatTexture_Texture labelOppChar = FlatTexture_Texture_OPP_CHAR_LABEL;
  FlatTexture_Texture descSelectStrike = FlatTexture_Texture_SELECT_STRIKE_DESC;
  FlatTexture_Texture descSelectTwoStrikes = FlatTexture_Texture_SELECT_TWO_STRIKES_DESC;
  FlatTexture_Texture descWaitStrike = FlatTexture_Texture_WAIT_STRIKE_DESC;

  data->steps[0].player_idx = 0;
  data->steps[0].type = GameSetup_Step_Type_CHOOSE_CHAR;
  data->steps[0].required_selection_count = 1;
  data->steps[0].char_selection = match_state->game_info_block[0x60];
  data->steps[0].char_color_selection = match_state->game_info_block[0x63];
  data->steps[0].timer_seconds = 45;
  data->steps[0].selectors = isFirst ? data->char_selectors : data->char_wait_selectors;
  data->steps[0].selector_count = 1;
  data->steps[0].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[0].desc_tex = FlatTexture_Texture_SELECT_CHAR_DESC;
  data->steps[0].label = InitStepLabel(data->steps[0].display_icons[0], isFirst ? labelYourChar : labelOppChar);
  data->steps[0].arrow = 0;

  data->steps[1].player_idx = 1;
  data->steps[1].type = GameSetup_Step_Type_CHOOSE_CHAR;
  data->steps[1].required_selection_count = 1;
  data->steps[1].char_selection = match_state->game_info_block[0x60 + 0x24];
  data->steps[1].char_color_selection = match_state->game_info_block[0x63 + 0x24];
  data->steps[1].timer_seconds = 45;
  data->steps[1].selectors = isFirst ? data->char_wait_selectors : data->char_selectors;
  data->steps[1].selector_count = 1;
  data->steps[1].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[1].desc_tex = FlatTexture_Texture_SELECT_CHAR_DESC;
  data->steps[1].label = InitStepLabel(data->steps[1].display_icons[0], isFirst ? labelOppChar : labelYourChar);
  data->steps[1].arrow = InitStepArrow(data->steps[1].display_icons[0]);

  data->steps[2].player_idx = 0;
  data->steps[2].type = GameSetup_Step_Type_REMOVE_STAGE;
  data->steps[2].required_selection_count = 1;
  data->steps[2].timer_seconds = 30;
  data->steps[2].selectors = data->stage_strike_selectors;
  data->steps[2].selector_count = STRIKE_STAGE_SELECTOR_COUNT;
  data->steps[2].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[2].desc_tex = isFirst ? descSelectStrike : descWaitStrike;
  data->steps[2].label = InitStepLabel(data->steps[2].display_icons[0], FlatTexture_Texture_STRIKE1_LABEL);
  data->steps[2].arrow = InitStepArrow(data->steps[2].display_icons[0]);

  data->steps[3].player_idx = 1;
  data->steps[3].type = GameSetup_Step_Type_REMOVE_STAGE;
  data->steps[3].required_selection_count = 2;
  data->steps[3].timer_seconds = 30;
  data->steps[3].selectors = data->stage_strike_selectors;
  data->steps[3].selector_count = STRIKE_STAGE_SELECTOR_COUNT;
  data->steps[3].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[3].display_icons[1] = CSIcon_Init(gui_assets);
  data->steps[3].desc_tex = isFirst ? descWaitStrike : descSelectTwoStrikes;
  data->steps[3].label = InitStepLabel(data->steps[3].display_icons[0], FlatTexture_Texture_STRIKE23_LABEL);
  data->steps[3].arrow = InitStepArrow(data->steps[3].display_icons[0]);

  data->steps[4].player_idx = 0;
  data->steps[4].type = GameSetup_Step_Type_REMOVE_STAGE;
  data->steps[4].required_selection_count = 1;
  data->steps[4].timer_seconds = 10;
  data->steps[4].selectors = data->stage_strike_selectors;
  data->steps[4].selector_count = STRIKE_STAGE_SELECTOR_COUNT;
  data->steps[4].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[4].desc_tex = isFirst ? descSelectStrike : descWaitStrike;
  data->steps[4].label = InitStepLabel(data->steps[4].display_icons[0], FlatTexture_Texture_STRIKE4_LABEL);
  data->steps[4].arrow = InitStepArrow(data->steps[4].display_icons[0]);

  // Start with the first two steps completed
  CompleteCurrentStep(data->steps[0].required_selection_count);
  CompleteCurrentStep(data->steps[1].required_selection_count);
}

void InitCounterpickingSteps() {
  ExiSlippi_MatchState_Response *match_state = data->match_state;
  GameSetup_SceneData *scene_data = data->scene_data;

  data->step_count = 4;
  data->steps = calloc(data->step_count * sizeof(GameSetup_Step));
  data->state.step_idx = 0;

  u8 isWinner = match_state->local_player_idx == scene_data->prev_winner;

  FlatTexture_Texture labelYourChar = FlatTexture_Texture_YOUR_CHAR_LABEL;
  FlatTexture_Texture labelOppChar = FlatTexture_Texture_OPP_CHAR_LABEL;
  FlatTexture_Texture descSelectChar = FlatTexture_Texture_SELECT_CHAR_DESC;
  FlatTexture_Texture descWaitChar = FlatTexture_Texture_WAIT_CHAR_DESC;
  FlatTexture_Texture descSelectBan = FlatTexture_Texture_SELECT_BAN_DESC;
  FlatTexture_Texture descWaitBan = FlatTexture_Texture_WAIT_BAN_DESC;
  FlatTexture_Texture descSelectStage = FlatTexture_Texture_SELECT_STAGE_DESC;
  FlatTexture_Texture descWaitStage = FlatTexture_Texture_WAIT_STAGE_DESC;

  data->steps[0].player_idx = scene_data->prev_winner;
  data->steps[0].type = GameSetup_Step_Type_REMOVE_STAGE;
  data->steps[0].required_selection_count = 1;
  data->steps[0].timer_seconds = 30;
  data->steps[0].selectors = data->stage_cp_selectors;
  data->steps[0].selector_count = CP_STAGE_SELECTOR_COUNT;
  data->steps[0].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[0].desc_tex = isWinner ? descSelectBan : descWaitBan;
  data->steps[0].label = InitStepLabel(data->steps[0].display_icons[0], FlatTexture_Texture_BAN_LABEL);
  data->steps[0].arrow = 0;

  data->steps[1].player_idx = !scene_data->prev_winner;
  data->steps[1].type = GameSetup_Step_Type_CHOOSE_STAGE;
  data->steps[1].required_selection_count = 1;
  data->steps[1].timer_seconds = 30;
  data->steps[1].selectors = data->stage_cp_selectors;
  data->steps[1].selector_count = CP_STAGE_SELECTOR_COUNT;
  data->steps[1].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[1].desc_tex = isWinner ? descWaitStage : descSelectStage;
  data->steps[1].label = InitStepLabel(data->steps[1].display_icons[0], FlatTexture_Texture_STAGE_LABEL);
  data->steps[1].arrow = InitStepArrow(data->steps[1].display_icons[0]);

  data->steps[2].player_idx = scene_data->prev_winner;
  data->steps[2].type = GameSetup_Step_Type_CHOOSE_CHAR;
  data->steps[2].required_selection_count = 1;
  data->steps[2].timer_seconds = 45;
  data->steps[2].selectors = isWinner ? data->char_selectors : data->char_wait_selectors;
  data->steps[2].selector_count = 1;
  data->steps[2].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[2].desc_tex = isWinner ? descSelectChar : descWaitChar;
  data->steps[2].label = InitStepLabel(data->steps[2].display_icons[0], isWinner ? labelYourChar : labelOppChar);
  data->steps[2].arrow = InitStepArrow(data->steps[2].display_icons[0]);

  data->steps[3].player_idx = !scene_data->prev_winner;
  data->steps[3].type = GameSetup_Step_Type_CHOOSE_CHAR;
  data->steps[3].required_selection_count = 1;
  data->steps[3].timer_seconds = 2000;
  data->steps[3].selectors = isWinner ? data->char_wait_selectors : data->char_selectors;
  data->steps[3].selector_count = 1;
  data->steps[3].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[3].desc_tex = isWinner ? descWaitChar : descSelectChar;
  data->steps[3].label = InitStepLabel(data->steps[3].display_icons[0], isWinner ? labelOppChar : labelYourChar);
  data->steps[3].arrow = InitStepArrow(data->steps[3].display_icons[0]);
}

void InitSteps() {
  switch (data->process_type) {
    case GameSetup_Process_Type_STAGE_STRIKING:
      InitStrikingSteps();
      break;
    case GameSetup_Process_Type_COUNTERPICKING:
      InitCounterpickingSteps();
      break;
  }
}

void InitSelectorJobjs(CSIcon_Material *iconMats, CSBoxSelector **selectors, int count) {
  float gap = 8;
  float yPos = 5;
  float xPos = 0;

  // If odd number of items, the middle point should be in between two items
  if (count % 2 == 0) {
    xPos += gap / 2;
  }

  // Move to x pos for first item
  xPos -= gap * (int)(count / 2);

  // Initialize the box selectors
  for (int i = 0; i < count; i++) {
    CSBoxSelector *csbs = CSBoxSelector_Init(gui_assets);
    CSIcon_SetMaterial(csbs->icon, iconMats[i]);
    CSBoxSelector_SetPos(csbs, (Vec3){xPos, yPos, 0});
    CSBoxSelector_SetVisibility(csbs, false);

    selectors[i] = csbs;
    xPos += gap;
  }
}

void InitAllSelectorJobjs() {
  ExiSlippi_MatchState_Response *match_state = data->match_state;

  if (data->process_type == GameSetup_Process_Type_STAGE_STRIKING) {
    CSIcon_Material stageStrikeMats[] = {
        CSIcon_Material_Fountain,
        CSIcon_Material_Dreamland,
        CSIcon_Material_Yoshis,
        CSIcon_Material_Pokemon,
        CSIcon_Material_Battlefield,
    };
    InitSelectorJobjs(stageStrikeMats, data->stage_strike_selectors, STRIKE_STAGE_SELECTOR_COUNT);
  }

  if (data->process_type == GameSetup_Process_Type_COUNTERPICKING) {
    CSIcon_Material stageCpMats[] = {
        CSIcon_Material_Fountain,
        CSIcon_Material_Dreamland,
        CSIcon_Material_Yoshis,
        CSIcon_Material_Pokemon,
        CSIcon_Material_Battlefield,
        CSIcon_Material_FinalDestination,
    };
    InitSelectorJobjs(stageCpMats, data->stage_cp_selectors, CP_STAGE_SELECTOR_COUNT);
  }

  CSIcon_Material unknownCharMats[] = {CSIcon_Material_Question};
  InitSelectorJobjs(unknownCharMats, data->char_wait_selectors, 1);

  u8 charId = match_state->game_info_block[0x60 + match_state->local_player_idx * 0x24];
  u8 colorId = match_state->game_info_block[0x63 + match_state->local_player_idx * 0x24];
  CSIcon_Material playerCharMats[] = {CSIcon_ConvertCharToMat(charId)};
  InitSelectorJobjs(playerCharMats, data->char_selectors, 1);
  CSIcon_SetStockIconVisibility(data->char_selectors[0]->icon, true);
  StockIcon_SetIcon(data->char_selectors[0]->icon->stock_icon, charId, colorId);
}

void ResetButtonState(u8 is_visible) {
  for (int i = 0; i < data->button_count; i++) {
    Button_SetHover(data->buttons[i], false);
    Button_SetVisibility(data->buttons[i], is_visible);
  }

  data->state.btn_hover_idx = 0;

  // If buttons are visible, set hover state to the first button
  if (is_visible) {
    Button_SetHover(data->buttons[0], true);
  }

  // Description text visibility should be opposite of button visibility
  FlatTexture_SetVisibility(data->description, !is_visible);
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

void UpdateButtonHoverPos(u64 scrollInputs) {
  ////////////////////////////////
  // Adjust button hover
  ////////////////////////////////
  if (scrollInputs & (HSD_BUTTON_RIGHT | HSD_BUTTON_DPAD_RIGHT)) {
    // Handle a right input
    data->state.btn_hover_idx++;
    SFX_PlayCommon(2);  // Play move SFX
  } else if (scrollInputs & (HSD_BUTTON_LEFT | HSD_BUTTON_DPAD_LEFT)) {
    // Handle a left input
    data->state.btn_hover_idx--;
    SFX_PlayCommon(2);  // Play move SFX
  }

  if (data->state.btn_hover_idx < 0) {
    data->state.btn_hover_idx += data->button_count;
  } else if (data->state.btn_hover_idx >= data->button_count) {
    data->state.btn_hover_idx -= data->button_count;
  }
}

void HandleStageInputs(GameSetup_Step *step) {
  u8 port = R13_U8(-0x5108);
  u64 scrollInputs = Pad_GetRapidHeld(port);  // long delay between initial triggers, then frequent
  u64 downInputs = Pad_GetDown(port);

  if (data->state.selector_idx >= 0) {
    ////////////////////////////////
    // Adjust scroll position
    ////////////////////////////////
    if (scrollInputs & (HSD_BUTTON_RIGHT | HSD_BUTTON_DPAD_RIGHT)) {
      // Handle a right input
      IncrementSelectorIndex();
      SFX_PlayCommon(2);  // Play move SFX
    } else if (scrollInputs & (HSD_BUTTON_LEFT | HSD_BUTTON_DPAD_LEFT)) {
      // Handle a left input
      DecrementSelectorIndex();
      SFX_PlayCommon(2);  // Play move SFX
    }

    ////////////////////////////////
    // Handle confirmation button press
    ////////////////////////////////
    // TODO: Handle buttons before scroll, don't scroll on the same frame a button is pressed
    if (downInputs & HSD_BUTTON_A) {
      int idx = data->state.selector_idx;
      u8 isSelected = step->selectors[idx]->state.is_selected;

      if (isSelected) {
        CSBoxSelector_SetSelectState(step->selectors[idx], CSBoxSelector_Select_State_NotSelected);
        data->state.selected_values_count--;
      } else {
        CSBoxSelector_Select_State sel_state = CSBoxSelector_Select_State_Selected_X;
        if (step->type == GameSetup_Step_Type_CHOOSE_STAGE) {
          sel_state = sel_state = CSBoxSelector_Select_State_Selected;
        }
        CSBoxSelector_SetSelectState(step->selectors[idx], sel_state);
        data->state.selected_values_count++;
      }

      // TODO: Pick better sound?
      SFX_PlayCommon(2);
    }

    // Check condition to see if selections are complete
    if (data->state.selected_values_count >= step->required_selection_count) {
      // Clear selection index
      data->state.prev_selector_idx = data->state.selector_idx;
      data->state.selector_idx = -1;

      ResetButtonState(true);
    }
  } else {
    // Takes scroll inputs and updates button selection state position
    UpdateButtonHoverPos(scrollInputs);

    // TODO: Handle buttons before scroll, don't scroll on the same frame a button is pressed
    if (downInputs & HSD_BUTTON_B || (downInputs & HSD_BUTTON_A && data->state.btn_hover_idx == 1)) {
      data->state.selector_idx = data->state.prev_selector_idx;
      data->state.selected_values_count = 0;
      for (int i = 0; i < step->selector_count; i++) {
        // Only reset selectors that are currently selected
        if (!step->selectors[i]->state.is_selected) {
          continue;
        }

        CSBoxSelector_SetSelectState(step->selectors[i], CSBoxSelector_Select_State_NotSelected);
      }

      // Hide buttons
      ResetButtonState(false);

      SFX_PlayCommon(2);
    } else if (downInputs & HSD_BUTTON_A && data->state.btn_hover_idx == 0) {
      SFX_PlayCommon(1);
      CompleteCurrentStep(0);
      PrepareCurrentStep();
      UpdateTimeline();
    }
  }

  // Update selector position
  for (int i = 0; i < step->selector_count; i++) {
    // Set hover state. Won't do anything if already set to that state
    CSBoxSelector_SetHover(step->selectors[i], data->state.selector_idx == i);
  }

  // Update button hover position
  for (int i = 0; i < data->button_count; i++) {
    // Set hover state. Won't do anything if already set to that state
    Button_SetHover(data->buttons[i], data->state.btn_hover_idx == i);
  }
}

void HandleCharacterInputs(GameSetup_Step *step) {
  u8 port = R13_U8(-0x5108);
  u64 scrollInputs = Pad_GetRapidHeld(port);  // long delay between initial triggers, then frequent
  u64 downInputs = Pad_GetDown(port);

  // If char picker dialog is open, don't process inputs here. The callback function
  // OnCharSelectionComplete will be used to return to the button state
  if (!data->char_picker_dialog->state.is_open) {
    // Takes scroll inputs and updates button selection state position
    UpdateButtonHoverPos(scrollInputs);

    // TODO: Handle buttons before scroll, don't scroll on the same frame a button is pressed
    if (downInputs & HSD_BUTTON_B || (downInputs & HSD_BUTTON_A && data->state.btn_hover_idx == 1)) {
      u8 charId = CSIcon_ConvertMatToChar(step->selectors[0]->icon->state.material);
      u8 charColor = step->selectors[0]->icon->stock_icon->state.color_id;
      CharPickerDialog_OpenDialog(data->char_picker_dialog, charId, charColor);

      // Hide buttons
      ResetButtonState(false);

      SFX_PlayCommon(2);
    } else if (downInputs & HSD_BUTTON_A && data->state.btn_hover_idx == 0) {
      SFX_PlayCommon(1);
      CompleteCurrentStep(0);
      PrepareCurrentStep();
      UpdateTimeline();
    }
  }

  // Update button hover position
  for (int i = 0; i < data->button_count; i++) {
    // Set hover state. Won't do anything if already set to that state
    Button_SetHover(data->buttons[i], data->state.btn_hover_idx == i);
  }
}

void InputsThink(GOBJ *gobj) {
  GameSetup_Step *step = &data->steps[data->state.step_idx];

  // If current step is completed (process finished, don't allow any inputs)
  if (step->state == GameSetup_Step_State_COMPLETE) {
    // TODO: Play an animation on selected stage and play a sound
    return;
  }

  u8 is_time_elapsed = UpdateTimer();

  // Check if this step is player controlled or if we are waiting for the opponent
  if (step->player_idx != data->match_state->local_player_idx) {
    HandleOpponentStep();
    return;
  }

  // If this step is player controlled and time is elapsed, complete the step
  if (is_time_elapsed) {
    SFX_PlayCommon(1);
    CompleteCurrentStep(0);
    PrepareCurrentStep();
    UpdateTimeline();
    return;
  }

  switch (step->type) {
    case GameSetup_Step_Type_CHOOSE_STAGE:
    case GameSetup_Step_Type_REMOVE_STAGE:
      HandleStageInputs(step);
      break;
    case GameSetup_Step_Type_CHOOSE_CHAR:
      HandleCharacterInputs(step);
      break;
  }
}

void HandleOpponentStep() {
  GameSetup_Step *step = &data->steps[data->state.step_idx];

  // Check if we've timed out waiting for opponent
  if (data->timer_frames > 60 * (step->timer_seconds + GRACE_SECONDS + WAIT_TIMEOUT_SECONDS)) {
    // TODO: Abandon match
    return;
  }

  data->fetch_query->command = ExiSlippi_Command_GP_FETCH_STEP;
  data->fetch_query->step_idx = data->state.step_idx;
  ExiSlippi_Transfer(data->fetch_query, sizeof(ExiSlippi_FetchStep_Query), ExiSlippi_TransferMode_WRITE);
  ExiSlippi_Transfer(data->fetch_resp, sizeof(ExiSlippi_FetchStep_Response), ExiSlippi_TransferMode_READ);

  // If no response is found, do nothing
  if (!data->fetch_resp->is_found) {
    return;
  }

  // If we receive a response from opponent, populate the step values
  step->char_selection = data->fetch_resp->char_selection;
  step->char_color_selection = data->fetch_resp->char_color_selection;
  memcpy(step->stage_selections, data->fetch_resp->stage_selections, 2);

  // Complete the step. Indicate that all selections are already made
  SFX_PlayCommon(1);
  CompleteCurrentStep(data->fetch_resp->is_skip ? 0 : step->required_selection_count);
  PrepareCurrentStep();
  UpdateTimeline();
}

void CompleteCurrentStep(int committed_count) {
  // Get and complete current step
  GameSetup_Step *step = &data->steps[data->state.step_idx];
  step->state = GameSetup_Step_State_COMPLETE;

  int commit_index = committed_count;

  if (step->type == GameSetup_Step_Type_CHOOSE_STAGE || step->type == GameSetup_Step_Type_REMOVE_STAGE) {
    // Commit selections to selected values
    for (int i = 0; i < step->selector_count; i++) {
      CSBoxSelector *bs = step->selectors[i];

      // If this selector is selected, indicate that stage as the one selected
      if (bs->state.is_selected) {
        int stageId = CSIcon_ConvertMatToStage(bs->icon->state.material);
        step->stage_selections[commit_index] = stageId;
        commit_index++;
      }
    }

    // In the case of a timeout, the selections may not have been made. Select non-disabled
    // stages
    for (int i = 0; i < step->selector_count; i++) {
      if (commit_index >= step->required_selection_count) {
        break;  // If enough selections have been made, exit loop
      }

      CSBoxSelector *bs = step->selectors[i];

      // If this selector is selected, indicate that stage as the one selected
      if (!bs->state.is_disabled) {
        int stageId = CSIcon_ConvertMatToStage(bs->icon->state.material);
        step->stage_selections[commit_index] = stageId;
        commit_index++;
      }
    }
  } else if (step->type == GameSetup_Step_Type_CHOOSE_CHAR && committed_count == 0) {
    // Default color and char to what was selected last game in case of a timeout with no selection.
    // We don't need to worry about that color not being allowed because the opponent shouldn't
    // be allowed to take it
    int playerIdx = step->player_idx;
    int charId = data->match_state->game_info_block[0x60 + 0x24 * playerIdx];
    int charColorId = data->match_state->game_info_block[0x63 + 0x24 * playerIdx];

    if (step->selectors[0]->icon->state.material != CSIcon_Material_Question) {
      charId = CSIcon_ConvertMatToChar(step->selectors[0]->icon->state.material);
      charColorId = step->selectors[0]->icon->stock_icon->state.color_id;
    }

    step->char_selection = charId;
    step->char_color_selection = charColorId;
  }

  // If the current player is in control of this step, send a message to opponent
  if (step->player_idx == data->match_state->local_player_idx) {
    data->complete_query->command = ExiSlippi_Command_GP_COMPLETE_STEP;
    data->complete_query->step_idx = data->state.step_idx;
    data->complete_query->char_selection = step->char_selection;
    data->complete_query->char_color_selection = step->char_color_selection;
    memcpy(data->complete_query->stage_selections, step->stage_selections, 2);
    ExiSlippi_Transfer(data->complete_query, sizeof(ExiSlippi_CompleteStep_Query), ExiSlippi_TransferMode_WRITE);
  }

  // Hide current selectors, the correct selectors will be made visible in Prepare function
  for (int i = 0; i < step->selector_count; i++) {
    CSBoxSelector *bs = step->selectors[i];
    CSBoxSelector_SetVisibility(bs, false);
  }

  // Increment step idx
  data->state.step_idx++;
  if (data->state.step_idx >= data->step_count) {
    // Complete full process and end scene
    CompleteGamePrep();
    data->state.step_idx = data->step_count - 1;
    return;
  } else {
    // Activate next step
    step = &data->steps[data->state.step_idx];
    step->state = GameSetup_Step_State_ACTIVE;
  }
}

void CompleteGamePrep() {
  ExiSlippi_OverwriteSelections_Query *osq = calloc(sizeof(ExiSlippi_OverwriteSelections_Query));

  int stages[] = {
      GRKINDEXT_IZUMI,
      GRKINDEXT_OLDPU,
      GRKINDEXT_STORY,
      GRKINDEXT_PSTAD,
      GRKINDEXT_BATTLE,
  };

  u8 localCharId;
  u8 localCharColor;

  for (int i = 0; i < data->step_count; i++) {
    GameSetup_Step *step = &data->steps[i];
    if (step->type != GameSetup_Step_Type_REMOVE_STAGE) {
      continue;
    }

    for (int j = 0; j < 5; j++) {
      if (step->stage_selections[0] == stages[j]) {
        stages[j] = -1;
      } else if (step->required_selection_count > 1 && step->stage_selections[1] == stages[j]) {
        stages[j] = -1;
      } else if (stages[j] >= 0) {
        osq->stage_id = (u16)stages[j];
      }
    }
  }

  for (int i = 0; i < data->step_count; i++) {
    GameSetup_Step *step = &data->steps[i];
    if (step->type == GameSetup_Step_Type_CHOOSE_STAGE) {
      osq->stage_id = step->stage_selections[0];
      break;
    }
  }

  for (int i = 0; i < data->step_count; i++) {
    GameSetup_Step *step = &data->steps[i];
    if (step->type != GameSetup_Step_Type_CHOOSE_CHAR) {
      continue;
    }

    if (step->player_idx == data->match_state->local_player_idx) {
      localCharId = step->char_selection;
      localCharColor = step->char_color_selection;
    }

    osq->chars[step->player_idx].is_set = true;
    osq->chars[step->player_idx].char_id = step->char_selection;
    osq->chars[step->player_idx].char_color_id = step->char_color_selection;
  }

  // We overwrite selections to make sure remote player can't cheat their selections
  osq->command = ExiSlippi_Command_OVERWRITE_SELECTIONS;
  ExiSlippi_Transfer(osq, sizeof(ExiSlippi_OverwriteSelections_Query), ExiSlippi_TransferMode_WRITE);

  u8 mergeOption = ExiSlippi_SelectionOption_MERGE;
  SetMatchSelections(localCharId, localCharColor, mergeOption, osq->stage_id, mergeOption);

  // is complete and should terminate could be used to show some kind of final animation
  data->state.is_complete = true;
  data->state.should_terminate = true;
}

void SetMatchSelections(u8 char_id, u8 char_color, u8 char_option, u16 stage_id, u8 stage_option) {
  ExiSlippi_SetSelections_Query *ssq = calloc(sizeof(ExiSlippi_SetSelections_Query));
  ssq->command = ExiSlippi_Command_SET_MATCH_SELECTIONS;
  ssq->team_id = 0;
  ssq->char_id = char_id;
  ssq->char_color_id = char_color;
  ssq->char_option = char_option;
  ssq->stage_id = stage_id;
  ssq->stage_option = stage_option;
  ssq->online_mode = 0;  // Ranked. Doesn't actually do anything in Dolphin atm
  ExiSlippi_Transfer(ssq, sizeof(ExiSlippi_SetSelections_Query), ExiSlippi_TransferMode_WRITE);
}

void PrepareStageStep(GameSetup_Step *step) {
  // Disable stages that cannot be selected and move selector index
  // Pokemon is the last stage, make array large enough to fit a bool for all of them
  u8 shouldDisableMat[CSIcon_LAST_STAGE_MAT_IDX + 1] = {false};
  for (int i = 0; i < data->step_count; i++) {
    GameSetup_Step *s = &data->steps[i];
    if (s->state != GameSetup_Step_State_COMPLETE) {
      // If we reach a step that is not complete, stop looking for disabled mats
      break;
    }

    if (s->type != GameSetup_Step_Type_REMOVE_STAGE) {
      continue;
    }

    // This is a remove stage step, grab the result and mark those stages as removed
    for (int j = 0; j < s->required_selection_count; j++) {
      u8 matIdx = CSIcon_ConvertStageToMat(s->stage_selections[j]);
      shouldDisableMat[matIdx] = true;
    }
  }

  // Go through selectors and mark the proper state
  for (int i = 0; i < step->selector_count; i++) {
    CSBoxSelector *csbs = step->selectors[i];

    CSBoxSelector_Select_State newSelectState = CSBoxSelector_Select_State_NotSelected;
    if (shouldDisableMat[csbs->icon->state.material]) {
      newSelectState = CSBoxSelector_Select_State_Disabled_X;
    }

    CSBoxSelector_SetSelectState(csbs, newSelectState);
  }

  if (step->player_idx == data->match_state->local_player_idx) {
    // Reset selector index to choose first non-disabled icon
    data->state.selected_values_count = 0;
    ResetSelectorIndex();
  } else {
    data->state.selector_idx = -1;
  }

  // Clear button state
  ResetButtonState(false);
}

void PrepareCharacterStep(GameSetup_Step *step) {
  // We don't use the selector on char steps
  data->state.selector_idx = -1;

  // Buttons are visible if we are the player picking a character
  u8 buttons_visible = step->player_idx == data->match_state->local_player_idx;
  ResetButtonState(buttons_visible);

  // Ensure dialog starts hidden
  CharPickerDialog_CloseDialog(data->char_picker_dialog);
}

void PrepareCurrentStep() {
  // Do nothing if step outside of bounds
  if (data->state.step_idx >= data->step_count) {
    return;
  }

  GameSetup_Step *step = &data->steps[data->state.step_idx];

  // Initialize current timer to 0
  data->timer_frames = 0;

  // Change description
  FlatTexture_SetTexture(data->description, step->desc_tex);

  // Show current selectors
  for (int i = 0; i < step->selector_count; i++) {
    CSBoxSelector *bs = step->selectors[i];
    CSBoxSelector_SetVisibility(bs, true);
  }

  switch (step->type) {
    case GameSetup_Step_Type_CHOOSE_STAGE:
    case GameSetup_Step_Type_REMOVE_STAGE:
      PrepareStageStep(step);
      break;
    case GameSetup_Step_Type_CHOOSE_CHAR:
      PrepareCharacterStep(step);
      break;
  }
}

void UpdateTimeline() {
  float xPos = -26;
  float yPos = -17.5;

  float gap = 10;
  float double_inc = 2.6;

  float labelX = 6;
  float labelY = 0.6;
  float labelGapY = 3.2;  // Distance above the icon

  // Iterate through all the steps
  for (int i = 0; i < data->step_count; i++) {
    GameSetup_Step *step = &data->steps[i];

    // Set position of icons
    float double_ofst = 0;
    if (step->required_selection_count == 2) {
      xPos += double_inc;
      CSIcon_SetPos(step->display_icons[0], (Vec3){xPos - double_inc, yPos, 0});
      CSIcon_SetPos(step->display_icons[1], (Vec3){xPos + double_inc, yPos, 0});
      double_ofst = double_inc;
    } else {
      CSIcon_SetPos(step->display_icons[0], (Vec3){xPos, yPos, 0});
    }

    CSIcon_Select_State icon_ss = CSIcon_Select_State_NotSelected;
    RightArrow_Display_State ra_ss = RightArrow_Display_State_NORMAL;

    if (i > data->state.step_idx) {
      icon_ss = CSIcon_Select_State_Disabled;
      ra_ss = RightArrow_Display_State_DIM;
    } else if (step->state == GameSetup_Step_State_ACTIVE) {
      icon_ss = CSIcon_Select_State_Blink;
      ra_ss = RightArrow_Display_State_BLINK;
    }

    for (int j = 0; j < step->required_selection_count; j++) {
      CSIcon_SetStockIconVisibility(step->display_icons[j], false);

      // Show question if not complete, result if complete
      CSIcon_Material mat = CSIcon_Material_Question;
      if (step->state == GameSetup_Step_State_COMPLETE) {
        if (step->type == GameSetup_Step_Type_CHOOSE_CHAR) {
          mat = CSIcon_ConvertCharToMat(step->char_selection);

          // Show stock icon with proper color
          CSIcon_SetStockIconVisibility(step->display_icons[j], true);
          StockIcon_SetIcon(step->display_icons[j]->stock_icon, step->char_selection, step->char_color_selection);
        } else {
          mat = CSIcon_ConvertStageToMat(step->stage_selections[j]);
        }
      }

      CSIcon_SetMaterial(step->display_icons[j], mat);
      CSIcon_SetSelectState(step->display_icons[j], icon_ss);
    }

    // Position text and set label texture
    Vec3 tl = {-labelX + double_ofst, labelY + labelGapY, 0};
    Vec3 tr = {labelX + double_ofst, labelY + labelGapY, 0};
    Vec3 bl = {-labelX + double_ofst, -labelY + labelGapY, 0};
    Vec3 br = {labelX + double_ofst, -labelY + labelGapY, 0};
    FlatTexture_SetPosCorners(step->label, tl, tr, bl, br);

    // Set offset position for arrow
    if (step->arrow != 0) {
      RightArrow_SetOffsetPos(step->arrow, (Vec3){-gap / 2, 0, 0});
      RightArrow_SetDisplayState(step->arrow, ra_ss);
    }

    xPos += gap + double_ofst;
  }
}

u8 UpdateTimer() {
  GameSetup_Step *step = &data->steps[data->state.step_idx];

  int seconds_elapsed = data->timer_frames / 60;
  int seconds_remaining = step->timer_seconds - seconds_elapsed;
  if (seconds_remaining < 0) {
    seconds_remaining = 0;
  }

  int minutes_remaining = seconds_remaining / 60;
  Text_SetText(data->text, data->timer_subtext_id, "%d:%02d", minutes_remaining, seconds_remaining % 60);

  // Increment frame timer
  data->timer_frames++;

  // Return that we have run out of time 3 seconds after time runs out
  return data->timer_frames > 60 * (step->timer_seconds + GRACE_SECONDS);
}

static void ModifySelectorIndex(int change) {
  GameSetup_Step *step = &data->steps[data->state.step_idx];

  // Iterate through and try to find an available selector
  int iter_count = 0;
  for (iter_count = 0; iter_count < step->selector_count; iter_count++) {
    // Edit selector index. We increment by count to handle negative numbers, causing them to loop
    data->state.selector_idx = (data->state.selector_idx + change + step->selector_count) % step->selector_count;

    // Once we are hovering over a selector that is not disabled, stop editing selector idx
    if (!step->selectors[data->state.selector_idx]->state.is_disabled) {
      return;
    }
  }

  // If we didn't find an available selector, don't select any
  if (iter_count >= step->selector_count) {
    data->state.selector_idx = -1;
  }
}

void IncrementSelectorIndex() {
  ModifySelectorIndex(1);
}

void DecrementSelectorIndex() {
  ModifySelectorIndex(-1);
}

void ResetSelectorIndex() {
  data->state.selector_idx = -1;
  ModifySelectorIndex(1);
}

void OnCharSelectionComplete(CharPickerDialog *cpd, u8 is_selection) {
  GameSetup_Step *step = &data->steps[data->state.step_idx];

  // Copy selections from dialog to selector
  if (is_selection) {
    CSIcon_Material mat = CSIcon_ConvertCharToMat(cpd->state.char_selection_idx);
    CSIcon_SetMaterial(step->selectors[0]->icon, mat);
    StockIcon_SetIcon(step->selectors[0]->icon->stock_icon, cpd->state.char_selection_idx, cpd->state.char_color_idx);
  }

  // Display buttons
  ResetButtonState(true);
}

u8 GetNextColor(u8 charId, u8 colorId, int incr) {
  int newCol = colorId + incr;

  int maxColor = CSS_GetCostumeNum(charId);
  newCol = newCol % maxColor;
  if (newCol < 0) {
    newCol += maxColor;
  }

  // TODO: Do recursive call if this color is not allowed

  return newCol;
}