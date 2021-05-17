#include "./m-ex/MexTK/mex.h"
#include "Components/Button.h"
#include "Components/CharStageBoxSelector.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

#define MAX_SELECTORS 10

typedef struct GameSetup_State {
  int selector_idx;
  int prev_selector_idx;
  u8 selected_values_count;
  int btn_hover_idx;
} GameSetup_State;

typedef struct GameSetup_Data {
  GameSetup_State state;
  Button *buttons[2];
  CSBoxSelector *selectors[MAX_SELECTORS];
  int selector_count;
  int button_count;
} GameSetup_Data;

void CObjThink(GOBJ *gobj);
void InputsThink(GOBJ *gobj);
GOBJ *InitSelector(float x, float y, int stageIdx);
void InitSelectors();
void InitState();
void ResetButtonState();