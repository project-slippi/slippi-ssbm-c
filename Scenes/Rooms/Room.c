#include "Room.h"

#include "../../Components/CharStageIcon.h"
#include "../../Components/StockIcon.h"
#include "../../Game/Sounds.h"

static HSD_Archive *gui_archive;
static GUI_GameSetup *gui_assets;
static Room_Data *data;

static char *mode_labels[] = {"Singles", "Doubles", "Ironman", "Crews", "Tourney"};
static char *stage_labels[] = {"Random stages", "Stage draft"};

// Starter stages, used for both the draft and random stages. Same order as Dolphin's room
static CSIcon_Material stages[ROOM_STAGE_COUNT] = {
    CSIcon_Material_Fountain,
    CSIcon_Material_Dreamland,
    CSIcon_Material_Yoshis,
    CSIcon_Material_FinalDestination,
    CSIcon_Material_Battlefield,
    CSIcon_Material_Pokemon,
};
static char *stage_names[ROOM_STAGE_COUNT] = {
    "Fountain of Dreams",
    "Dreamland",
    "Yoshi's Story",
    "Final Destination",
    "Battlefield",
    "Pokemon Stadium",
};

// The room's playlist. A song is picked at random each time the room opens, including after a match
static u8 room_songs[] = {
    0x25,  // howto_s.hps, How to Play
    0x53,  // target.hps, Target Test
    0x00,  // 1p_qk.hps, All-Star
    0x01,  // akaneia.hps
    0x1e,  // flatzone.hps
    0x1f,  // fourside.hps
    0x33,  // kraid.hps
    0x4c,  // siren.hps
    0x5d,  // vs_hyou1.hps, the tournament theme Ranked plays
    0x5e,  // vs_hyou2.hps, the other one
};

// Indexed by ExiSlippi_RoomError
static char *join_errors[] = {
    "",
    "Rooms are unavailable right now",
    "No room with that code",
    "Wrong password",
    "That room is full",
    "Too many tries, wait a few minutes",
    "Could not connect to the room",
    "Lost connection to the room",
    "The room did not let you in",
    "The room closed after an hour without activity",
};

const GXColor text_color = {255, 255, 255, 255};
const GXColor dim_color = {190, 190, 195, 255};
const GXColor shadow_color = {0, 0, 0, 255};
const GXColor list_bar_color = {48, 48, 54, 255};
const GXColor warn_color = {254, 202, 52, 255};
const GXColor panic_color = {255, 50, 50, 255};

void minor_load(Rooms_SceneData *minor_data) {
  data = calloc(sizeof(Room_Data));
  data->scene_data = minor_data;
  data->hold_idx = -1;
  data->confirm_idx = -1;

  // Allocate some memory used throughout
  data->state_query = calloc(sizeof(ExiSlippi_GetRoomState_Query));
  data->state = calloc(sizeof(ExiSlippi_GetRoomState_Response));
  data->prev_state = calloc(sizeof(ExiSlippi_GetRoomState_Response));
  data->action_query = calloc(sizeof(ExiSlippi_RoomAction_Query));
  data->match_state = ExiSlippi_LoadMatchState(0);

  // Coming back from the room's match. The room is told once the state is in, see OnStateChange
  if (minor_data->start_match) {
    minor_data->start_match = false;
    data->is_back_from_match = true;
  }

  // Set up input handler. Initialize at top to make sure it runs before anything else
  GOBJ *input_handler_gobj = GObj_Create(4, 0, 128);
  GObj_AddProc(input_handler_gobj, InputsThink, 0);

  // Load file
  gui_archive = Archive_LoadFile("GameSetup_gui.dat");
  gui_assets = Archive_GetPublicAddress(gui_archive, "ScGamTour_scene_data");

  GOBJ *cam_gobj = GObj_Create(2, 3, 128);
  COBJ *cam_cobj = COBJ_LoadDesc(gui_assets->cobjs[0]);
  GObj_AddObject(cam_gobj, 1, cam_cobj);
  GOBJ_InitCamera(cam_gobj, CObjThink, 0);
  GObj_AddProc(cam_gobj, MainMenu_CamRotateThink, 5);

  // Indicates which gx_links to render
  cam_gobj->cobj_links = (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4);

  // Like the rooms list, the list's names are drawn by a second camera after everything else, so they
  // sit on the bars behind them. It tilts with the main camera to stay lined up
  GOBJ *text_cam_gobj = GObj_Create(2, 3, 128);
  COBJ *text_cam_cobj = COBJ_LoadDesc(gui_assets->cobjs[0]);
  GObj_AddObject(text_cam_gobj, 1, text_cam_cobj);
  GOBJ_InitCamera(text_cam_gobj, TextCObjThink, 1);
  GObj_AddProc(text_cam_gobj, MainMenu_CamRotateThink, 5);
  text_cam_gobj->cobj_links = 1 << LIST_TEXT_GXLINK;
  int list_canvas = Text_CreateCanvas(0, (int)text_cam_gobj, 11, 11, 0, LIST_TEXT_GXLINK, 0, 0);

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

  // Load panels, narrowing the middle frame to fit the stage area
  JOBJ *panels = LoadPanels();
  SetFrameSides(panels, STAGE_LEFT, STAGE_RIGHT);

  // Load a second copy of the panels and only show its frame, used for the list
  JOBJ *list_panels = LoadPanels();
  HideJoint(GetJoint(list_panels, PANELS_MIDDLE_JOINT));
  HideJoint(GetJoint(list_panels, PANELS_TOP_JOINT));
  HideJoint(GetJoint(list_panels, PANELS_BOTTOM_JOINT));
  SetFrameSides(list_panels, LIST_LEFT, LIST_RIGHT);

  // Prepare text
  data->text = CreateText(0, 1);
  data->lower_text = CreateText(0, 1);
  data->left_text = CreateText(0, 0);
  data->right_text = CreateText(0, 2);
  data->list_shadow_text = CreateText(list_canvas, 0);
  data->list_bold_text = CreateText(list_canvas, 0);
  data->list_text = CreateText(list_canvas, 0);

  InitHeader();
  InitStage();
  InitList();
  InitPrompts();

  // Initialize dialog last to make sure it's on top of everything
  data->char_picker_dialog = CharPickerDialog_Init(gui_assets, OnCharSelectionComplete, GetNextColor);
  CharPickerDialog_SetPos(data->char_picker_dialog, (Vec3){0, -9, 0});

  FetchState();
  OnStateChange();

  BGM_Play(room_songs[HSD_Randi(sizeof(room_songs))]);
}

void minor_think() {
  if (data->should_exit) {
    Scene_ExitMinor();
    return;
  }

  FetchState();

  // The room is gone, such as after leaving it
  if (!data->state->is_active) {
    data->should_exit = true;
    return;
  }

  if (memcmp(data->state, data->prev_state, sizeof(ExiSlippi_GetRoomState_Response)) != 0) {
    OnStateChange();
  }

  if (IsInRoom()) {
    UpdateCharPicker();
    HandleMatchHandoff();
  }

  // Text draws over the models, so the text under the stage hides while the character picker is
  // open, and so does the list, bars and all
  u8 is_picker_open = data->char_picker_dialog->state.is_open;
  if (is_picker_open != data->is_list_hidden) {
    data->is_list_hidden = is_picker_open;
    data->lower_text->hidden = is_picker_open;
    data->list_shadow_text->hidden = is_picker_open;
    data->list_bold_text->hidden = is_picker_open;
    data->list_text->hidden = is_picker_open;
    if (IsInRoom()) {
      UpdateList();
    }
  }

  // Watchers go straight to the match, the same way its players do
  if (data->state->watch_status == ExiSlippi_WatchStatus_READY) {
    data->scene_data->start_watch = true;
    data->should_exit = true;
    return;
  }

  if (data->notice_frames > 0 && --data->notice_frames == 0 && IsInRoom()) {
    UpdateStage();
  }
}

void minor_exit(Rooms_SceneData *minor_data) {
}

// Loads a copy of the panels, frozen fully open
JOBJ *LoadPanels() {
  GOBJ *gobj = JOBJ_LoadSet(0, gui_assets->jobjs[GUI_GameSetup_JOBJ_Panels], 0, 0, 3, 1, 1, GObj_Anim);
  GObj_RemoveProc(gobj);

  JOBJ *jobj = gobj->hsd_object;
  JOBJ_ReqAnimAll(jobj, PANELS_OPEN_FRAME);
  JOBJ_AnimAll(jobj);
  return jobj;
}

// Index is depth first, the same order HSDRawViewer lists joints in
JOBJ *GetJoint(JOBJ *root, int index) {
  JOBJ *jobj = 0;
  JOBJ_GetChild(root, &jobj, index, -1);
  return jobj;
}

void HideJoint(JOBJ *jobj) {
  for (DOBJ *d = jobj->dobj; d; d = d->next) {
    d->flags |= DOBJ_HIDDEN;
  }
}

// Moves the left and right edges of the inner frame
void SetFrameSides(JOBJ *panels, float left, float right) {
  int left_joints[] = {PANELS_FRAME_TOP_LEFT, PANELS_FRAME_BOTTOM_LEFT};
  int right_joints[] = {PANELS_FRAME_TOP_RIGHT, PANELS_FRAME_BOTTOM_RIGHT};

  for (int i = 0; i < 2; i++) {
    JOBJ *left_jobj = GetJoint(panels, left_joints[i]);
    left_jobj->trans.X = left;
    JOBJ_SetMtxDirtySub(left_jobj);

    JOBJ *right_jobj = GetJoint(panels, right_joints[i]);
    right_jobj->trans.X = right;
    JOBJ_SetMtxDirtySub(right_jobj);
  }
}

// Same text setup as ranked. Subtext positions are 100 units per world unit, with y
// pointing down
Text *CreateText(int canvas, u8 align) {
  Text *text = Text_CreateText(0, canvas);
  text->kerning = 1;
  text->align = align;
  text->use_aspect = 1;
  text->scale = (Vec2){0.01, 0.01};

  // Widen the text so long lines don't get squeezed
  text->aspect.X *= 5;
  return text;
}

int AddSubtext(Text *text, float x, float y, float scale, char *str) {
  int id = Text_AddSubtext(text, x, y, str);
  Text_SetScale(text, id, scale, scale);
  return id;
}

// Room code and password on the left, room settings on the right
void InitHeader() {
  data->code_subtext_id = AddSubtext(data->left_text, -2800, -1940, 5, "");
  data->password_subtext_id = AddSubtext(data->left_text, -2800, -1740, 3, "");
  Text_SetColor(data->left_text, data->password_subtext_id, &dim_color);

  data->mode_subtext_id = AddSubtext(data->right_text, 2800, -1940, 5, "");
  data->stage_mode_subtext_id = AddSubtext(data->right_text, 2800, -1740, 3, "");
  Text_SetColor(data->right_text, data->stage_mode_subtext_id, &dim_color);

  // Init timer subtext
  data->timer_subtext_id = AddSubtext(data->text, 0, -1880, 6, "");
}

// Winner on the left, challenger on the right and the stage between them. The strike
// selectors replace all three during the draft
void InitStage() {
  float xs[] = {STAGE_BOX_LEFT_X, STAGE_BOX_RIGHT_X};
  for (int i = 0; i < 2; i++) {
    CSBoxSelector *bs = CSBoxSelector_Init(gui_assets);
    CSBoxSelector_SetPos(bs, (Vec3){xs[i], STAGE_BOX_Y, 0});
    data->char_selectors[i] = bs;

    data->name_subtext_ids[i] = AddSubtext(data->text, xs[i] * 100, -720, 4, "");
  }

  // Win streak, under the winner
  data->streak_subtext_id = AddSubtext(data->lower_text, STAGE_BOX_LEFT_X * 100, 130, 3, "");
  Text_SetColor(data->lower_text, data->streak_subtext_id, &warn_color);

  data->stage_selector = CSBoxSelector_Init(gui_assets);
  CSBoxSelector_SetPos(data->stage_selector, (Vec3){STAGE_CENTER_X, STAGE_BOX_Y, 0});

  float x = STAGE_CENTER_X - STRIKE_BOX_GAP * (ROOM_STAGE_COUNT - 1) / 2;
  for (int i = 0; i < ROOM_STAGE_COUNT; i++) {
    CSBoxSelector *bs = CSBoxSelector_Init(gui_assets);
    CSIcon_SetMaterial(bs->icon, stages[i]);
    CSBoxSelector_SetPos(bs, (Vec3){x + STRIKE_BOX_GAP * i, STAGE_BOX_Y, 0});
    data->stage_strike_selectors[i] = bs;
  }

  data->vs_subtext_id = AddSubtext(data->text, STAGE_CENTER_X * 100, -330, 5, "VS");
  data->status_subtext_id = AddSubtext(data->lower_text, STAGE_CENTER_X * 100, 520, 3.5, "");
  Text_SetColor(data->lower_text, data->status_subtext_id, &dim_color);
  data->watch_subtext_id = AddSubtext(data->lower_text, STAGE_CENTER_X * 100, 325, 3.2, "");
}

// The queue, followed by the rest of the room. Each player gets a bar behind their name
void InitList() {
  for (int i = 0; i < LIST_LINES; i++) {
    float x = LIST_LEFT * 100 + LIST_TEXT_INSET_X;
    float y = -880 + LIST_LINE_GAP * i;
    AddSubtext(data->list_shadow_text, x + LIST_SHADOW_OFFSET, y + LIST_SHADOW_OFFSET, 3.5, "");
    Text_SetColor(data->list_shadow_text, i, &shadow_color);
    AddSubtext(data->list_bold_text, x + LIST_BOLD_OFFSET, y, 3.5, "");
    AddSubtext(data->list_text, x, y, 3.5, "");
    data->list_bars[i] = LoadListBar(i);
  }

}

// Button prompts in the bottom panel
void InitPrompts() {
  for (int i = 0; i < 3; i++) {
    data->prompt_subtext_ids[i] = AddSubtext(data->lower_text, (i - 1) * PROMPT_GAP_X, 1640, 3.6, "");
  }
}

// The character picker's dark box, reshaped into a bar by moving its corners, the way FlatTexture
// places its corners. Hidden until a player is on its line
JOBJ *LoadListBar(int line) {
  GOBJ *gobj = JOBJ_LoadSet(0, gui_assets->jobjs[GUI_GameSetup_JOBJ_CharDialog], 0, 0, 3, 1, 0, 0);
  JOBJ *jobj = gobj->hsd_object;

  // The picker raises its box toward the camera, which would make the bar bigger than its corners say
  JOBJ *layer = jobj->child;
  layer->trans.Z = LIST_BAR_Z;

  // A flat, solid color rather than the picker's see-through bordered texture. Each loaded copy has
  // its own material, so the picker keeps its look
  JOBJ *box = layer->child;
  MOBJ *mobj = box->dobj->mobj;
  mobj->rendermode &= ~(RENDER_TEXTURES | RENDER_XLU | CHANNEL_FIELD | RENDER_ALPHA_BITS);
  mobj->rendermode |= RENDER_CONSTANT;
  mobj->mat->diffuse = list_bar_color;
  mobj->mat->alpha = 1;

  // Top right, bottom left, top left and bottom right
  JOBJ *corner = box->child;
  float w = LIST_BAR_HALF_WIDTH;
  float h = LIST_BAR_HALF_HEIGHT;
  corner->trans = (Vec3){w, h, 0};
  corner->sibling->trans = (Vec3){-w, -h, 0};
  corner->sibling->sibling->trans = (Vec3){-w, h, 0};
  corner->sibling->sibling->sibling->trans = (Vec3){w, -h, 0};

  // Text y points down while the scene's y points up
  jobj->trans.X = LIST_BAR_CENTER_X;
  jobj->trans.Y = (880 - LIST_LINE_GAP * line) / 100.0 - LIST_BAR_Y_OFFSET;
  JOBJ_SetMtxDirtySub(jobj);
  JOBJ_SetFlagsAll(jobj, JOBJ_HIDDEN);
  return jobj;
}

// Sets a list line on the text and on its shadow and bold passes
void SetListLine(int line, const GXColor *color, char *str) {
  Text_SetText(data->list_shadow_text, line, "%s", str);
  Text_SetText(data->list_bold_text, line, "%s", str);
  Text_SetText(data->list_text, line, "%s", str);
  Text_SetColor(data->list_bold_text, line, color);
  Text_SetColor(data->list_text, line, color);
}

void ShowListBar(int line, u8 is_shown) {
  if (is_shown && !data->is_list_hidden) {
    JOBJ_ClearFlagsAll(data->list_bars[line], JOBJ_HIDDEN);
  } else {
    JOBJ_SetFlagsAll(data->list_bars[line], JOBJ_HIDDEN);
  }
}

// Fetch the room from Dolphin, keeping the last one to see what changed
void FetchState() {
  memcpy(data->prev_state, data->state, sizeof(ExiSlippi_GetRoomState_Response));

  data->state_query->command = ExiSlippi_Command_GET_ROOM_STATE;
  ExiSlippi_Transfer(data->state_query, sizeof(ExiSlippi_GetRoomState_Query), ExiSlippi_TransferMode_WRITE);
  ExiSlippi_Transfer(data->state, sizeof(ExiSlippi_GetRoomState_Response), ExiSlippi_TransferMode_READ);
}

void SendAction(u8 action, u8 value0, u8 value1) {
  ExiSlippi_RoomAction_Query *q = data->action_query;
  q->command = ExiSlippi_Command_ROOM_ACTION;
  q->action = action;
  q->value[0] = value0;
  q->value[1] = value1;
  ExiSlippi_Transfer(q, sizeof(ExiSlippi_RoomAction_Query), ExiSlippi_TransferMode_WRITE);
}

void OnStateChange() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  ExiSlippi_GetRoomState_Response *prev = data->prev_state;

  UpdateHeader();
  if (!IsInRoom()) {
    UpdateJoining();
    return;
  }

  // Start the stage cursor on the first stage that isn't struck each turn
  if (state->phase != prev->phase) {
    data->selector_idx = 0;
    while (data->selector_idx < ROOM_STAGE_COUNT - 1 && state->struck[data->selector_idx]) {
      data->selector_idx++;
    }
  }

  if (HasHostChanged()) {
    data->notice_frames = NOTICE_FRAMES;
  }

  // Tells the room the match ended without a result, again if the room missed it, such as while
  // its host changed. The room ignores it once it knows
  if (data->is_back_from_match && state->phase == Room_Phase_PLAYING && !state->match_over &&
      LocalSide() != Room_Side_NONE) {
    SendAction(ExiSlippi_RoomAction_MATCH_ENDED, 0, 0);
  }

  // Warn the local player when their time is almost up
  if (IsLocalTurn() && state->turn_seconds == PANIC_SECONDS && prev->turn_seconds != PANIC_SECONDS) {
    SFX_PlayCommon(CommonSound_OFFSCREEN);
  }

  UpdateStage();
  UpdatePrompts();
  UpdateList();
}

// Returns the member's position in the queue, -1 if not queued
int QueuePos(int member) {
  ExiSlippi_GetRoomState_Response *state = data->state;
  for (int i = 0; i < state->queue_count; i++) {
    if (state->queue[i] == member) {
      return i;
    }
  }
  return -1;
}

u8 IsOnSide(int member) {
  return data->state->sides[Room_Side_WINNER] == member || data->state->sides[Room_Side_CHALLENGER] == member;
}

Room_Side LocalSide() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  if (state->sides[Room_Side_WINNER] == state->local_member) {
    return Room_Side_WINNER;
  }
  if (state->sides[Room_Side_CHALLENGER] == state->local_member) {
    return Room_Side_CHALLENGER;
  }
  return Room_Side_NONE;
}

Room_Side TurnSide() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  if (state->phase == Room_Phase_STRIKING) {
    return Room_Side_WINNER;
  }
  if (state->phase == Room_Phase_CHOOSING) {
    return Room_Side_CHALLENGER;
  }
  if (state->phase == Room_Phase_PICKING) {
    return state->has_picked[Room_Side_WINNER] ? Room_Side_CHALLENGER : Room_Side_WINNER;
  }
  return Room_Side_NONE;
}

u8 IsLocalTurn() {
  Room_Side turn = TurnSide();
  return turn != Room_Side_NONE && turn == LocalSide();
}

// Name followed by the member's crown count. 0x817e is the multiplication sign in
// Melee's font
void GetDisplayName(char *out, int member) {
  ExiSlippi_RoomMember *m = &data->state->members[member];
  if (m->crowns == 0) {
    sprintf(out, "%s", m->name);
  } else {
    sprintf(out, "%s  \x81\x7e%d", m->name, m->crowns);
  }
}

// Whether we're hosting the room or have joined it, rather than still joining
u8 IsInRoom() {
  u8 status = data->state->connection_status;
  return status == ExiSlippi_RoomStatus_HOSTING || status == ExiSlippi_RoomStatus_JOINED ||
         status == ExiSlippi_RoomStatus_RECONNECTING;
}

// A member takes over when the host leaves or drops, so the host can change at any time
u8 HasHostChanged() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  ExiSlippi_GetRoomState_Response *prev = data->prev_state;
  if (state->host_member == 0xFF || prev->host_member == 0xFF) {
    return false;
  }
  return strcmp(state->members[state->host_member].connect_code, prev->members[prev->host_member].connect_code) != 0;
}

// The code arrives once the room is registered, and stays empty if it couldn't be
void UpdateHeader() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  Text_SetText(data->left_text, data->code_subtext_id, "Room %s", state->code[0] ? state->code : "----");

  // Only private rooms have a password
  if (state->password[0]) {
    Text_SetText(data->left_text, data->password_subtext_id, "Password %s", state->password);
  } else {
    Text_SetText(data->left_text, data->password_subtext_id, "");
  }

  if (!IsInRoom()) {
    Text_SetText(data->right_text, data->mode_subtext_id, "");
    Text_SetText(data->right_text, data->stage_mode_subtext_id, "");
    return;
  }

  // Dolphin only sends modes it knows, but they index the labels, so they're checked anyway
  u8 is_mode_known = state->mode < sizeof(mode_labels) / sizeof(mode_labels[0]);
  u8 is_stage_mode_known = state->stage_mode < sizeof(stage_labels) / sizeof(stage_labels[0]);
  Text_SetText(data->right_text, data->mode_subtext_id, is_mode_known ? mode_labels[state->mode] : "");
  Text_SetText(data->right_text, data->stage_mode_subtext_id, is_stage_mode_known ? stage_labels[state->stage_mode] : "");
}

// Shows the join happening, or why it didn't work
void UpdateJoining() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  u8 is_failed = state->connection_status == ExiSlippi_RoomStatus_FAILED;

  for (int i = 0; i < 2; i++) {
    CSBoxSelector_SetVisibility(data->char_selectors[i], false);
    Text_SetText(data->text, data->name_subtext_ids[i], "");
  }
  CSBoxSelector_SetVisibility(data->stage_selector, false);
  for (int i = 0; i < ROOM_STAGE_COUNT; i++) {
    CSBoxSelector_SetVisibility(data->stage_strike_selectors[i], false);
  }
  Text_SetText(data->text, data->vs_subtext_id, "");
  Text_SetText(data->lower_text, data->streak_subtext_id, "");
  Text_SetText(data->text, data->timer_subtext_id, "");
  Text_SetText(data->lower_text, data->watch_subtext_id, "");

  u8 error = state->connection_error < sizeof(join_errors) / sizeof(join_errors[0]) ? state->connection_error : 0;
  if (is_failed) {
    Text_SetText(data->lower_text, data->status_subtext_id, "%s", join_errors[error]);
  } else {
    Text_SetText(data->lower_text, data->status_subtext_id, "Joining room %s", state->code);
  }

  for (int i = 0; i < LIST_LINES; i++) {
    SetListLine(i, &text_color, "");
    ShowListBar(i, false);
  }

  if (data->confirm_idx >= 0) {
    ShowConfirm();
    return;
  }
  Text_SetText(data->lower_text, data->prompt_subtext_ids[0], "");
  Text_SetText(data->lower_text, data->prompt_subtext_ids[1], is_failed ? "B  Back" : "Hold B  Leave");
  Text_SetText(data->lower_text, data->prompt_subtext_ids[2], "");
}

void UpdateStage() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  Room_Side turn = TurnSide();
  u8 is_drafting = state->phase == Room_Phase_STRIKING || state->phase == Room_Phase_CHOOSING;

  for (int side = 0; side < 2; side++) {
    CSBoxSelector *bs = data->char_selectors[side];
    CSBoxSelector_SetVisibility(bs, !is_drafting);

    int member = state->sides[side];
    if (member < 0) {
      Text_SetText(data->text, data->name_subtext_ids[side], "Waiting");
      Text_SetColor(data->text, data->name_subtext_ids[side], &dim_color);
    } else {
      char name[40];
      GetDisplayName(name, member);
      Text_SetText(data->text, data->name_subtext_ids[side], "%s", name);
      Text_SetColor(data->text, data->name_subtext_ids[side], &text_color);
    }

    // Show a question mark until the side has picked. Random stays hidden until the
    // match starts
    u8 char_id = CKIND_RANDOM;
    u8 char_color = 0;
    if (member >= 0 && state->phase == Room_Phase_PLAYING) {
      char_id = state->play_char[side];
      char_color = state->play_color[side];
    } else if (member >= 0 && state->has_picked[side]) {
      char_id = state->members[member].char_id;
      char_color = state->members[member].char_color;
    }

    u8 has_char = char_id < CKIND_RANDOM;
    if (has_char) {
      CSIcon_SetMaterial(bs->icon, CSIcon_ConvertCharToMat(char_id));
      StockIcon_SetIcon(bs->icon->stock_icon, char_id, char_color);
    } else {
      CSIcon_SetMaterial(bs->icon, CSIcon_Material_Question);
    }
    CSIcon_SetStockIconVisibility(bs->icon, has_char);
    CSIcon_SetSelectState(bs->icon, turn == side && !is_drafting ? CSIcon_Select_State_Blink
                                                                 : CSIcon_Select_State_NotSelected);
  }

  if (state->sides[Room_Side_WINNER] < 0 || state->streak == 0 || is_drafting) {
    Text_SetText(data->lower_text, data->streak_subtext_id, "");
  } else if (state->streak == 1) {
    Text_SetText(data->lower_text, data->streak_subtext_id, "Won the last set");
  } else {
    Text_SetText(data->lower_text, data->streak_subtext_id, "Won %d in a row", state->streak);
  }

  // Show the stage between the players once it's decided
  u8 show_stage = state->phase == Room_Phase_PICKING || state->phase == Room_Phase_PLAYING;
  CSBoxSelector_SetVisibility(data->stage_selector, show_stage);
  CSIcon_SetMaterial(data->stage_selector->icon, stages[state->stage_idx]);
  Text_SetText(data->text, data->vs_subtext_id, show_stage || is_drafting ? "" : "VS");

  u8 is_local_turn = IsLocalTurn();
  for (int i = 0; i < ROOM_STAGE_COUNT; i++) {
    CSBoxSelector *bs = data->stage_strike_selectors[i];
    CSBoxSelector_SetVisibility(bs, is_drafting);
    CSBoxSelector_SetSelectState(
        bs, state->struck[i] ? CSBoxSelector_Select_State_Disabled_X : CSBoxSelector_Select_State_NotSelected);
    CSBoxSelector_SetHover(bs, is_local_turn && is_drafting && data->selector_idx == i);
  }

  // Same timer as ranked, only warning the player whose turn it is
  if (state->turn_seconds == 0xFF) {
    Text_SetText(data->text, data->timer_subtext_id, "");
  } else {
    Text_SetText(data->text, data->timer_subtext_id, "%d:%02d", state->turn_seconds / 60, state->turn_seconds % 60);
    Text_SetColor(data->text, data->timer_subtext_id, &text_color);
    if (is_local_turn && state->turn_seconds <= PANIC_SECONDS) {
      Text_SetColor(data->text, data->timer_subtext_id, &panic_color);
    } else if (is_local_turn && state->turn_seconds <= WARN_SECONDS) {
      Text_SetColor(data->text, data->timer_subtext_id, &warn_color);
    }
  }

  char *turn_name = turn != Room_Side_NONE ? state->members[state->sides[turn]].name : "";
  char status[64];
  if (state->connection_status == ExiSlippi_RoomStatus_RECONNECTING) {
    sprintf(status, "Reconnecting to the room");
  } else if (state->connection_status == ExiSlippi_RoomStatus_HOSTING &&
             state->connection_error == ExiSlippi_RoomError_UNAVAILABLE) {
    // The room keeps trying to register, and has no code to join by until it does
    sprintf(status, "Rooms are unavailable, nobody can join yet");
  } else if (state->watch_status == ExiSlippi_WatchStatus_CONNECTING) {
    sprintf(status, "Connecting to the match");
  } else if (state->watch_status == ExiSlippi_WatchStatus_FAILED && state->phase == Room_Phase_PLAYING) {
    sprintf(status, "Could not watch the match");
  } else if (data->notice_frames > 0 && state->host_member != 0xFF) {
    sprintf(status, "%s is now hosting", state->members[state->host_member].name);
  } else if (state->phase == Room_Phase_STRIKING) {
    sprintf(status, "%s strikes a stage", turn_name);
  } else if (state->phase == Room_Phase_CHOOSING) {
    sprintf(status, "%s chooses the stage", turn_name);
  } else if (state->phase == Room_Phase_PICKING) {
    sprintf(status, "%s is picking a character", turn_name);
  } else if (state->phase == Room_Phase_PLAYING &&
             (state->match_over || (data->is_back_from_match && LocalSide() != Room_Side_NONE))) {
    sprintf(status, "The match did not finish");
  } else if (state->phase == Room_Phase_PLAYING && data->handoff == Room_Handoff_FAILED) {
    sprintf(status, "Could not connect, trying again");
  } else if (state->phase == Room_Phase_PLAYING && LocalSide() != Room_Side_NONE) {
    sprintf(status, "Connecting to your opponent");
  } else if (state->phase == Room_Phase_PLAYING) {
    sprintf(status, "Playing on %s", stage_names[state->stage_idx]);
  } else if (state->crowned >= 0) {
    sprintf(status, "%s earned a crown", state->members[state->crowned].name);
  } else if (state->sides[Room_Side_CHALLENGER] >= 0) {
    sprintf(status, "Get ready");
  } else if (state->sides[Room_Side_WINNER] >= 0) {
    sprintf(status, "Waiting for a challenger");
  } else {
    sprintf(status, "Waiting for players");
  }
  Text_SetText(data->lower_text, data->status_subtext_id, "%s", status);

  // Watching is offered between the match and its status while it's being played, and again after a
  // watch failed
  u8 show_watch = CanWatch() && state->watch_status != ExiSlippi_WatchStatus_CONNECTING;
  Text_SetText(data->lower_text, data->watch_subtext_id, show_watch ? "Y  Watch" : "");
}

void UpdatePrompts() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  if (data->confirm_idx >= 0) {
    ShowConfirm();
    return;
  }

  char *prompts[3] = {"START  Join the queue", "Hold B  Leave the room", ""};
  if (IsWaitingToPlay()) {
    prompts[0] = "Hold Z  Leave the queue";
    prompts[2] = "START  Practice";
  } else if (CanChangeColor()) {
    prompts[0] = "X Y  Costume";
    prompts[2] = "Z  Random costume";
  } else if (LocalSide() != Room_Side_NONE) {
    prompts[0] = "";
  }
#ifdef LOCAL_TESTING
  prompts[2] = state->phase == Room_Phase_PLAYING ? "L R  Pick the winner" : "L  Add a player";
#endif
  for (int i = 0; i < 3; i++) {
    Text_SetText(data->lower_text, data->prompt_subtext_ids[i], prompts[i]);
  }
}

void UpdateList() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  int line = 0;
  char name[40];

  for (int i = 0; i < LIST_LINES; i++) {
    ShowListBar(i, false);
  }

  SetListLine(line++, &text_color, "Up next");

  if (state->queue_count == 0) {
    SetListLine(line++, &dim_color, "Nobody in the queue");
  }
  for (int i = 0; i < state->queue_count && line < LIST_LINES - 2; i++) {
    char entry[48];
    GetDisplayName(name, state->queue[i]);
    sprintf(entry, "%d  %s", i + 1, name);
    SetListLine(line, &text_color, entry);
    ShowListBar(line++, true);
  }

  // Member count beside the heading. 0x815e is the slash in Melee's font
  char lobby[24];
  sprintf(lobby, "Lobby  %d\x81\x5e%d", state->member_count, state->capacity);
  SetListLine(line++, &text_color, lobby);

  // Members who aren't playing or queued
  for (int i = 0; i < state->member_count && line < LIST_LINES; i++) {
    if (QueuePos(i) >= 0 || IsOnSide(i)) {
      continue;
    }
    GetDisplayName(name, i);
    SetListLine(line, &text_color, name);
    ShowListBar(line++, true);
  }

  while (line < LIST_LINES) {
    SetListLine(line++, &text_color, "");
  }
}

// Keep the character picker open during the local player's pick, and close it if the
// turn ends without one, such as when time runs out
void UpdateCharPicker() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  u8 is_picking = state->phase == Room_Phase_PICKING && IsLocalTurn() &&
                  state->connection_status != ExiSlippi_RoomStatus_RECONNECTING;
  u8 is_open = data->char_picker_dialog->state.is_open;

  // B backs out of the picker, so it stays closed while B is held to leave or a leave is confirmed
  u8 is_leaving = (Pad_GetHeld(R13_U8(-0x5108)) & HSD_BUTTON_B) || data->confirm_idx >= 0;

  if (is_picking && !is_open && !is_leaving) {
    ExiSlippi_RoomMember *m = &state->members[state->local_member];
    CharPickerDialog_OpenDialog(data->char_picker_dialog, m->char_id, m->char_color);
  } else if (!is_picking && is_open) {
    CharPickerDialog_CloseDialog(data->char_picker_dialog);
  }
}

// Once the room's match is on, find the opponent the same way direct does, send the room's
// picks, then go to the match once both players are ready
void HandleMatchHandoff() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  Room_Side side = LocalSide();
  if (state->phase != Room_Phase_PLAYING || side == Room_Side_NONE) {
    // The set can end while this player is still looking for their opponent, such as when the
    // opponent drops. The search has to stop here, or the next match starts a second one on top of it
    if (data->handoff == Room_Handoff_SEARCHING || data->handoff == Room_Handoff_CONNECTED) {
      CleanupConnection();
    }
    data->handoff = Room_Handoff_NONE;
    data->is_back_from_match = false;
    return;
  }

  // A match that ended without a result is replayed by the room after a moment, or settled if the
  // opponent dropped. Searching for them meanwhile could start a match the room isn't tracking
  if (data->is_back_from_match) {
    return;
  }

#ifdef LOCAL_TESTING
  // Test players without a real account are played with L and R instead
  ExiSlippi_RoomMember *opp = &state->members[state->sides[!side]];
  if (memcmp(opp->connect_code, "TEST", 4) == 0) {
    return;
  }
#endif

  data->handoff_frames++;

  switch (data->handoff) {
    case Room_Handoff_NONE:
      FindOpponent();
      data->handoff = Room_Handoff_SEARCHING;
      data->handoff_frames = 0;
      break;
    case Room_Handoff_SEARCHING:
      data->match_state = ExiSlippi_LoadMatchState(data->match_state);
      if (data->match_state->mm_state == ExiSlippi_MmState_CONNECTION_SUCCESS) {
        SetMatchSelections();
        data->handoff = Room_Handoff_CONNECTED;
      } else if (data->match_state->mm_state == ExiSlippi_MmState_ERROR_ENCOUNTERED ||
                 data->handoff_frames > CONNECT_TIMEOUT_FRAMES) {
        CleanupConnection();
        data->handoff = Room_Handoff_FAILED;
        data->handoff_frames = 0;
        UpdateStage();
      }
      break;
    case Room_Handoff_CONNECTED:
      data->match_state = ExiSlippi_LoadMatchState(data->match_state);
      if (data->match_state->is_local_player_ready && data->match_state->is_remote_player_ready) {
        data->scene_data->start_match = true;
        data->should_exit = true;
      } else if (data->match_state->mm_state != ExiSlippi_MmState_CONNECTION_SUCCESS) {
        CleanupConnection();
        data->handoff = Room_Handoff_FAILED;
        data->handoff_frames = 0;
        UpdateStage();
      }
      break;
    case Room_Handoff_FAILED:
      if (data->handoff_frames >= CONNECT_RETRY_FRAMES) {
        data->handoff = Room_Handoff_NONE;
        UpdateStage();
      }
      break;
  }
}

void FindOpponent() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  ExiSlippi_RoomMember *opp = &state->members[state->sides[!LocalSide()]];

  ExiSlippi_FindOpponent_Query *q = calloc(sizeof(ExiSlippi_FindOpponent_Query));
  q->command = ExiSlippi_Command_FIND_OPPONENT;
  q->mode = ExiSlippi_OnlineMode_DIRECT;
  memcpy(q->connect_code, opp->connect_code, sizeof(opp->connect_code));
  ExiSlippi_Transfer(q, sizeof(ExiSlippi_FindOpponent_Query), ExiSlippi_TransferMode_WRITE);
}

// Both players send the room's stage, and Pokemon Stadium is never frozen
void SetMatchSelections() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  Room_Side side = LocalSide();

  ExiSlippi_SetSelections_Query *q = calloc(sizeof(ExiSlippi_SetSelections_Query));
  q->command = ExiSlippi_Command_SET_MATCH_SELECTIONS;
  q->team_id = 0;
  q->char_id = state->play_char[side];
  q->char_color_id = state->play_color[side];
  q->char_option = ExiSlippi_SelectionOption_MERGE;
  q->stage_id = CSIcon_ConvertMatToStage(stages[state->stage_idx]);
  q->stage_option = ExiSlippi_SelectionOption_MERGE;
  q->online_mode = ExiSlippi_OnlineMode_DIRECT;
  q->alt_stage_mode = 0;
  ExiSlippi_Transfer(q, sizeof(ExiSlippi_SetSelections_Query), ExiSlippi_TransferMode_WRITE);
}

void CleanupConnection() {
  ExiSlippi_CleanupConnection_Query *q = calloc(sizeof(ExiSlippi_CleanupConnection_Query));
  q->command = ExiSlippi_Command_CLEANUP_CONNECTION;
  ExiSlippi_Transfer(q, sizeof(ExiSlippi_CleanupConnection_Query), ExiSlippi_TransferMode_WRITE);
}

u8 GetVanilaMaxColors(u8 charId) {
  // Returns the maximum number of colors for a character in vanilla Melee. Same as ranked
  switch (charId) {
    case CKIND_FALCON:
    case CKIND_KIRBY:
    case CKIND_YOSHI:
      return 6;
    case CKIND_DK:
    case CKIND_LINK:
    case CKIND_MARIO:
    case CKIND_MARTH:
    case CKIND_PEACH:
    case CKIND_JIGGLYPUFF:
    case CKIND_SAMUS:
    case CKIND_ZELDA:
    case CKIND_SHEIK:
    case CKIND_YOUNGLINK:
    case CKIND_DRMARIO:
    case CKIND_ROY:
    case CKIND_GANONDORF:
      return 5;
    case CKIND_FOX:
    case CKIND_GAW:
    case CKIND_BOWSER:
    case CKIND_LUIGI:
    case CKIND_MEWTWO:
    case CKIND_NESS:
    case CKIND_PIKACHU:
    case CKIND_ICECLIMBERS:
    case CKIND_FALCO:
    case CKIND_PICHU:
      return 4;
    default:
      return 1;  // All other characters have only one color
  }
}

u8 GetNextColor(u8 char_id, u8 color_id, int incr) {
  int max_color = GetVanilaMaxColors(char_id);
  return (color_id + incr + max_color) % max_color;
}

void OnCharSelectionComplete(CharPickerDialog *cpd, u8 is_selection) {
  if (!is_selection || !IsLocalTurn()) {
    return;
  }

  // The picker resolves random before closing, so use where its cursor was last frame
  u8 char_id = data->prev_picker_char >= CKIND_RANDOM ? CKIND_RANDOM : cpd->state.char_selection_idx;
  u8 char_color = char_id < CKIND_RANDOM ? cpd->state.char_color_idx : 0;
  SendAction(ExiSlippi_RoomAction_PICK, char_id, char_color);
  SFX_PlayCommon(CommonSound_ACCEPT);

  // Remember the local player's pick for the next room
  data->scene_data->last_char = char_id;
  data->scene_data->last_color = char_color;
}

// In the queue, or on a side with nobody to play yet, such as a winner nobody has challenged. Either
// can leave the queue with Z or practice until the room calls them up
u8 IsWaitingToPlay() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  if (QueuePos(state->local_member) >= 0) {
    return true;
  }

  Room_Side side = LocalSide();
  return side != Room_Side_NONE && state->phase == Room_Phase_WAITING && state->sides[!side] < 0;
}

// Active players can change color once they have picked a character and have someone to play. Z
// leaves the queue until then
u8 CanChangeColor() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  Room_Side side = LocalSide();
  if (side == Room_Side_NONE || state->phase == Room_Phase_PLAYING || IsWaitingToPlay()) {
    return false;
  }

  return state->has_picked[side] && state->members[state->local_member].char_id < CKIND_RANDOM;
}

// X and Y cycle colors, Z picks a random one
void HandleColorInputs(u64 downInputs) {
  ExiSlippi_RoomMember *m = &data->state->members[data->state->local_member];
  u8 char_color = m->char_color;

  if (downInputs & HSD_BUTTON_X) {
    char_color = GetNextColor(m->char_id, char_color, 1);
  } else if (downInputs & HSD_BUTTON_Y) {
    char_color = GetNextColor(m->char_id, char_color, -1);
  } else if (downInputs & HSD_TRIGGER_Z) {
    char_color = HSD_Randi(GetVanilaMaxColors(m->char_id));
  } else {
    return;
  }

  SendAction(ExiSlippi_RoomAction_SET_COLOR, char_color, 0);
  SFX_PlayCommon(CommonSound_NEXT);
  data->scene_data->last_color = char_color;
}

// Leaving the queue (Z) or the room (B) requires holding the button. The prompt fades to
// yellow while it's held
void HandleHoldInputs(u64 heldInputs) {
  int idx = -1;
  if (heldInputs & HSD_TRIGGER_Z && IsWaitingToPlay()) {
    idx = 0;
  } else if (heldInputs & HSD_BUTTON_B) {
    idx = 1;
  }

  if (idx != data->hold_idx) {
    data->hold_idx = idx;
    data->hold_frames = 0;
  }

  for (int i = 0; i < 3; i++) {
    GXColor col = text_color;
    if (i == idx) {
      float t = (float)data->hold_frames / HOLD_FRAMES;
      col.r += (warn_color.r - text_color.r) * t;
      col.g += (warn_color.g - text_color.g) * t;
      col.b += (warn_color.b - text_color.b) * t;
    }
    Text_SetColor(data->lower_text, data->prompt_subtext_ids[i], &col);
  }

  if (idx < 0 || ++data->hold_frames < HOLD_FRAMES) {
    return;
  }

  data->hold_idx = -1;
  data->confirm_idx = idx;
  SFX_PlayCommon(CommonSound_NEXT);
  ShowConfirm();
}

// A finished hold is only acted on once A confirms it
void ShowConfirm() {
  char *actions[2] = {"A  Leave the queue", "A  Leave the room"};
  Text_SetText(data->lower_text, data->prompt_subtext_ids[0], actions[data->confirm_idx]);
  Text_SetText(data->lower_text, data->prompt_subtext_ids[1], "B  Stay");
  Text_SetText(data->lower_text, data->prompt_subtext_ids[2], "");
  for (int i = 0; i < 3; i++) {
    Text_SetColor(data->lower_text, data->prompt_subtext_ids[i], &text_color);
  }
}

void HandleConfirmInputs(u64 downInputs) {
  int idx = data->confirm_idx;
  if (!(downInputs & (HSD_BUTTON_A | HSD_BUTTON_B))) {
    return;
  }

  data->confirm_idx = -1;
  SFX_PlayCommon(CommonSound_BACK);
  if (downInputs & HSD_BUTTON_A && idx == 0) {
    SendAction(ExiSlippi_RoomAction_LEAVE_QUEUE, 0, 0);
  } else if (downInputs & HSD_BUTTON_A) {
    LeaveRoom();
    return;
  }

  if (IsInRoom()) {
    UpdatePrompts();
  } else {
    UpdateJoining();
  }
}

void HandleStageInputs(u64 downInputs, u64 scrollInputs) {
  ExiSlippi_GetRoomState_Response *state = data->state;
  int dir = 0;
  if (scrollInputs & (HSD_BUTTON_RIGHT | HSD_BUTTON_DPAD_RIGHT)) {
    dir = 1;
  } else if (scrollInputs & (HSD_BUTTON_LEFT | HSD_BUTTON_DPAD_LEFT)) {
    dir = -1;
  }

  // Skip over struck stages
  if (dir != 0) {
    do {
      data->selector_idx = (data->selector_idx + dir + ROOM_STAGE_COUNT) % ROOM_STAGE_COUNT;
    } while (state->struck[data->selector_idx]);
    SFX_PlayCommon(CommonSound_NEXT);
    UpdateStage();
  } else if (downInputs & HSD_BUTTON_A && state->phase == Room_Phase_STRIKING) {
    SendAction(ExiSlippi_RoomAction_STRIKE, data->selector_idx, 0);
    SFX_PlayCommon(CommonSound_NEXT);
  } else if (downInputs & HSD_BUTTON_A) {
    SendAction(ExiSlippi_RoomAction_CHOOSE, data->selector_idx, 0);
    SFX_PlayCommon(CommonSound_ACCEPT);
  }
}

// Anyone not playing can watch the match the room is playing, but not one that already ended
u8 CanWatch() {
  ExiSlippi_GetRoomState_Response *state = data->state;
  return state->phase == Room_Phase_PLAYING && !state->match_over && LocalSide() == Room_Side_NONE &&
         state->connection_status != ExiSlippi_RoomStatus_RECONNECTING;
}

void WatchMatch() {
  ExiSlippi_RoomWatch_Query *q = calloc(sizeof(ExiSlippi_RoomWatch_Query));
  q->command = ExiSlippi_Command_ROOM_WATCH;
  ExiSlippi_Transfer(q, sizeof(ExiSlippi_RoomWatch_Query), ExiSlippi_TransferMode_WRITE);
  SFX_PlayCommon(CommonSound_ACCEPT);
}

// Players waiting to play can practice meanwhile. Practice ends itself once the room calls them up
void StartPractice() {
  data->scene_data->start_practice = true;
  SFX_PlayCommon(CommonSound_ACCEPT);
  data->should_exit = true;
}

void LeaveRoom() {
  if (data->handoff != Room_Handoff_NONE) {
    CleanupConnection();
  }

  SendAction(ExiSlippi_RoomAction_LEAVE_ROOM, 0, 0);
  data->should_exit = true;
}

void TextCObjThink(GOBJ *gobj) {
  COBJ *cobj = gobj->hsd_object;

  if (!CObj_SetCurrent(cobj)) {
    return;
  }

  CObj_RenderGXLinks(gobj, 7);
  CObj_EndCurrent();
}

void CObjThink(GOBJ *gobj) {
  COBJ *cobj = gobj->hsd_object;

  if (!CObj_SetCurrent(cobj)) {
    return;
  }

  CObj_SetEraseColor(0, 0, 0, 255);
  CObj_EraseScreen(cobj, 1, 0, 1);
  CObj_RenderGXLinks(gobj, 7);
  CObj_EndCurrent();
}

void InputsThink(GOBJ *gobj) {
  ExiSlippi_GetRoomState_Response *state = data->state;
  u8 port = R13_U8(-0x5108);
  u64 downInputs = Pad_GetDown(port);
  u64 scrollInputs = Pad_GetRapidHeld(port);

  if (data->should_exit) {
    return;
  }

  if (data->confirm_idx >= 0) {
    HandleConfirmInputs(downInputs);
    return;
  }

  // Only leaving is possible until the room is joined
  if (!IsInRoom()) {
    if (state->connection_status == ExiSlippi_RoomStatus_FAILED && downInputs & HSD_BUTTON_B) {
      SFX_PlayCommon(CommonSound_BACK);
      LeaveRoom();
    } else {
      HandleHoldInputs(Pad_GetHeld(port));
    }
    return;
  }

  // Only leaving is possible while the room is being found again
  if (state->connection_status == ExiSlippi_RoomStatus_RECONNECTING) {
    HandleHoldInputs(Pad_GetHeld(port));
    return;
  }

  // The picker handles its own inputs. This runs before the picker, so save its cursor
  // before a press resolves random
  if (data->char_picker_dialog->state.is_open) {
    data->prev_picker_char = data->char_picker_dialog->state.char_selection_idx;
    HandleHoldInputs(0);
    return;
  }

  u8 is_drafting = state->phase == Room_Phase_STRIKING || state->phase == Room_Phase_CHOOSING;
  if (is_drafting && IsLocalTurn()) {
    HandleStageInputs(downInputs, scrollInputs);
  }

  if (CanChangeColor()) {
    HandleColorInputs(downInputs);
  }

  if (downInputs & HSD_BUTTON_START && QueuePos(state->local_member) < 0 && !IsOnSide(state->local_member)) {
    SendAction(ExiSlippi_RoomAction_JOIN_QUEUE, 0, 0);
    SFX_PlayCommon(CommonSound_ACCEPT);
  } else if (downInputs & HSD_BUTTON_START && IsWaitingToPlay()) {
    StartPractice();
  }

  if (downInputs & HSD_BUTTON_Y && CanWatch() && state->watch_status != ExiSlippi_WatchStatus_CONNECTING) {
    WatchMatch();
  }

  HandleHoldInputs(Pad_GetHeld(port));

#ifdef LOCAL_TESTING
  // L and R pick the winner during a match, otherwise L adds a test player
  if (state->phase == Room_Phase_PLAYING && downInputs & (HSD_TRIGGER_L | HSD_TRIGGER_R)) {
    SendAction(ExiSlippi_RoomAction_FINISH_SET, downInputs & HSD_TRIGGER_L ? Room_Side_WINNER : Room_Side_CHALLENGER, 0);
    SFX_PlayCommon(CommonSound_ACCEPT);
  } else if (downInputs & HSD_TRIGGER_L) {
    SendAction(ExiSlippi_RoomAction_ADD_TEST_PLAYER, 0, 0);
    SFX_PlayCommon(CommonSound_ACCEPT);
  }
#endif
}
