#include "./m-ex/MexTK/mex.h"
#include "Components/Button.h"
#include "Components/CharPickerDialog.h"
#include "Components/CharStageBoxSelector.h"
#include "Components/FlatTexture.h"
#include "Components/GameResult.h"
#include "Components/RightArrow.h"
#include "Components/TurnIndicator.h"
#include "ExiSlippi.h"

// How to auto-format code: https://stackoverflow.com/a/50489812/1249024

#define STRIKE_STAGE_SELECTOR_COUNT 5
#define CP_STAGE_SELECTOR_COUNT 6

#define GRACE_SECONDS 3
#define WARN_SECONDS 10
#define PANIC_SECONDS 3
#define WAIT_TIMEOUT_SECONDS 15

// This struct is defined in asm via directives and has no padding, so we need to use pack(1)
#pragma pack(1)
typedef struct GameSetup_SceneData {
  u8 max_games;
  u8 cur_game;
  u8 score_by_player[2];
  u8 prev_winner;
  u8 tiebreak_game_num;
  u8 game_results[9];
  u16 last_stage_win_by_player[2];
  u8 color_ban_active;
  u8 color_ban_char;
  u8 color_ban_color;
  u8 (*get_winner_idx)();
} GameSetup_SceneData;
#pragma pack()

typedef enum GameSetup_Process_Type {
  GameSetup_Process_Type_SKIP = 0,
  GameSetup_Process_Type_STAGE_STRIKING = 1,
  GameSetup_Process_Type_COUNTERPICKING = 2,
} GameSetup_Process_Type;

typedef enum GameSetup_Step_Type {
  GameSetup_Step_Type_NONE = 0,
  GameSetup_Step_Type_REMOVE_STAGE = 1,
  GameSetup_Step_Type_CHOOSE_STAGE = 2,
  GameSetup_Step_Type_CHOOSE_CHAR = 3,
  GameSetup_Step_Type_CHOOSE_COLOR = 4,
} GameSetup_Step_Type;

typedef enum GameSetup_Step_State {
  GameSetup_Step_State_PENDING = 0,
  GameSetup_Step_State_ACTIVE = 1,
  GameSetup_Step_State_COMPLETE = 2,
} GameSetup_Step_State;

typedef struct GameSetup_Step {
  u8 player_idx;
  GameSetup_Step_Type type;
  int required_selection_count;
  u32 timer_seconds;

  u8 hide_secondary_button;

  CSIcon *display_icons[2];
  FlatTexture *label;
  RightArrow *arrow;
  FlatTexture_Texture desc_tex;

  CSBoxSelector **selectors;
  int selector_count;

  u8 char_selection;
  u8 char_color_selection;
  u8 stage_selections[2];

  GameSetup_Step_State state;
} GameSetup_Step;

typedef struct GameSetup_State {
  int selector_idx;
  int prev_selector_idx;
  u8 selected_values_count;
  int btn_hover_idx;
  int step_idx;
  u8 is_complete;
  u8 should_terminate;
} GameSetup_State;

typedef struct GameSetup_Data {
  GameSetup_Process_Type process_type;
  GameSetup_State state;
  GameSetup_Step *steps;
  int step_count;
  Button *buttons[2];
  int button_count;
  FlatTexture *description;
  TurnIndicator *turn_indicators[2];
  GameResult **game_results;
  int game_result_count;
  Text *text;
  int timer_subtext_id;
  int timer_frames;
  GameSetup_Step_Type initialized_step_type;
  GameSetup_SceneData *scene_data;

  CharPickerDialog *char_picker_dialog;

  CSBoxSelector *stage_strike_selectors[STRIKE_STAGE_SELECTOR_COUNT];
  CSBoxSelector *stage_cp_selectors[CP_STAGE_SELECTOR_COUNT];
  CSBoxSelector *char_selectors[1];
  CSBoxSelector *char_wait_selectors[1];

  ExiSlippi_MatchState_Response *match_state;
  ExiSlippi_FetchStep_Query *fetch_query;
  ExiSlippi_FetchStep_Response *fetch_resp;
  ExiSlippi_CompleteStep_Query *complete_query;
} GameSetup_Data;

void CObjThink(GOBJ *gobj);
void InputsThink(GOBJ *gobj);
void HandleOpponentStep();
void InitAllSelectorJobjs();
void InitSelectorJobjs(CSIcon_Material *iconMats, CSBoxSelector **selectors, int count);
void InitState();
void InitStrikingSteps();
void InitCounterpickingSteps();
void InitSteps();
void InitHeader();
void ResetButtonState(u8 is_visible);
void CompleteCurrentStep(int committed_count);
void SetMatchSelections(u8 char_id, u8 char_color, u8 char_option, u16 stage_id, u8 stage_option);
void PrepareCurrentStep();
void CompleteGamePrep();

void OnCharSelectionComplete(CharPickerDialog *cpd, u8 is_selection);
u8 GetNextColor(u8 charId, u8 colorId, int incr);
u8 IsColorAllowed(u8 charId, u8 colorId, u8 playerIdx);

void IncrementSelectorIndex();
void DecrementSelectorIndex();
void ResetSelectorIndex();

u8 UpdateTimer();  // Returns bool indicating if time is elapsed
void UpdateHoverDisplays();
void UpdateTimeline();