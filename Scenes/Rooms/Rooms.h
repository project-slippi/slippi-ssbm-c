#ifndef SLIPPI_ROOMS_H
#define SLIPPI_ROOMS_H

#include "../../ExiSlippi.h"
#include "../../m-ex/MexTK/mex.h"
#include "SceneData.h"

// Frame of the ToyFigurePanel animation where the trophy list is fully in view
#define PANEL_LIST_FRAME 22

// ToyFigurePanel joint indices, depth first. Pills are listed right to left, and each
// has one child whose first DOBJ is the pill and second is the word
#define PANEL_HEADER_JOINT 1
#define PANEL_COUNTER_JOINT 2
#define PANEL_SIDE_LABEL_JOINT 13
#define PANEL_ROW_1_JOINT 15
#define PANEL_ROW_2_JOINT 16
#define PANEL_PILL_RIGHT 17
#define PANEL_PILL_MIDDLE 19
#define PANEL_PILL_LEFT 21
#define PANEL_SERIES_JOINT 23

// ToyFigureListBase joint for the trophy's series logo
#define ROW_SERIES_JOINT 1

// Row text offsets and scales, taken from Melee's trophy list (tylist, 0x80312904)
#define ROW_TEXT_GXLINK 5
#define ROW_TEXT_Y 0.41
#define ROW_NAME_X -6.5
#define ROW_NAME_SCALE_X 0.028
#define ROW_COUNT_SCALE_X 0.038
#define ROW_TEXT_SCALE_Y 0.029
#define ROW_NAME_WIDTH 640
#define ROW_COUNT_WIDTH 192
#define ROW_TEXT_HEIGHT 64

// Row text positions. The code replaces the series logo and the count is right aligned.
// Counts of 10 and up are drawn narrower to fit
#define ROW_CODE_X -9.45
#define ROW_REGION_X 0.5
#define ROW_MODE_X 3.5
#define ROW_COUNT_X 11.1
#define ROW_COUNT_LONG_SCALE_X 0.030

#define PILL_COUNT 5
#define PILL_SCALE 0.817
#define PILL_SHIFT 0.149
#define LIST_ROWS 9

// The public rooms plus the room to rejoin, which the list scrolls through LIST_ROWS at a time
#define ROOM_SLOTS (ROOM_LIST_MAX + 1)

// Create and join form layout. Choices and digits are spread evenly between
// FORM_LEFT_X and FORM_RIGHT_X, and BAR_CENTER_X is the middle of a row's bar
#define FORM_LEFT_X -9.7
#define FORM_RIGHT_X 7.6
#define BAR_CENTER_X 1.1

#define CREATE_SETTINGS 4
#define CREATE_PILL_COUNT 2
#define CREATE_NARROW_SCALE_X 0.022

// Join screen. Typing fills the room code first, then the password
#define CODE_DIGITS 4
#define PASSWORD_DIGITS 4
#define ENTRY_DIGITS (CODE_DIGITS + PASSWORD_DIGITS)
#define JOIN_PILL_COUNT 2
#define JOIN_CODE_ROW 1
#define JOIN_PASSWORD_ROW 3
#define KEYPAD_FIRST_ROW 5
#define KEYPAD_ROWS 4
#define KEYPAD_COLUMNS 3
#define KEYPAD_GAP_X 4.0
#define KEYPAD_SCALE_X 0.038

// Scale of the row bar and cursor used to highlight one key
#define KEY_SCALE 0.15

// Scale of the cursor that marks the next digit's slot
#define SLOT_SCALE 0.19

// How often the public room list is refreshed while it's shown
#define LIST_REFRESH_FRAMES (60 * 5)

// Rooms from countries outside the listed regions have none
#define REGION_UNKNOWN 0xFF

typedef enum Rooms_Screen {
  Rooms_Screen_LIST,
  Rooms_Screen_CREATE,
  Rooms_Screen_JOIN,
} Rooms_Screen;

typedef enum Rooms_Focus {
  Rooms_Focus_LIST,
  Rooms_Focus_PILLS,
} Rooms_Focus;

typedef enum Rooms_PillRow {
  Rooms_PillRow_MAIN,
  Rooms_PillRow_MODE,
  Rooms_PillRow_REGION,
  Rooms_PillRow_CREATE,
  Rooms_PillRow_JOIN,
} Rooms_PillRow;

typedef enum Rooms_MainPill {
  Rooms_MainPill_CREATE,
  Rooms_MainPill_JOIN,
  Rooms_MainPill_MODE,
  Rooms_MainPill_REGION,
  Rooms_MainPill_RANDOM,
} Rooms_MainPill;

typedef enum Rooms_CreatePill {
  Rooms_CreatePill_CREATE,
  Rooms_CreatePill_CANCEL,
} Rooms_CreatePill;

typedef enum Rooms_JoinPill {
  Rooms_JoinPill_JOIN,
  Rooms_JoinPill_CANCEL,
} Rooms_JoinPill;

// Create form settings, top to bottom
typedef enum Rooms_Setting {
  Rooms_Setting_VISIBILITY,
  Rooms_Setting_MODE,
  Rooms_Setting_SIZE,
  Rooms_Setting_STAGES,
} Rooms_Setting;

typedef struct Rooms_SettingInfo {
  char *title;
  char **choices;
  int count;
  float scale_x;
} Rooms_SettingInfo;

typedef struct Rooms_Room {
  char code[5];
  char password[5];  // Only known for the room being rejoined
  char host[31];
  u8 mode;
  u8 region;
  u8 players;
  u8 capacity;
  u8 is_rejoin;  // The room left by a crash, listed first so it can be rejoined
} Rooms_Room;

typedef struct Rooms_Data {
  Rooms_SceneData *scene_data;
  ExiSlippi_FetchRoomList_Query *fetch_query;
  ExiSlippi_GetRoomList_Query *list_query;
  ExiSlippi_GetRoomList_Response *list;
  u8 list_status;
  int list_frames;
  Text *pill_text;
  int pill_subtext_ids[PILL_COUNT];
  JOBJ *pills[PILL_COUNT];
  int row_canvas;
  Text *name_text[LIST_ROWS];
  Text *mode_text[LIST_ROWS];
  Text *size_text[LIST_ROWS];
  Text *empty_text;
  Text *choice_text[CREATE_SETTINGS];
  Text *code_text;
  Text *password_text;
  Text *keypad_text[KEYPAD_ROWS];
  JOBJ *rows[LIST_ROWS];
  JOBJ *cursor;
  JOBJ *key_bar;
  JOBJ *slot_cursor;
  JOBJ *list_end;
  JOBJ *side_label;
  Vec3 row_pos;
  Vec3 row_step;
  Rooms_Screen screen;
  Rooms_Focus focus;
  Rooms_PillRow pill_row;
  int pill_idx;
  int row_idx;  // Index into visible, not the row on screen
  int scroll;   // The first visible room, shown on the top row
  int visible[ROOM_SLOTS];
  int visible_count;
  int setting_idx;
  int choices[CREATE_SETTINGS];
  u8 entry[ENTRY_DIGITS];
  int entry_len;
  int key_row;
  int key_col;
  u8 mode_filter;
  u8 region_filter;
  u8 should_exit;
} Rooms_Data;

GOBJ *LoadModel(char *joint, char *animjoint, char *matanimjoint);
JOBJ *FreezeModel(GOBJ *gobj, float frame);
JOBJ *LoadPanel();
JOBJ *GetJoint(JOBJ *root, int index);
void SetLabel(DOBJ *dobj, char *image);
void HideJoint(JOBJ *jobj);
void ShowJoint(JOBJ *jobj);
JOBJ *InitPill(JOBJ *panel, int joint, float x, float scale);
void InitRows(JOBJ *panel);
Text *CreateRowText(int row, float x, float scale_x, float width);
Text *CreateSlotText(int row, int count, float scale_x);
Text *CreateKeypadText(int row);
void SetRowPos(JOBJ *jobj, float row);
u8 IsPillOn(int idx);
u8 IsChoiceEnabled(int setting, int choice);
int PillCount();
void UpdatePills();
void ClearRow(int row);
void ClearListArea();
void PlaceModel(JOBJ *jobj, float row, float x, float scale_x);
void FilterRooms();
void UpdateList();
void UpdateCreate();
void UpdateEntry(Text *text, int row, int first, int count);
void UpdateJoin();
void OpenCreate();
void OpenJoin();
void CloseForm();
u8 HandleCreateInputs(u8 up, u8 down, u8 left, u8 right, u64 downInputs);
u8 HandleKeypadInputs(u8 up, u8 down, u8 left, u8 right, u64 downInputs);
void EnterRoom();
void JoinRoom(char *code, char *password);
void FetchRoomList();
void PollRoomList();
void PlayAlert();
void HandlePillPress();
void CObjThink(GOBJ *gobj);
void TextCObjThink(GOBJ *gobj);
void InputsThink(GOBJ *gobj);

#endif  // SLIPPI_ROOMS_H
