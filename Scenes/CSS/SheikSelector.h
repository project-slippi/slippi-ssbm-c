#include "../../Slippi.h"

typedef struct CSSIcon {
  u8 ft_hudindex;  // 0x00 - used for getting combo count @ 8025C0C4
  u8 char_kind;    // 0x01 - icons external ID
  u8 state;        // 0x02 - 0x0 Not Unlocked, 0x01 Unlocked, 0x02 Unlocked and disp
  u8 anim_timer;   // 0x03 - 0xC when char is chosen
  u8 joint_id_vs;  // 0x04 - Vs Icon Bg Jobj ID
  u8 joint_id_1p;  // 0x05 - Vs Icon Bg Jobj ID
  int sfx;         // 0x08
  float bound_l;   // 0x0C
  float bound_r;   // 0x10
  float bound_u;   // 0x14
  float bound_d;   // 0x18
} CSSIcon;

typedef struct PlayerInitData {
  s8 c_kind;     // character kind
  u8 slot_type;  // slot type (0 = human, 1 = cpu?)
  s8 stocks;     // stocks
  u8 color;      // color
  u8 slot;       // port
  s8 x5;         // spawnpos32
  s8 spawn_dir;  // spawn direction
  u8 sub_color;  // subcolor
  s8 handicap;   // handicap
  u8 team;       // team
  s8 xA;         // nametag
  u8 xB;
  u8 xC_b0 : 1;
  u8 xC_b1 : 1;
  u8 xC_b2 : 1;
  u8 xC_b3 : 1;
  u8 xC_b4 : 1;
  u8 xC_b5 : 1;
  u8 xC_b6 : 1;
  u8 xC_b7 : 1;
  u8 xD_b0 : 1;
  u8 xD_b1 : 1;
  u8 xD_b2 : 1;
  u8 xD_b3 : 1;
  u8 xD_b4 : 1;
  u8 xD_b5 : 1;
  u8 xD_b6 : 1;
  u8 xD_b7 : 1;
  u8 xE;
  u8 cpu_level;  // CPU level
  u16 x10;
  u16 x12;
  u16 x14;
  float x18;  // offense ratio
  float x1C;  // defense ratio
  float x20;
} PlayerInitData;
typedef struct StartMeleeRules {
  u32 x0_0 : 3;  // match mode? 1 = stock mode, 2 = coin mode?
  u32 x0_3 : 3;
  u32 x0_6 : 1;
  u32 x0_7 : 1;

  u32 x1_0 : 1;
  u32 x1_1 : 1;
  u32 x1_2 : 1;
  u32 x1_3 : 1;
  u32 x1_4 : 1;
  u32 x1_5 : 1;
  u32 timer_shows_hours : 1;  // false=65:00.00, true=1:05:00.00
  u32 x1_7 : 1;

  u32 x2_0 : 1;
  u32 x2_1 : 1;
  u32 x2_2 : 1;
  u32 x2_3 : 1;
  u32 x2_4 : 1;
  u32 x2_5 : 1;
  u32 x2_6 : 1;
  u32 x2_7 : 1;

  u32 x3_0 : 1;
  u32 x3_1 : 1;
  u32 x3_2 : 1;
  u32 x3_3 : 1;
  u32 x3_4 : 1;
  u32 x3_5 : 1;
  u32 x3_6 : 1;
  u32 x3_7 : 1;

  u32 x4_0 : 1;
  u32 x4_1 : 1;
  u32 x4_2 : 1;
  u32 x4_3 : 1;
  u32 x4_4 : 1;
  u32 x4_5 : 1;
  u32 x4_6 : 1;
  u32 x4_7 : 1;

  u32 x5_0 : 1;
  u32 x5_1 : 1;
  u32 x5_2 : 1;
  u32 x5_3 : 1;
  u32 x5_4 : 1;
  u32 x5_5 : 1;
  u32 x5_6 : 1;
  u32 x5_7 : 1;

  u8 x6;
  u8 x7;
  u8 x8;  // is teams
  u8 x9;
  u8 xA;
  s8 xB;  // item frequency
  s8 xC;  // SD penalty
  u8 xD;
  u16 xE;  // InternalStageId

  u32 x10;  // time limit
  u8 x14;
  u32 x18;
  u32 x1C_pad[(0x20 - 0x1C) / 4];

  u64 x20;  // item mask
  int x28;
  float x2C;
  float x30;  // damage ratio
  float x34;  // game speed
  void (*x38)(int);
  void (*x3C)(int);
  int (*x40)(void);
  void (*x44)(void);
  void (*x48)(void);
  void (*x4C)(void);
  void (*x50)(int);
  struct
  {
    u8 pad_x0[0x10];
    u8 x10_b0 : 1;
    u8 x10_b1 : 1;
  } *x54;
  int x58;
  u8 pad_x5C[0x60 - 0x5C];
} StartMeleeRules;

typedef struct StartMeleeData {
  StartMeleeRules rules;
  PlayerInitData players[6];
} StartMeleeData;

typedef struct VsModeData {
  s8 loser;
  s8 ordered_stage_index;
  s8 winner;
  u8 unk_0x3;
  u8 unk_0x4;
  u8 unk_0x5;
  u8 unk_0x6;
  u8 unk_0x7;
  StartMeleeData data;
} VsModeData;

typedef struct CSSData {
  u16 unk_0x0;  ///< 1p port?
  u8 match_type;
  u8 pending_scene_change;
  u8 *ko_star_counts;
  VsModeData data;
} CSSData;

typedef struct CSSPlayerData {
  GOBJ *gobj;
  u8 port;
  u8 state;
  u8 held_puck;
  u8 unk;
  short unk1;
  short exit_timer;
  Vec2 position;
} CSSPlayerData;

int CSS_DATA_R13_OFFSET = -0x49F0;
int PLAYER_IDX_R13_OFFSET = -0x49B0;

const float SELECTOR_CENTER_X = -13.f;
const float SELECTOR_CENTER_Y = -21.25f;

const float TAP_TARGET_WIDTH = 3.5f;
const float TAP_TARGET_HEIGHT = 3.0f;

const float ACTIVE_ALPHA = 1.0f;
const float INACTIVE_ALPHA = 0.4f;
const float HOVER_ALPHA = 0.75f;

bool isInNameEntry = false;

GOBJ *selectorGobj;
JOBJ *selectorJobj;

Vec2 GetCursorPos();
u8 GetPlayerIndex();
CSSData *GetCSSData();
u8 GetSelectedChar();
void SetSelectedChar(u8 ckind);

void InitSheikSelector();
void UpdateSheikSelector();

void *(*SFX_getCharacterNameAnnouncer)(int char_id) = 0x80168c5c;
void *(*CSS_CursorHighlightUpdateCSPInfo)(u8 port) = 0x8025db34;
