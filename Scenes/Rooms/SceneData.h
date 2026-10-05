#ifndef SLIPPI_ROOMS_SCENE_DATA_H
#define SLIPPI_ROOMS_SCENE_DATA_H

#include "../../m-ex/MexTK/mex.h"

// Shared by the rooms list and room scenes. This struct is defined in asm via directives
// and has no padding, so we need to use pack(1)
#pragma pack(1)
typedef struct Rooms_SceneData {
  u8 enter_room;
  u8 start_match;  // Set by the room when its match is ready to start
  u8 last_char;    // Local player's last pick, kept between rooms
  u8 last_color;
  u8 start_practice;  // Set by the room when a queued player goes to practice
  u8 start_watch;     // Set by the room when a watch of its match is ready
} Rooms_SceneData;
#pragma pack()

#endif  // SLIPPI_ROOMS_SCENE_DATA_H
