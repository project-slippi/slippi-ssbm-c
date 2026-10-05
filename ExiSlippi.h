#ifndef EXI_SLIPPI_H
#define EXI_SLIPPI_H

#include "./m-ex/MexTK/mex.h"

typedef enum ExiSlippi_Command {
  ExiSlippi_Command_FIND_OPPONENT = 0xB4,
  ExiSlippi_Command_SET_MATCH_SELECTIONS = 0xB5,
  ExiSlippi_Command_CLEANUP_CONNECTION = 0xBA,
  ExiSlippi_Command_OVERWRITE_SELECTIONS = 0xBF,
  ExiSlippi_Command_GP_COMPLETE_STEP = 0xC0,
  ExiSlippi_Command_GP_FETCH_STEP = 0xC1,
  ExiSlippi_Command_REPORT_SET_COMPLETE = 0xC2,
  ExiSlippi_Command_GET_PLAYER_SETTINGS = 0xC3,
  ExiSlippi_Command_REPORT_MATCH_STATUS = 0xC4,
  ExiSlippi_Command_CREATE_ROOM = 0xC5,
  ExiSlippi_Command_ROOM_ACTION = 0xC6,
  ExiSlippi_Command_GET_ROOM_STATE = 0xC7,
  ExiSlippi_Command_JOIN_ROOM = 0xC8,
  ExiSlippi_Command_FETCH_ROOM_LIST = 0xC9,
  ExiSlippi_Command_GET_ROOM_LIST = 0xCA,
  ExiSlippi_Command_ROOM_PRACTICE_OVER = 0xCB,  // Sent by the asm while practicing
  ExiSlippi_Command_ROOM_WATCH = 0xCC,
  ExiSlippi_Command_GET_RANK = 0xE3,
  ExiSlippi_Command_FETCH_RANK = 0xE4
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

typedef enum ExiSlippi_OnlineMode {
  ExiSlippi_OnlineMode_RANKED,
  ExiSlippi_OnlineMode_UNRANKED,
  ExiSlippi_OnlineMode_DIRECT,
  ExiSlippi_OnlineMode_TEAMS,
} ExiSlippi_OnlineMode;

typedef enum ExiSlippi_RoomAction {
  ExiSlippi_RoomAction_JOIN_QUEUE,
  ExiSlippi_RoomAction_LEAVE_QUEUE,
  ExiSlippi_RoomAction_STRIKE,
  ExiSlippi_RoomAction_CHOOSE,
  ExiSlippi_RoomAction_PICK,
  ExiSlippi_RoomAction_SET_COLOR,
  ExiSlippi_RoomAction_MATCH_ENDED,
  ExiSlippi_RoomAction_ADD_TEST_PLAYER,
  ExiSlippi_RoomAction_FINISH_SET,
  ExiSlippi_RoomAction_LEAVE_ROOM,
} ExiSlippi_RoomAction;

// Same values as Dolphin's SlippiRoomSession
typedef enum ExiSlippi_RoomStatus {
  ExiSlippi_RoomStatus_HOSTING,
  ExiSlippi_RoomStatus_JOINING,
  ExiSlippi_RoomStatus_JOINED,
  ExiSlippi_RoomStatus_FAILED,
  ExiSlippi_RoomStatus_RECONNECTING,  // The host changed and the room is being rejoined
} ExiSlippi_RoomStatus;

typedef enum ExiSlippi_RoomError {
  ExiSlippi_RoomError_NONE,
  ExiSlippi_RoomError_UNAVAILABLE,
  ExiSlippi_RoomError_NOT_FOUND,
  ExiSlippi_RoomError_WRONG_PASSWORD,
  ExiSlippi_RoomError_FULL,
  ExiSlippi_RoomError_LOCKED,
  ExiSlippi_RoomError_UNREACHABLE,
  ExiSlippi_RoomError_DISCONNECTED,
  ExiSlippi_RoomError_REJECTED,
  ExiSlippi_RoomError_IDLE,  // The room closed after an hour without activity
} ExiSlippi_RoomError;

typedef enum ExiSlippi_WatchStatus {
  ExiSlippi_WatchStatus_NONE,
  ExiSlippi_WatchStatus_CONNECTING,
  ExiSlippi_WatchStatus_READY,
  ExiSlippi_WatchStatus_FAILED,
} ExiSlippi_WatchStatus;

typedef enum ExiSlippi_RoomListStatus {
  ExiSlippi_RoomListStatus_FETCHING,
  ExiSlippi_RoomListStatus_FETCHED,
  ExiSlippi_RoomListStatus_FAILED,
} ExiSlippi_RoomListStatus;

#define ROOM_MAX_MEMBERS 32
#define ROOM_STAGE_COUNT 6
#define ROOM_LIST_MAX 50  // As many as the directory lists

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
  u8 alt_stage_mode;
} ExiSlippi_SetSelections_Query;

typedef struct ExiSlippi_FindOpponent_Query {
  u8 command;
  u8 mode;
  char connect_code[18];
} ExiSlippi_FindOpponent_Query;

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
  s8 p1_rank;
  s8 p2_rank;
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

typedef struct ExiSlippi_CreateRoom_Query {
  u8 command;
  u8 visibility;
  u8 mode;
  u8 capacity;
  u8 stage_mode;
  u8 last_char;
  u8 last_color;
} ExiSlippi_CreateRoom_Query;

typedef struct ExiSlippi_RoomAction_Query {
  u8 command;
  u8 action;
  u8 value[2];
} ExiSlippi_RoomAction_Query;

typedef struct ExiSlippi_GetRoomState_Query {
  u8 command;
} ExiSlippi_GetRoomState_Query;

typedef struct ExiSlippi_RoomMember {
  char name[31];
  char connect_code[10];
  u8 char_id;
  u8 char_color;
  u8 crowns;
} ExiSlippi_RoomMember;

typedef struct ExiSlippi_GetRoomState_Response {
  u8 is_active;
  u8 connection_status;
  u8 connection_error;
  u8 visibility;
  u8 mode;
  u8 capacity;
  u8 stage_mode;
  char code[5];      // Empty until the room is registered
  char password[5];  // Empty for public rooms
  u8 local_member;
  u8 member_count;
  ExiSlippi_RoomMember members[ROOM_MAX_MEMBERS];
  u8 queue_count;
  u8 queue[ROOM_MAX_MEMBERS];
  s8 sides[2];
  u8 streak;
  s8 crowned;
  u8 phase;
  u8 struck[ROOM_STAGE_COUNT];
  u8 stage_idx;
  u8 has_picked[2];
  u8 play_char[2];
  u8 play_color[2];
  u8 turn_seconds;  // 0xFF when it's nobody's turn
  u8 host_member;   // 0xFF while nobody is hosting
  u8 watch_status;  // ExiSlippi_WatchStatus
  u8 match_over;    // The match ended without a result, and the room replays it shortly
} ExiSlippi_GetRoomState_Response;

typedef struct ExiSlippi_RoomWatch_Query {
  u8 command;
} ExiSlippi_RoomWatch_Query;

typedef struct ExiSlippi_JoinRoom_Query {
  u8 command;
  char code[5];
  char password[5];  // Empty when joining a public room
  u8 last_char;
  u8 last_color;
} ExiSlippi_JoinRoom_Query;

typedef struct ExiSlippi_FetchRoomList_Query {
  u8 command;
} ExiSlippi_FetchRoomList_Query;

typedef struct ExiSlippi_GetRoomList_Query {
  u8 command;
} ExiSlippi_GetRoomList_Query;

typedef struct ExiSlippi_RoomListing {
  char code[5];
  char host_name[31];
  u8 mode;
  u8 stage_mode;
  u8 capacity;
  u8 member_count;
  u8 region;  // 0xFF when unknown
} ExiSlippi_RoomListing;

typedef struct ExiSlippi_GetRoomList_Response {
  u8 status;
  u8 count;
  ExiSlippi_RoomListing rooms[ROOM_LIST_MAX];
  char rejoin_code[5];  // The last room, after a crash. Empty when there's nothing to rejoin
  char rejoin_password[5];
} ExiSlippi_GetRoomList_Response;

typedef struct ExiSlippi_CleanupConnection_Query {
  u8 command;
} ExiSlippi_CleanupConnection_Query;

typedef struct ExiSlippi_ReportCompletion_Query {
  u8 command;
  u8 end_mode;
} ExiSlippi_ReportCompletion_Query;

typedef struct ExiSlippi_ReportMatchStatus_Query {
  u8 command;
  u8 statusIdx;  // Maps to a string on Dolphin side
} ExiSlippi_ReportMatchStatus_Query;

typedef struct ExiSlippi_GetPlayerSettings_Query {
  u8 command;
} ExiSlippi_GetPlayerSettings_Query;

typedef struct PlayerSettings {
  char chatMessages[16][51];
} PlayerSettings;

typedef struct ExiSlippi_GetPlayerSettings_Response {
  PlayerSettings settings[4];
} ExiSlippi_GetPlayerSettings_Response;

typedef enum RankInfo_FetchStatus {
  RankInfo_FetchStatus_FETCHING = 0,
  RankInfo_FetchStatus_FETCHED = 1,
  RankInfo_FetchStatus_ERROR = 2,
} RankInfo_FetchStatus;

typedef struct ExiSlippi_FetchRank_Query {
  u8 command;
} ExiSlippi_FetchRank_Query;

typedef struct ExiSlippi_GetRank_Query {
  u8 command;
} ExiSlippi_GetRank_Query;

typedef struct ExiSlippi_GetRank_Response {
  u8 visibility;
  u8 status;
  s8 rank;
  float ratingOrdinal;
  u32 ratingUpdateCount;
  float ratingChange;
  s8 rankChange;
} ExiSlippi_GetRank_Response;

// Not sure if resetting is strictly needed, might be contained to the file
#pragma pack()

void ExiSlippi_Transfer(void *msg, u32 length, u8 mode);
ExiSlippi_MatchState_Response *ExiSlippi_LoadMatchState(ExiSlippi_MatchState_Response *msrb);

#endif
