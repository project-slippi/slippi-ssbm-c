#include "Button.h"

#include "../Files.h"
#include "../m-ex/MexTK/mex.h"

static void _SetVisibility(Button *btn, u8 is_visible) {
  JOBJ *jobj = btn->root_jobj;

  if (is_visible) {
    jobj->child->flags = jobj->child->flags & ~JOBJ_HIDDEN;  // Clear hidden flag
  } else {
    // TODO: Figure out why setting child flag hidden works but it doesn't work on root
    jobj->child->flags = jobj->child->flags | JOBJ_HIDDEN;  // Set hidden flag
  }

  btn->state.is_visible = is_visible;
}

static void _SetHover(Button *btn, u8 is_hover) {
  if (!btn->state.is_visible) {
    // TODO: This is a hack to deal with the bug where hitting the redo button or pressing b while
    // TODO: hovering the redo button would cause the btn hover outline to continue animating
    is_hover = false;
  }

  if (is_hover) {
    // Reset animation to start in the same place
    JOBJ_AddSetAnim(btn->root_jobj, btn->jobj_set, 0);
    JOBJ_ReqAnimAll(btn->root_jobj, 10);
    // JOBJ_Anim
  } else {
    JOBJ_ReqAnimAll(btn->root_jobj, 0);
    JOBJ_AnimAll(btn->root_jobj);
    JOBJ_RemoveAnimAll(btn->root_jobj);
  }

  btn->state.is_hover = is_hover;
}

Button *Button_Init(GUI_GameSetup *gui) {
  Button *btn = calloc(sizeof(Button));

  // Init btn jobj
  btn->jobj_set = gui->jobjs[GUI_GameSetup_JOBJ_Button];
  btn->gobj = JOBJ_LoadSet(0, btn->jobj_set, 0, 0, 3, 1, 1, GObj_Anim);
  btn->root_jobj = btn->gobj->hsd_object;

  // Init state
  _SetVisibility(btn, true);
  _SetHover(btn, false);

  return btn;
}

void Button_Free(Button *btn) {
  // Not implemented because we only initialize a very limited amount of these
}

void Button_SetMaterial(Button *btn, Button_Material matIdx) {
  // Add set anim id 1 which allows us to change texture and animate
  JOBJ_AddSetAnim(btn->root_jobj, btn->jobj_set, 1);
  JOBJ_ReqAnimAll(btn->root_jobj->child, matIdx);
  JOBJ_AnimAll(btn->root_jobj->child);

  // Remove set anim id 1 and re-add anim id 0 which supports blinking
  JOBJ_RemoveAnimAll(btn->root_jobj);
  JOBJ_AddSetAnim(btn->root_jobj, btn->jobj_set, 0);
}

void Button_SetHover(Button *btn, u8 is_hover) {
  if (btn->state.is_hover == is_hover) {
    return;
  }

  _SetHover(btn, is_hover);
}

void Button_SetVisibility(Button *btn, u8 is_visible) {
  if (btn->state.is_visible == is_visible) {
    return;
  }

  _SetVisibility(btn, is_visible);
}

void Button_SetPos(Button *btn, Vec3 p) {
  btn->root_jobj->trans = p;
  JOBJ_SetMtxDirtySub(btn->root_jobj);
}