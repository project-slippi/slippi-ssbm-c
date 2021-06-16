#ifndef EXI_SLIPPI_H
#define EXI_SLIPPI_H

#include "./m-ex/MexTK/mex.h"

typedef enum ExiSlippi_Command {
  ExiSlippi_Command_RESET_SELECTIONS = 0xBF,
} ExiSlippi_Command;

typedef enum ExiSlippi_TransferMode {
  ExiSlippi_TransferMode_READ = 0,
  ExiSlippi_TransferMode_WRITE = 1,
} ExiSlippi_TransferMode;

typedef struct ExiSlippi_ResetSelections_Query {
  u8 command;
} ExiSlippi_ResetSelections_Query;

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
  u8 char_selection;
  u8 char_color_selection;
  u8 stage_selections[2];
} ExiSlippi_FetchStep_Query;

void ExiSlippi_Transfer(void *msg, u32 length, u8 mode);

#endif