#include "CharPickerDialog.h"

#include "../Files.h"
#include "../Game/Sounds.h"
#include "../m-ex/MexTK/mex.h"
#include "CharStageIcon.h"

void SelectRandomChar(CharPickerDialog *cpd) {
  cpd->state.char_selection_idx = HSD_Randi(26);
}

static void _SetPos(CharPickerDialog *cpd, Vec3 pos) {
  cpd->root_jobj->trans = pos;
  JOBJ_SetMtxDirtySub(cpd->root_jobj);
}

static void _InputsThink(GOBJ *gobj) {
  CharPickerDialog *cpd = gobj->userdata;
  if (!cpd->state.is_open) {
    return;
  }

  // Increment frame count
  cpd->state.open_frame_count++;

  // Skip input handling on the first frame of opening
  if (cpd->state.open_frame_count == 1) {
    return;
  }

  u8 port = R13_U8(-0x5108);
  u64 scrollInputs = Pad_GetRapidHeld(port);  // long delay between initial triggers, then frequent
  u64 downInputs = Pad_GetDown(port);

  if (downInputs & HSD_BUTTON_A) {
    SFX_PlayCommon(CommonSound_ACCEPT);  // Play "accept" sound
    if (cpd->state.char_selection_idx == 26) { // Select random
      SelectRandomChar(cpd);
    }
    CharPickerDialog_CloseDialog(cpd);
    cpd->on_close(cpd, true);
    return;
  } else if (downInputs & HSD_BUTTON_B) {
    SFX_PlayCommon(CommonSound_BACK);  // Play "back" sound
    CharPickerDialog_CloseDialog(cpd);
    cpd->on_close(cpd, false);
    return;
  }

  if (downInputs & HSD_BUTTON_X) {
    // Increment color
    cpd->state.char_color_idx = cpd->get_next_color(cpd->state.char_selection_idx, cpd->state.char_color_idx, 1);
    SFX_PlayCommon(CommonSound_NEXT);  // Play "next" sound
  } else if (downInputs & HSD_BUTTON_Y) {
    // Decrement color
    cpd->state.char_color_idx = cpd->get_next_color(cpd->state.char_selection_idx, cpd->state.char_color_idx, -1);
    SFX_PlayCommon(CommonSound_NEXT);  // Play "next" sound
  }

  if (scrollInputs & (HSD_BUTTON_RIGHT | HSD_BUTTON_DPAD_RIGHT)) {
    // Handle a right input
    if (cpd->state.char_selection_idx >= CKIND_GANONDORF + 1) {
      cpd->state.char_selection_idx = CKIND_YOUNGLINK;
    } else if ((cpd->state.char_selection_idx + 1) % 7 == 0) {
      cpd->state.char_selection_idx -= 6;
    } else {
      cpd->state.char_selection_idx++;
    }

    // Reset color
    cpd->state.char_color_idx = 0;
    SFX_PlayCommon(CommonSound_NEXT);  // Play "next" sound
  } else if (scrollInputs & (HSD_BUTTON_LEFT | HSD_BUTTON_DPAD_LEFT)) {
    // Handle a left input
    if (cpd->state.char_selection_idx == CKIND_YOUNGLINK) {
      cpd->state.char_selection_idx = CKIND_GANONDORF + 1;
    } else if (cpd->state.char_selection_idx % 7 == 0) {
      cpd->state.char_selection_idx += 6;
    } else {
      cpd->state.char_selection_idx--;
    }

    // Reset color
    cpd->state.char_color_idx = 0;
    SFX_PlayCommon(CommonSound_NEXT);  // Play "next" sound
  } else if (scrollInputs & (HSD_BUTTON_DOWN | HSD_BUTTON_DPAD_DOWN)) {
    // Handle a down input
    if (cpd->state.char_selection_idx % 7 >= 6 && cpd->state.char_selection_idx > CKIND_SHEIK) {
      cpd->state.char_selection_idx -= 14;
    } else if (cpd->state.char_selection_idx >= CKIND_YOUNGLINK) {
      cpd->state.char_selection_idx -= 21;
    } else {
      cpd->state.char_selection_idx += 7;
    }

    // Reset color
    cpd->state.char_color_idx = 0;
    SFX_PlayCommon(CommonSound_NEXT);  // Play "next" sound
  } else if (scrollInputs & (HSD_BUTTON_UP | HSD_BUTTON_DPAD_UP)) {
    // Handle a up input
    if (cpd->state.char_selection_idx % 7 >= 5 && cpd->state.char_selection_idx <= CKIND_LINK) {
      cpd->state.char_selection_idx += 21;
    } else if (cpd->state.char_selection_idx <= CKIND_LINK) {
      cpd->state.char_selection_idx += 21;
    } else {
      cpd->state.char_selection_idx -= 7;
    }

    // Reset color
    cpd->state.char_color_idx = 0;
    SFX_PlayCommon(CommonSound_NEXT);  // Play "next" sound
  }

  // Update which icon shows up selected
  for (int i = CKIND_FALCON; i <= CKIND_GANONDORF + 1; i++) {
    CSIcon_Select_State state = CSIcon_Select_State_NotSelected;
    u8 colorId = 0;
    if (i == cpd->state.char_selection_idx) {
      state = CSIcon_Select_State_Hover;
      colorId = cpd->state.char_color_idx;
    }

    // Only show stock icons on characters
    if (i <= CKIND_GANONDORF) {
      CSIcon_SetStockIconVisibility(cpd->char_icons[i], i == cpd->state.char_selection_idx);
      StockIcon_SetIcon(cpd->char_icons[i]->stock_icon, i, colorId);
    }
    CSIcon_SetSelectState(cpd->char_icons[i], state);
  }
}

CharPickerDialog *CharPickerDialog_Init(GUI_GameSetup *gui, void *on_close, void *get_next_color) {
  CharPickerDialog *cpd = calloc(sizeof(CharPickerDialog));

  // Init char picker dialog jobj
  cpd->jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_CharDialog];
  cpd->gobj = JOBJ_LoadSet(0, cpd->jobj_set, 0, 0, 3, 1, 0, GObj_Anim);
  cpd->root_jobj = cpd->gobj->hsd_object;

  // Store callback
  cpd->on_close = on_close;
  cpd->get_next_color = get_next_color;

  // Connect CharStageIcons for each character to the dialog
  JOBJ *cur_joint = cpd->root_jobj->child->child->sibling->child;
  for (int i = CKIND_FALCON; i <= CKIND_GANONDORF + 1; i++) {
    cpd->char_icons[i] = CSIcon_Init(gui);
    CSIcon_SetMaterial(cpd->char_icons[i], i < 26 ? CSIcon_ConvertCharToMat(i): CSIcon_Material_Question);
    // Attach icon to cur joint and move to next joint
    JOBJ_AddChild(cur_joint, cpd->char_icons[i]->root_jobj);
    cur_joint = cur_joint->sibling;
  }

  // Create GOBJ to handle inputs
  GOBJ *input_handler_gobj = GObj_Create(4, 0, 128);
  GObj_AddUserData(input_handler_gobj, 4, HSD_Free, cpd);
  GObj_AddProc(input_handler_gobj, _InputsThink, 0);

  // Initialize state
  CharPickerDialog_SetPos(cpd, (Vec3){0, 0, 0});
  CharPickerDialog_CloseDialog(cpd);

  return cpd;
}

void CharPickerDialog_Free(CharPickerDialog *cpd) {
  // Not implemented because we only initialize a very limited amount of these
}

void CharPickerDialog_SetPos(CharPickerDialog *cpd, Vec3 pos) {
  cpd->state.pos = pos;
  if (!cpd->state.is_open) {
    return;
  }

  _SetPos(cpd, pos);
}

void CharPickerDialog_OpenDialog(CharPickerDialog *cpd, u8 start_char_idx, u8 start_char_color) {
  cpd->state.is_open = true;
  cpd->state.char_selection_idx = start_char_idx;
  cpd->state.char_color_idx = start_char_color;
  cpd->state.open_frame_count = 0;
  _SetPos(cpd, cpd->state.pos);
}

void CharPickerDialog_CloseDialog(CharPickerDialog *cpd) {
  cpd->state.is_open = false;
  _SetPos(cpd, (Vec3){0, 1000, 0});
}