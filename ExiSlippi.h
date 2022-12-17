#ifndef EXI_SLIPPI_H
#define EXI_SLIPPI_H

#include "./m-ex/MexTK/mex.h"

typedef enum ExiSlippi_Command {
  ExiSlippi_Command_SET_MATCH_SELECTIONS = 0xB5,
  ExiSlippi_Command_CLEANUP_CONNECTION = 0xBA,
  ExiSlippi_Command_OVERWRITE_SELECTIONS = 0xBF,
  ExiSlippi_Command_GP_COMPLETE_STEP = 0xC0,
  ExiSlippi_Command_GP_FETCH_STEP = 0xC1,
  ExiSlippi_Command_REPORT_SET_COMPLETE = 0xC2,
} ExiSlippi_Command;

typedef enum ExiSlippi_TransferMode {
  ExiSlippi_TransferMode_READ = 0,
  ExiSlippi_TransferMode_WRITE = 1,
} ExiSlippi_TransferMode;

typedef enum ExiSlippi_SelectionOption {
  ExiSlippi_SelectionOption_UNSET,
  ExiSlippi_SelectionOption_MERGE,
  ExiSlippi_SelectionOption_CLEAR,   // Unsupported
  ExiSlippi_SelectionOption_RANDOM,  // Supported for stage only
} ExiSlippi_SelectionOption;

typedef enum ExiSlippi_MmState {
  ExiSlippi_MmState_UNSET,
  ExiSlippi_MmState_INITIALIZING,
  ExiSlippi_MmState_MATCHMAKING,
  ExiSlippi_MmState_OPPONENT_CONNECTING,
  ExiSlippi_MmState_CONNECTION_SUCCESS,
  ExiSlippi_MmState_ERROR_ENCOUNTERED,
} ExiSlippi_MmState;

// Using pragma pack here will remove any structure padding which is what EXI comms expect
// https://www.geeksforgeeks.org/how-to-avoid-structure-padding-in-c/
#pragma pack(1)

typedef struct ExiSlippi_SetSelections_Query {
  u8 command;
  u8 team_id;
  u8 char_id;
  u8 char_color_id;
  u8 char_option;
  u16 stage_id;
  u8 stage_option;
  u8 online_mode;
} ExiSlippi_SetSelections_Query;

typedef struct ExiSlippi_OverwriteCharSelections {
  u8 is_set;
  u8 char_id;
  u8 char_color_id;
} ExiSlippi_OverwriteCharSelections;
typedef struct ExiSlippi_OverwriteSelections_Query {
  u8 command;
  u16 stage_id;
  ExiSlippi_OverwriteCharSelections chars[4];
} ExiSlippi_OverwriteSelections_Query;

typedef struct ExiSlippi_CompleteStep_Query {
  u8 command;
  u8 step_idx;
  u8 char_selection;
  u8 char_color_selection;
  u8 stage_selections[2];
} ExiSlippi_CompleteStep_Query;

typedef struct ExiSlippi_FetchStep_Query {
  u8 command;
  u8 step_idx;
} ExiSlippi_FetchStep_Query;

typedef struct ExiSlippi_FetchStep_Response {
  u8 is_found;
  u8 is_skip;
  u8 char_selection;
  u8 char_color_selection;
  u8 stage_selections[2];
} ExiSlippi_FetchStep_Response;

typedef struct ExiSlippi_MatchState_Response {
  u8 mm_state;
  u8 is_local_player_ready;
  u8 is_remote_player_ready;
  u8 local_player_idx;
  u8 remote_player_idx;
  u32 rng_offset;
  u8 delay_frames;
  u8 usr_chat_msg_id;
  u8 opp_chat_msg_id;
  u8 chat_msg_player_idx;
  u32 vs_left_players;
  u32 vs_right_players;
  char local_name[31];
  char p1_name[31];
  char p2_name[31];
  char p3_name[31];
  char p4_name[31];
  char opp_name[31];
  char p1_connect_code[10];
  char p2_connect_code[10];
  char p3_connect_code[10];
  char p4_connect_code[10];
  char p1_uid[29];
  char p2_uid[29];
  char p3_uid[29];
  char p4_uid[29];
  char err_msg[241];
  u8 game_info_block[0x138];
  char matchmake_id[51];
} ExiSlippi_MatchState_Response;

typedef struct ExiSlippi_CleanupConnection_Query {
  u8 command;
} ExiSlippi_CleanupConnection_Query;

typedef struct ExiSlippi_ReportCompletion_Query {
  u8 command;
  u8 end_mode;
} ExiSlippi_ReportCompletion_Query;

// Not sure if resetting is strictly needed, might be contained to the file
#pragma pack()

void ExiSlippi_Transfer(void *msg, u32 length, u8 mode);
ExiSlippi_MatchState_Response *ExiSlippi_LoadMatchState(ExiSlippi_MatchState_Response *msrb);

#endif