#include "./m-ex/MexTK/mex.h"
#include "Components/Button.h"
#include "Components/CharStageBoxSelector.h"
#include "Components/FlatTexture.h"
#include "Components/RightArrow.h"
#include "ExiSlippi.h"

// How to auto-format code: https://stackoverflow.com/a/50489812/1249024

#define MAX_SELECTORS 10
#define GRACE_SECONDS 3
#define WAIT_TIMEOUT_SECONDS 15

typedef enum GameSetup_Process_Type {
  GameSetup_Process_Type_STAGE_STRIKING = 0,
  GameSetup_Process_Type_COUNTERPICKING = 1,
} GameSetup_Process_Type;

typedef enum GameSetup_Step_Type {
  GameSetup_Step_Type_NONE = 0,
  GameSetup_Step_Type_REMOVE_STAGE = 1,
  GameSetup_Step_Type_CHOOSE_STAGE = 2,
  GameSetup_Step_Type_CHOOSE_CHAR = 3,
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

  CSIcon *display_icons[2];
  FlatTexture *label;
  RightArrow *arrow;
  FlatTexture_Texture desc_tex;

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
  CSBoxSelector *selectors[MAX_SELECTORS];
  FlatTexture *description;
  int selector_count;
  int button_count;
  Text *text;
  int timer_subtext_id;
  int timer_frames;
  GameSetup_Step_Type initialized_step_type;

  ExiSlippi_MatchState_Response *match_state;
  ExiSlippi_FetchStep_Query *fetch_query;
  ExiSlippi_FetchStep_Response *fetch_resp;
  ExiSlippi_CompleteStep_Query *complete_query;
} GameSetup_Data;

void CObjThink(GOBJ *gobj);
void InputsThink(GOBJ *gobj);
void HandleOpponentStep();
void InitSelectorJobjs();
void InitState();
void InitSteps();
void ResetButtonState();
void CompleteCurrentStep(int committed_count);
void PrepareCurrentStep();
void CompleteGamePrep();

void IncrementSelectorIndex();
void DecrementSelectorIndex();
void ResetSelectorIndex();

u8 UpdateTimer();  // Returns bool indicating if time is elapsed
void UpdateTimeline();