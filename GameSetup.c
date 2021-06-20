#include "GameSetup.h"

#include "./Components/Button.h"
#include "./Components/CharStageBoxSelector.h"
#include "./Components/CharStageIcon.h"
#include "./m-ex/MexTK/mex.h"
#include "Files.h"

static ArchiveInfo *gui_archive;
static GUI_GameSetup *gui_assets;
static GameSetup_Data *data;

void Minor_Load(void *minor_data) {
  OSReport("minor load\n");

  // Reset selections such that we can use that as a signal to start the game
  // TODO: We actually can't do this yet because it causes the match state function to return
  // TODO: different values, one I noticed is the rng offset returns as zero.
  // TODO: Perhaps I can call this after fetching match state? Need to figure out exactly what
  // TODO: our termination condition looks like, have to make sure we get the correct stage.
  // ExiSlippi_ResetSelections_Query *rsq = calloc(sizeof(ExiSlippi_ResetSelections_Query));
  // rsq->command = ExiSlippi_Command_RESET_SELECTIONS;
  // ExiSlippi_Transfer(rsq, sizeof(ExiSlippi_ResetSelections_Query), ExiSlippi_TransferMode_WRITE);
  // HSD_Free(rsq);

  // Init location to store data
  data = calloc(sizeof(GameSetup_Data));
  data->process_type = GameSetup_Process_Type_STAGE_STRIKING;

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
  InitSelectorJobjs();

  // Prepare text
  data->text = Text_CreateText(0, 0);
  data->text->kerning = 1;
  data->text->align = 0;  // align left
  data->text->scale = (Vec2){0.01, 0.01};

  data->timer_subtext_id = Text_AddSubtext(data->text, -2300, -1300, "0:30");
  Text_SetScale(data->text, data->timer_subtext_id, 5, 5);

  // Load confirm/change buttons
  data->buttons[0] = Button_Init(gui_assets);
  data->buttons[1] = Button_Init(gui_assets);
  Button_SetPos(data->buttons[0], (Vec3){-4.8, -2.5, 0});
  Button_SetPos(data->buttons[1], (Vec3){4.8, -2.5, 0});
  Button_SetMaterial(data->buttons[0], Button_Material_Ok);
  Button_SetMaterial(data->buttons[1], Button_Material_Redo);
  data->button_count = 2;

  // Initialize state, init steps, and prepare current step
  InitState();
  InitSteps();
  PrepareCurrentStep();
  UpdateTimeline();
}

void InitState() {
  data->state.selector_idx = 0;  // TODO: Set differently if first or second striker
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

void InitSteps() {
  ExiSlippi_MatchState_Response *match_state = data->match_state;

  data->step_count = 5;
  data->steps = calloc(data->step_count * sizeof(GameSetup_Step));
  data->state.step_idx = 0;

  FlatTexture_Texture p1_label = FlatTexture_Texture_YOUR_CHAR_LABEL;
  FlatTexture_Texture p2_label = FlatTexture_Texture_OPP_CHAR_LABEL;
  if (match_state->local_player_idx != 0) {
    p1_label = FlatTexture_Texture_OPP_CHAR_LABEL;
    p2_label = FlatTexture_Texture_YOUR_CHAR_LABEL;
  }

  data->steps[0].player_idx = 0;
  data->steps[0].type = GameSetup_Step_Type_CHOOSE_CHAR;
  data->steps[0].required_selection_count = 1;
  data->steps[0].char_selection = match_state->game_info_block[0x60];
  data->steps[0].char_color_selection = match_state->game_info_block[0x63];
  data->steps[0].timer_seconds = 20;
  data->steps[0].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[0].label = InitStepLabel(data->steps[0].display_icons[0], p1_label);
  data->steps[0].arrow = 0;

  data->steps[1].player_idx = 1;
  data->steps[1].type = GameSetup_Step_Type_CHOOSE_CHAR;
  data->steps[1].required_selection_count = 1;
  data->steps[1].char_selection = match_state->game_info_block[0x60 + 0x24];
  data->steps[1].char_color_selection = match_state->game_info_block[0x63 + 0x24];
  data->steps[1].timer_seconds = 20;
  data->steps[1].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[1].label = InitStepLabel(data->steps[1].display_icons[0], p2_label);
  data->steps[1].arrow = InitStepArrow(data->steps[1].display_icons[0]);

  data->steps[2].player_idx = 0;
  data->steps[2].type = GameSetup_Step_Type_REMOVE_STAGE;
  data->steps[2].required_selection_count = 1;
  data->steps[2].timer_seconds = 30;
  data->steps[2].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[2].label = InitStepLabel(data->steps[2].display_icons[0], FlatTexture_Texture_STRIKE1_LABEL);
  data->steps[2].arrow = InitStepArrow(data->steps[2].display_icons[0]);

  data->steps[3].player_idx = 1;
  data->steps[3].type = GameSetup_Step_Type_REMOVE_STAGE;
  data->steps[3].required_selection_count = 2;
  data->steps[3].timer_seconds = 20;
  data->steps[3].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[3].display_icons[1] = CSIcon_Init(gui_assets);
  data->steps[3].label = InitStepLabel(data->steps[3].display_icons[0], FlatTexture_Texture_STRIKE23_LABEL);
  data->steps[3].arrow = InitStepArrow(data->steps[3].display_icons[0]);

  data->steps[4].player_idx = 0;
  data->steps[4].type = GameSetup_Step_Type_REMOVE_STAGE;
  data->steps[4].required_selection_count = 1;
  data->steps[4].timer_seconds = 10;
  data->steps[4].display_icons[0] = CSIcon_Init(gui_assets);
  data->steps[4].label = InitStepLabel(data->steps[4].display_icons[0], FlatTexture_Texture_STRIKE4_LABEL);
  data->steps[4].arrow = InitStepArrow(data->steps[4].display_icons[0]);

  // Start with the first two steps completed
  CompleteCurrentStep(data->steps[0].required_selection_count);
  CompleteCurrentStep(data->steps[1].required_selection_count);
}

void InitSelectorJobjs() {
  int count = 5;
  CSIcon_Material iconMats[] = {
      CSIcon_Material_Fountain,
      CSIcon_Material_Dreamland,
      CSIcon_Material_Yoshis,
      CSIcon_Material_Pokemon,
      CSIcon_Material_Battlefield,
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
  for (int i = 0; i < count; i++) {
    CSBoxSelector *csbs = CSBoxSelector_Init(gui_assets);
    CSIcon_SetMaterial(csbs->icon, iconMats[i]);
    CSBoxSelector_SetPos(csbs, (Vec3){xPos, yPos, 0});

    data->selectors[i] = csbs;
    xPos += gap;
  }

  data->selector_count = count;

  // Hover the first item
  CSBoxSelector_SetHover(data->selectors[0], true);
}

void ResetButtonState() {
  for (int i = 0; i < data->button_count; i++) {
    Button_SetHover(data->buttons[i], false);
    Button_SetVisibility(data->buttons[i], false);
  }

  data->state.btn_hover_idx = 0;
}

void Minor_Think() {
  GameSetup_Step *step = &data->steps[data->state.step_idx];

  // If current step is completed, process finished
  // TODO: Add some kind of delay/display to indicate which stage was selected
  if (step->state == GameSetup_Step_State_COMPLETE) {
    // TODO: Play an animation on selected stage and play a sound
    Scene_ExitMinor();
  }
}

void Minor_Exit(void *minor_data) {
  OSReport("minor exit\n");
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
    OSReport("Port: %d\n", port);
  }

  // Button_SetMaterial(data->buttons[0], Button_Material_Ok);

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
      u8 isSelected = data->selectors[idx]->state.is_selected;

      if (isSelected) {
        CSBoxSelector_SetSelectState(data->selectors[idx], CSBoxSelector_Select_State_NotSelected);
        data->state.selected_values_count--;
      } else {
        CSBoxSelector_SetSelectState(data->selectors[idx], CSBoxSelector_Select_State_Selected_X);
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

      // Display and prepare buttons
      for (int i = 0; i < data->button_count; i++) {
        Button_SetVisibility(data->buttons[i], true);
      }

      data->state.btn_hover_idx = 0;
      Button_SetHover(data->buttons[0], true);
    }
  } else {
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

    // TODO: Handle buttons before scroll, don't scroll on the same frame a button is pressed
    if (downInputs & HSD_BUTTON_B || (downInputs & HSD_BUTTON_A && data->state.btn_hover_idx == 1)) {
      data->state.selector_idx = data->state.prev_selector_idx;
      data->state.selected_values_count = 0;
      for (int i = 0; i < data->selector_count; i++) {
        // Only reset selectors that are currently selected
        if (!data->selectors[i]->state.is_selected) {
          continue;
        }

        CSBoxSelector_SetSelectState(data->selectors[i], CSBoxSelector_Select_State_NotSelected);
      }

      // Hide buttons
      ResetButtonState();

      SFX_PlayCommon(2);
    } else if (downInputs & HSD_BUTTON_A && data->state.btn_hover_idx == 0) {
      SFX_PlayCommon(1);
      CompleteCurrentStep(0);
      PrepareCurrentStep();
      UpdateTimeline();
    }
  }

  // Update selector position
  for (int i = 0; i < data->selector_count; i++) {
    // Set hover state. Won't do anything if already set to that state
    CSBoxSelector_SetHover(data->selectors[i], data->state.selector_idx == i);
  }

  // Update button hover position
  for (int i = 0; i < data->button_count; i++) {
    // Set hover state. Won't do anything if already set to that state
    Button_SetHover(data->buttons[i], data->state.btn_hover_idx == i);
  }

  frame_counter++;
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

  // Commit selections to selected values
  int commit_index = committed_count;
  for (int i = 0; i < data->selector_count; i++) {
    CSBoxSelector *bs = data->selectors[i];

    // If this selector is selected, indicate that stage as the one selected
    if (bs->state.is_selected) {
      int stageId = CSIcon_ConvertMatToStage(bs->icon->state.material);
      step->stage_selections[commit_index] = stageId;
      commit_index++;
    }
  }

  // In the case of a timeout, the selections may not have been made. Select non-disabled
  // stages
  for (int i = 0; i < data->selector_count; i++) {
    if (commit_index >= step->required_selection_count) {
      break;  // If enough selections have been made, exit loop
    }

    CSBoxSelector *bs = data->selectors[i];

    // If this selector is selected, indicate that stage as the one selected
    if (!bs->state.is_disabled) {
      int stageId = CSIcon_ConvertMatToStage(bs->icon->state.material);
      step->stage_selections[commit_index] = stageId;
      commit_index++;
    }
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

  // Increment step idx
  data->state.step_idx++;
  if (data->state.step_idx >= data->step_count) {
    // TODO: Complete full process and end scene
    data->state.step_idx = data->step_count - 1;
    return;
  } else {
    // Activate next step
    step = &data->steps[data->state.step_idx];
    step->state = GameSetup_Step_State_ACTIVE;
  }
}

void PrepareCurrentStep() {
  // Do nothing if step outside of bounds
  if (data->state.step_idx >= data->step_count) {
    return;
  }

  GameSetup_Step *step = &data->steps[data->state.step_idx];

  // Initialize current timer to 0
  data->timer_frames = 0;

  // TODO: Show/Hide the selectors used for this step and adjust number

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
  for (int i = 0; i < data->selector_count; i++) {
    CSBoxSelector *csbs = data->selectors[i];

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
  ResetButtonState();
}

void UpdateTimeline() {
  float xPos = -26;
  float yPos = -17.5;

  float gap = 10;
  float double_inc = 2.6;

  float labelX = 6;
  float labelY = 0.6;
  float labelGapY = 3.2;

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
      // Show question if not complete, result if complete
      CSIcon_Material mat = CSIcon_Material_Question;
      if (step->state == GameSetup_Step_State_COMPLETE) {
        if (step->type == GameSetup_Step_Type_CHOOSE_CHAR) {
          mat = CSIcon_ConvertCharToMat(step->char_selection);
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
  // Iterate through and try to find an available selector
  int iter_count = 0;
  for (iter_count = 0; iter_count < data->selector_count; iter_count++) {
    // Edit selector index. We increment by count to handle negative numbers, causing them to loop
    data->state.selector_idx = (data->state.selector_idx + change + data->selector_count) % data->selector_count;

    // Once we are hovering over a selector that is not disabled, stop editing selector idx
    if (!data->selectors[data->state.selector_idx]->state.is_disabled) {
      return;
    }
  }

  // If we didn't find an available selector, don't select any
  if (iter_count >= data->selector_count) {
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