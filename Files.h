#ifndef FILES_H
#define FILES_H

#include "./m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef struct GUI_GameSetup {
  JOBJSet **jobjs;
  COBJDesc **cobjs;
  void **lights;
  void **fog;
} GUI_GameSetup;

typedef enum GUI_GameSetup_JOBJ {
  GUI_GameSetup_JOBJ_Button = 2,
  GUI_GameSetup_JOBJ_Panels = 4,
  GUI_GameSetup_JOBJ_Background = 5,
  GUI_GameSetup_JOBJ_CSBoxSelector = 6,
  GUI_GameSetup_JOBJ_CSIcon = 7,
  GUI_GameSetup_JOBJ_X = 8,
  GUI_GameSetup_JOBJ_BUTTON_OK = 9,
  GUI_GameSetup_JOBJ_BUTTON_REDO = 10,
} GUI_GameSetup_JOBJ;

#endif