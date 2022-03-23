#include "Characters.h"

u8 IsMatchingSelection(u8 charId1, u8 charColor1, u8 charId2, u8 charColor2) {
  // Convert sheiks to zelda for comparison
  if (charId1 == CKIND_SHEIK) {
    charId1 = CKIND_ZELDA;
  }
  if (charId2 == CKIND_SHEIK) {
    charId2 = CKIND_ZELDA;
  }

  return charId1 == charId2 && charColor1 == charColor2;
}