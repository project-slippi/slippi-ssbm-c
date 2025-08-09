#include "SheikSelector.h"

bool sheikSelected = true;
bool zeldaSelected = false;

u8 GetPlayerIndex() {
  const u8 PLAYER_INDEX_OFFSET = 0x40;
  u8 playerIndex = *((u8 *)(R13_PTR(CSS_DATA_OFFSET + PLAYER_INDEX_OFFSET)));
  return playerIndex;
}

CSSData *GetPlayerSelections(u8 playerIndex) {
  return (CSSData *)(R13_PTR(CSS_DATA_OFFSET + playerIndex * 0x24));
}

void SetSelectedChar(u8 ckind) {
  u8 playerIndex = GetPlayerIndex();
  CSSData *CssData = GetPlayerSelections(playerIndex);
  u8 *selectedChar = &CssData->data.data.players[playerIndex].c_kind;
  *selectedChar = ckind;

  CSSIcon *iconData = 0x803f0cc8;
  iconData->char_kind = ckind;

  SFX_getCharacterNameAnnouncer(ckind);
  CSS_CursorHighlightUpdateCSPInfo(0);
}

u8 GetSelectedChar() {
  u8 selectedChar = *((u8 *)0x8043208f);
  return selectedChar;
}

Vec2 GetCursorPos() {
  CSSPlayerData *cssPlayerData = *((unsigned int *)0x804A0BC0);
  return cssPlayerData->position;
}

void InitSheikSelector() {
  SlippiCSSDataTable *dt = GetSlpCSSDT();
  SlpCSSDesc *slpCss = dt->SlpCSSDatAddress;

  // Initialize local player rank icon
  GOBJ *gobj = GObj_Create(0x4, 0x5, 0x80);
  selectorJobj = JOBJ_LoadJoint(slpCss->sheikSelector);

  JOBJ *zeldaIcon = selectorJobj->child;
  JOBJ *sheikIcon = selectorJobj->child->sibling;

  JOBJ_SetAllAlpha(zeldaIcon, INACTIVE_ALPHA);
  // Set sheik to highlighted by default
  JOBJ_SetAllAlpha(sheikIcon, ACTIVE_ALPHA);

  selectorJobj->trans.X = -8.f;
  selectorJobj->trans.Y = -22.5f;

  JOBJ_SetMtxDirtySub(selectorJobj);

  GObj_AddObject(gobj, 0x4, selectorJobj);
  GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
}

void UpdateSheikSelector() {
  bool isLockedIn = SLIPPI_CSS_DATA_REF->dt->msrb->isLocalPlayerReady;
  if (isLockedIn) {
    // Exit if player has locked their character selection
    // TODO :: Does this hide the selector?
    return;
  }

  u8 port = GetPlayerIndex();
  u64 downInputs = Pad_GetDown(port);

  Vec2 cursorPos = GetCursorPos();
  u8 selectedChar = GetSelectedChar();

  if (selectedChar == CKIND_SHEIK || selectedChar == CKIND_ZELDA) {
    if (selectorJobj) {
      // Show selector UI
      JOBJ_SetAllAlpha(selectorJobj, 1.f);

      JOBJ *zeldaIcon = selectorJobj->child;
      JOBJ *sheikIcon = selectorJobj->child->sibling;

      bool sheikHovered = false;
      bool zeldaHovered = false;

      JOBJ *buttons[2] = {zeldaIcon, sheikIcon};
      for (int i = 0; i < 2; i++) {
        bool isSheik = i == 1;
        const float BUTTON_BOTTOM = BUTTON_TOP - BUTTON_HEIGHT;
        // Check if cursor is at the height of the selector
        if (cursorPos.Y < BUTTON_TOP && cursorPos.Y >= BUTTON_BOTTOM) {
          float buttonLeft = BUTTON_CONTAINER_START_X + i * BUTTON_WIDTH;
          float buttonRight = buttonLeft + BUTTON_WIDTH;
          // Check if cursor is within the bounds of this button
          if (cursorPos.X > buttonLeft && cursorPos.X < buttonRight) {
            sheikHovered = isSheik;
            zeldaHovered = !isSheik;

            if (downInputs & HSD_BUTTON_A) {
              // Change character
              if (isSheik) {
                SetSelectedChar(CKIND_SHEIK);
              } else {
                SetSelectedChar(CKIND_ZELDA);
              }
              sheikSelected = isSheik;
              zeldaSelected = !isSheik;
            }
          }
        }
      }

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
  } else {
    // Hide selector UI
    JOBJ_SetAllAlpha(selectorJobj, 0.f);
  }
}
