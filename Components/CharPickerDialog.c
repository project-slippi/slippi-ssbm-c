#include "CharPickerDialog.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"
#include "CharStageIcon.h"

CharPickerDialog *CharPickerDialog_Init(GUI_GameSetup *gui) {
  CharPickerDialog *cpd = calloc(sizeof(CharPickerDialog));

  // Init char picker dialog jobj
  cpd->jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_CharDialog];
  cpd->gobj = JOBJ_LoadSet(0, cpd->jobj_set, 0, 0, 3, 1, 0, GObj_Anim);
  cpd->root_jobj = cpd->gobj->hsd_object;

  // Connect CharStageIcons for each character to the dialog
  JOBJ *cur_joint = cpd->root_jobj->child->child->sibling->child;
  for (int i = CKIND_FALCON; i <= CKIND_GANONDORF; i++) {
    cpd->char_icons[i] = CSIcon_Init(gui);
    CSIcon_SetMaterial(cpd->char_icons[i], CSIcon_ConvertCharToMat(i));

    // Attach icon to cur joint and move to next joint
    JOBJ_AddChild(cur_joint, cpd->char_icons[i]->root_jobj);
    cur_joint = cur_joint->sibling;
  }
}

void CharPickerDialog_Free(CharPickerDialog *cpd) {
  // Not implemented because we only initialize a very limited amount of these
}

void CharPickerDialog_SetPos(CharPickerDialog *cpd, Vec3 pos) {
  // OSReport("Pos: %f, %f, %f\n", pos.X, pos.Y, pos.Z);
  // OSReport("PosStart: %f, %f, %f\n", cpd->root_jobj->child->trans.X, cpd->root_jobj->child->trans.Y, cpd->root_jobj->child->trans.Z);
  cpd->root_jobj->trans = pos;
  JOBJ_SetMtxDirtySub(cpd->root_jobj);

  // OSReport("PosEnd: %f, %f, %f\n", cpd->root_jobj->child->trans.X, cpd->root_jobj->child->trans.Y, cpd->root_jobj->child->trans.Z);
}

void CharPickerDialog_OpenDialog(CharPickerDialog *cpd) {
  // TODO: Implement
}