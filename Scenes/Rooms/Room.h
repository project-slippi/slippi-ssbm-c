#ifndef SLIPPI_ROOM_H
#define SLIPPI_ROOM_H

#include "../../Common.h"
#include "../../Components/CharPickerDialog.h"
#include "../../Components/CharStageBoxSelector.h"
#include "../../ExiSlippi.h"
#include "../../Files.h"
#include "../../m-ex/MexTK/mex.h"
#include "SceneData.h"

// Frame of the panels animation where the panels are fully open
#define PANELS_OPEN_FRAME 150

// Panel joint indices, depth first. Moving the frame's corner joints resizes the inner
// frame without stretching its corners
#define PANELS_MIDDLE_JOINT 1
#define PANELS_FRAME_TOP_RIGHT 6
#define PANELS_FRAME_BOTTOM_LEFT 7
#define PANELS_FRAME_TOP_LEFT 8
#define PANELS_FRAME_BOTTOM_RIGHT 9
#define PANELS_TOP_JOINT 10
#define PANELS_BOTTOM_JOINT 15

// The middle panel is split into a stage frame and a list frame
#define STAGE_LEFT -25
#define STAGE_RIGHT 6.5
#define STAGE_CENTER_X ((STAGE_LEFT + STAGE_RIGHT) / 2)
#define LIST_LEFT 8.5
#define LIST_RIGHT 25

#define STAGE_BOX_LEFT_X -15.5
#define STAGE_BOX_RIGHT_X -3
#define STAGE_BOX_Y 2.5
#define STRIKE_BOX_GAP 5.2

#define LIST_LINES 9
#define LIST_LINE_GAP 250
#define LIST_TEXT_INSET_X 110

// Bars behind the players in the list, made from the character picker's box by moving its corners
#define LIST_BAR_CENTER_X 16.75
#define LIST_BAR_HALF_WIDTH 7.4
#define LIST_BAR_HALF_HEIGHT 0.45
#define LIST_BAR_Y_OFFSET 0.55
#define LIST_BAR_Z 0.2
#define LIST_TEXT_GXLINK 5

// The list's text gets a dark shadow and a second pass a little to the right, which thickens
// Melee's thin font against the moving background
#define LIST_SHADOW_OFFSET 9
#define LIST_BOLD_OFFSET 5
#define PROMPT_GAP_X 2000

#define HOLD_FRAMES 60
#define NOTICE_FRAMES (60 * 4)
#define WARN_SECONDS 10
#define PANIC_SECONDS 3

// Slippi's matchmaking can accept a search and never answer it, so a search that takes
// this long is given up on and tried again
#define CONNECT_TIMEOUT_FRAMES (60 * 30)
#define CONNECT_RETRY_FRAMES (60 * 3)

typedef enum Room_Side {
  Room_Side_NONE = -1,
  Room_Side_WINNER,
  Room_Side_CHALLENGER,
} Room_Side;

// Same order as Dolphin's SlippiRoom::Phase
typedef enum Room_Phase {
  Room_Phase_WAITING,
  Room_Phase_STRIKING,
  Room_Phase_CHOOSING,
  Room_Phase_PICKING,
  Room_Phase_PLAYING,
} Room_Phase;

// Getting the local player into their match, the same way the CSS does for direct
typedef enum Room_Handoff {
  Room_Handoff_NONE,
  Room_Handoff_SEARCHING,
  Room_Handoff_CONNECTED,
  Room_Handoff_FAILED,
} Room_Handoff;

typedef struct Room_Data {
  Rooms_SceneData *scene_data;
  ExiSlippi_GetRoomState_Query *state_query;
  ExiSlippi_GetRoomState_Response *state;  // The room as Dolphin runs it
  ExiSlippi_GetRoomState_Response *prev_state;
  ExiSlippi_RoomAction_Query *action_query;
  ExiSlippi_MatchState_Response *match_state;
  Text *text;
  Text *lower_text;  // The streak, status, watch line and prompts, under the character picker
  Text *left_text;
  Text *right_text;
  Text *list_text;
  Text *list_shadow_text;
  Text *list_bold_text;
  JOBJ *list_bars[LIST_LINES];
  u8 is_list_hidden;  // While the character picker is open
  int name_subtext_ids[2];
  int password_subtext_id;
  int code_subtext_id;
  int mode_subtext_id;
  int stage_mode_subtext_id;
  int timer_subtext_id;
  int streak_subtext_id;
  int vs_subtext_id;
  int status_subtext_id;
  int watch_subtext_id;  // Under the status while the room's match can be watched
  int prompt_subtext_ids[3];
  int hold_idx;  // Prompt being held, -1 if none
  int hold_frames;
  int confirm_idx;    // Finished hold waiting for A, -1 if none
  int notice_frames;  // Showing who hosts now after the host changed
  CSBoxSelector *char_selectors[2];
  CSBoxSelector *stage_selector;
  CSBoxSelector *stage_strike_selectors[ROOM_STAGE_COUNT];
  CharPickerDialog *char_picker_dialog;
  int selector_idx;
  u8 prev_picker_char;
  Room_Handoff handoff;
  int handoff_frames;
  u8 is_back_from_match;  // Back from the room's match, which the room is still settling
  u8 should_exit;
} Room_Data;

JOBJ *LoadPanels();
JOBJ *GetJoint(JOBJ *root, int index);
void HideJoint(JOBJ *jobj);
JOBJ *LoadListBar(int line);
void ShowListBar(int line, u8 is_shown);
void SetListLine(int line, const GXColor *color, char *str);
void SetFrameSides(JOBJ *panels, float left, float right);
Text *CreateText(int canvas, u8 align);
int AddSubtext(Text *text, float x, float y, float scale, char *str);
void InitHeader();
void InitStage();
void InitList();
void InitPrompts();
void FetchState();
void SendAction(u8 action, u8 value0, u8 value1);
void OnStateChange();
int QueuePos(int member);
u8 IsOnSide(int member);
Room_Side LocalSide();
Room_Side TurnSide();
u8 IsLocalTurn();
void GetDisplayName(char *out, int member);
u8 IsInRoom();
u8 HasHostChanged();
void UpdateHeader();
void UpdateJoining();
void UpdateStage();
void UpdatePrompts();
void UpdateList();
void UpdateCharPicker();
void HandleMatchHandoff();
void FindOpponent();
void SetMatchSelections();
void CleanupConnection();
u8 GetVanilaMaxColors(u8 charId);
u8 GetNextColor(u8 char_id, u8 color_id, int incr);
void OnCharSelectionComplete(CharPickerDialog *cpd, u8 is_selection);
u8 IsWaitingToPlay();
u8 CanChangeColor();
void HandleColorInputs(u64 downInputs);
void HandleHoldInputs(u64 heldInputs);
void ShowConfirm();
void HandleConfirmInputs(u64 downInputs);
void HandleStageInputs(u64 downInputs, u64 scrollInputs);
void LeaveRoom();
void StartPractice();
u8 CanWatch();
void WatchMatch();
void CObjThink(GOBJ *gobj);
void TextCObjThink(GOBJ *gobj);
void InputsThink(GOBJ *gobj);

#endif  // SLIPPI_ROOM_H
