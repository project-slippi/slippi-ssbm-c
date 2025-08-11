#include "SheikSelector.h"

u8 GetPlayerIndex() {
  return R13_U8(PLAYER_IDX_R13_OFFSET);
}

CSSData *GetCSSData() {
  return R13_PTR(CSS_DATA_R13_OFFSET);
}

void SetSelectedChar(u8 ckind) {
  u8 playerIndex = GetPlayerIndex();
  CSSData *CssData = GetCSSData();
  CssData->data.data.players[playerIndex].c_kind = ckind;

  // Change nametag to say Sheik or Zelda
  CSSIcon *iconData = 0x803f0cc8;
  iconData->char_kind = ckind;

  SFX_getCharacterNameAnnouncer(ckind);
  CSS_CursorHighlightUpdateCSPInfo(0);
}

u8 GetSelectedChar() {
  u8 selectedChar = ACCESS_U8(0x8043208f);
  return selectedChar;
}

Vec2 GetCursorPos() {
  CSSPlayerData *cssPlayerData = ACCESS_PTR(0x804A0BC0);
  return cssPlayerData->position;
}

void InitSheikSelector() {
  SlippiCSSDataTable *dt = GetSlpCSSDT();
  SlpCSSDesc *slpCss = dt->SlpCSSDatAddress;

  // Initialize local player rank icon
  selectorGobj = GObj_Create(0x4, 0x5, 0x80);
  selectorJobj = JOBJ_LoadJoint(slpCss->sheikSelector);

  JOBJ *zeldaIcon = selectorJobj->child;
  JOBJ *sheikIcon = selectorJobj->child->sibling;

  selectorJobj->trans.X = -8.f;
  selectorJobj->trans.Y = -22.5f;

  JOBJ_SetMtxDirtySub(selectorJobj);

  GObj_AddObject(selectorGobj, 0x4, selectorJobj);
  GObj_AddGXLink(selectorGobj, GXLink_Common, 1, 129);

  isInNameEntry = IsOnCSSNameEntryScreen();
}

void UpdateSheikSelector() {
  bool wasInNameEntry = isInNameEntry;
  isInNameEntry = IsOnCSSNameEntryScreen();

  if (isInNameEntry) {
    return;
  }

  if (wasInNameEntry) {
    // When we come back from name entry, we need to re-init our gobj.
    // All Gobj's get destroyed by fn 8022f138 when entering name entry so we shouldn't be
    // leaking memory by re-initializing here
    InitSheikSelector();
  }

  bool isLockedIn = SLIPPI_CSS_DATA_REF->dt->msrb->isLocalPlayerReady;
  if (isLockedIn) {
    // Show selector but dont let it be interactable while we are locked in.
    // We still need to update the alphas cause if we just game from name entry
    // both would show up as selected if we didn't run this logic. By passing false
    // twice though we avoid any potential hover states from showing.
    UpdateSelectorAlphas(false, false);
    return;
  }

  u8 port = GetPlayerIndex();
  u64 downInputs = Pad_GetDown(port);

  Vec2 cursorPos = GetCursorPos();
  u8 selectedChar = GetSelectedChar();

  // If sheik / zelda not selected, hide selector UI
  if (selectedChar != CKIND_SHEIK && selectedChar != CKIND_ZELDA) {
    // Hide selector UI
    JOBJ_SetAllAlpha(selectorJobj, 0.f);
    return;
  }

  // Show selector UI
  JOBJ_SetAllAlpha(selectorJobj, 1.f);

  bool sheikHovered = false;
  bool zeldaHovered = false;

  for (int i = 0; i < 2; i++) {
    bool isSheik = i == 1;
    const float BUTTON_BOTTOM = BUTTON_TOP - BUTTON_HEIGHT;

    // Check if cursor is at the height of the selector
    if (cursorPos.Y > BUTTON_TOP || cursorPos.Y < BUTTON_BOTTOM) {
      continue;
    }

    // Check if cursor is within the bounds of this button
    float buttonLeft = BUTTON_CONTAINER_START_X + i * BUTTON_WIDTH;
    float buttonRight = buttonLeft + BUTTON_WIDTH;
    if (cursorPos.X < buttonLeft || cursorPos.X > buttonRight) {
      continue;
    }

    // Manage hovered flags
    sheikHovered = isSheik;
    zeldaHovered = !isSheik;

    // Now check for button press to see if we should change characters
    bool aButtonPressed = downInputs & HSD_BUTTON_A;
    if (!aButtonPressed) {
      continue;
    }

    // A was pressed, change character
    SetSelectedChar(isSheik ? CKIND_SHEIK : CKIND_ZELDA);
  }

  UpdateSelectorAlphas(zeldaHovered, sheikHovered);
}

void UpdateSelectorAlphas(bool zeldaHovered, bool sheikHovered) {
  JOBJ *zeldaIcon = selectorJobj->child;
  JOBJ *sheikIcon = selectorJobj->child->sibling;

  u8 selectedChar = GetSelectedChar();

  bool sheikSelected = selectedChar == CKIND_SHEIK;
  bool zeldaSelected = selectedChar == CKIND_ZELDA;

  float zeldaAlpha = INACTIVE_ALPHA;
  float sheikAlpha = INACTIVE_ALPHA;

  if (zeldaHovered) {
    zeldaAlpha = HOVER_ALPHA;
  }
  if (sheikHovered) {
    sheikAlpha = HOVER_ALPHA;
  }

  if (zeldaSelected) {
    zeldaAlpha = ACTIVE_ALPHA;
  }
  if (sheikSelected) {
    sheikAlpha = ACTIVE_ALPHA;
  }

  JOBJ_SetAllAlpha(zeldaIcon, zeldaAlpha);
  JOBJ_SetAllAlpha(sheikIcon, sheikAlpha);
}
