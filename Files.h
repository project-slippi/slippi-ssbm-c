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
  GUI_GameSetup_JOBJ_Background,
  GUI_GameSetup_JOBJ_Panels,
  GUI_GameSetup_JOBJ_Button,
  GUI_GameSetup_JOBJ_CSBoxSelector,
  GUI_GameSetup_JOBJ_CSIcon,
  GUI_GameSetup_JOBJ_X,
} GUI_GameSetup_JOBJ;

#endif