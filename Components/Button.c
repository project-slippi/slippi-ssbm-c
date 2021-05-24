#include "Button.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

Button *Button_Init(GUI_GameSetup *gui, GUI_GameSetup_JOBJ type) {
  Button *btn = calloc(sizeof(Button));

  // Init btn jobj
  btn->jobj_set = gui->jobjs[type];
  btn->gobj = JOBJ_LoadSet(0, btn->jobj_set, 0, 0, 3, 1, 1, GObj_Anim);
  btn->root_jobj = btn->gobj->hsd_object;

  // Init state
  Button_SetVisibility(btn, true);
  Button_SetHover(btn, false);

  return btn;
}

void Button_Free(Button *btn) {
  // Not implemented because we only initialize a very limited amount of these
}

void test_aobj(AOBJ *aobj, int x, int y) {
  char prnt[50];
  sprintf(prnt, "Flags: %X\n", aobj->flags);
  OSReport(prnt);
}

void Button_SetMaterial(Button *btn, Button_Material matIdx) {
  // char prnt[50];
  // sprintf(prnt, "Mat: %f\n", JOBJ_GetCurrentMatAnimFrame(btn->root_jobj->child));
  // OSReport(prnt);

  // JOBJ_RunAObjCallback(btn->root_jobj->child, 6, 0x400, test_aobj, 6, 0, 0);
  // JOBJ_ReqAnimAll(btn->root_jobj->child, matIdx);
  // JOBJ_AnimAll(btn->root_jobj->child);

  // JOBJ_ReqAnimAll(icon->root_jobj, matIdx);
  // JOBJ_AnimAll(icon->root_jobj);
}

void Button_SetHover(Button *btn, u8 is_hover) {
  if (!btn->state.is_visible) {
    // TODO: This is a hack to deal with the bug where hitting the redo button or pressing b while
    // TODO: hovering the redo button would cause the btn hover outline to continue animating
    is_hover = false;
  }

  JOBJ *jobj = btn->root_jobj;

  if (is_hover) {
    // Reset animation to start in the same place
    JOBJ_AddSetAnim(jobj, btn->jobj_set, 0);
    JOBJ_ReqAnimAll(btn->root_jobj, 10);
    // JOBJ_Anim
  } else {
    JOBJ_ReqAnimAll(btn->root_jobj, 0);
    JOBJ_AnimAll(btn->root_jobj);
    JOBJ_RemoveAnimAll(jobj);
  }

  btn->state.is_hover = is_hover;
}

void Button_SetVisibility(Button *btn, u8 is_visible) {
  JOBJ *jobj = btn->root_jobj;

  if (is_visible) {
    jobj->child->flags = jobj->child->flags & ~JOBJ_HIDDEN;  // Clear hidden flag
  } else {
    // TODO: Figure out why setting child flag hidden works but it doesn't work on root
    jobj->child->flags = jobj->child->flags | JOBJ_HIDDEN;  // Set hidden flag
  }

  btn->state.is_visible = is_visible;
}

void Button_SetPos(Button *btn, Vec3 p) {
  btn->root_jobj->trans = p;
}