#include "./m-ex/MexTK/mex.h"

// How to style code: https://stackoverflow.com/a/50489812/1249024

typedef struct GmTou1Data {
  JOBJSet **jobjs;
  COBJDesc **cobjs;
  void **lights;
  void **unk1;
} GmTou1Data;

void CObjThink(GOBJ *gobj);
