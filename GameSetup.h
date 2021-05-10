#include "./m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef struct GuiData {
  JOBJSet **jobjs;
  COBJDesc **cobjs;
  void **lights;
  void **fog;
} GuiData;

void CObjThink(GOBJ *gobj);
void InputsThink(GOBJ *gobj);
GOBJ *InitSelector(float x, float y, int stageIdx);
