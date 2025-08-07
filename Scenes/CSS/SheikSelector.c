#include "SheikSelector.h"

typedef struct CSSPlayerData
{
    GOBJ *gobj;
    u8 port;
    u8 state;
    u8 held_puck;
    u8 unk;
    short unk1;
    short exit_timer;
    Vec2 position;
} CSSPlayerData;

int CSS_DATA_OFFSET = -0x49F0;

bool poopbool = false;
u8 GetPlayerIndex()
{
    /*
        # Get location from which we can find selected character
        lwz r4, -0x49F0(r13) # base address where css selections are stored
        lbz r3, -0x49B0(r13) # player index
        mulli r3, r3, 0x24
        add REG_PORT_SELECTIONS_ADDR, r4, r3
    */
    const u8 PLAYER_INDEX_OFFSET = 0x40;
    u8 playerIndex = *((u8 *)(R13_PTR(CSS_DATA_OFFSET + PLAYER_INDEX_OFFSET)));
    return playerIndex;
}

CSSData *GetPlayerSelections(u8 playerIndex)
{
    return (CSSData *)(R13_PTR(CSS_DATA_OFFSET + playerIndex * 0x24));
}

void SetSelectedChar(u8 ckind)
{
    u8 playerIndex = GetPlayerIndex();
    u8 *selectedChar = GetPlayerSelections(playerIndex)->data.data.players[playerIndex].c_kind;

    // Set character
    selectedChar = ckind;

    bp();
    CSSIcon *iconData = 0x803f0cc8;
    iconData->char_kind = ckind;

    SFX_getCharacterNameAnnouncer(ckind);

    u8 port = GetPlayerIndex();
    CSS_CursorHighlightUpdateCSPInfo(port);
}

u8 GetSelectedChar()
{
    u8 selectedChar = *((u8 *)0x8043208f);
    return selectedChar;
}

Vec2 GetCursorPos()
{
    CSSPlayerData *cssPlayerData = *((unsigned int *)0x804A0BC0);
    return cssPlayerData->position;
}

void InitSheikSelector()
{
    SlippiCSSDataTable *dt = GetSlpCSSDT();
    SlpCSSDesc *slpCss = dt->SlpCSSDatAddress;

    // Initialize local player rank icon
    GOBJ *gobj = GObj_Create(0x4, 0x5, 0x80);
    selectorJobj = JOBJ_LoadJoint(slpCss->sheikSelector);

    JOBJ *zeldaIcon = selectorJobj->child;
    JOBJ *sheikIcon = selectorJobj->child->sibling;

    JOBJ_SetAllAlpha(zeldaIcon, 0.5f);
    // Set sheik to highlighted by default
    JOBJ_SetAllAlpha(sheikIcon, 1.f);

    selectorJobj->trans.X = -8.f;
    selectorJobj->trans.Y = -22.5f;

    JOBJ_SetMtxDirtySub(selectorJobj);

    GObj_AddObject(gobj, 0x4, selectorJobj);
    GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
}

void UpdateSheikSelector()
{
    bool isLockedIn = SLIPPI_CSS_DATA_REF->dt->msrb->isLocalPlayerReady;
    if (isLockedIn)
    {
        // Exit if player has locked their character selection
        // TODO :: Does this hide the selector?
        return;
    }

    Vec2 cursorPos = GetCursorPos();
    u8 selectedChar = GetSelectedChar(); // Get selected character

    bool sheikSelected = selectedChar == CKIND_SHEIK;
    bool zeldaSelected = selectedChar == CKIND_ZELDA;
    if (sheikSelected || zeldaSelected)
    {
        if (selectorJobj)
        {
            JOBJ *zeldaIcon = selectorJobj->child;
            JOBJ *sheikIcon = selectorJobj->child->sibling;

            // First set alpha values based on who is currently selected
            float zeldaAlpha = zeldaSelected ? 1.f : 0.5f;
            JOBJ_SetAllAlpha(zeldaIcon, zeldaAlpha);
            float sheikAlpha = sheikSelected ? 1.f : 0.5f;
            JOBJ_SetAllAlpha(sheikIcon, sheikAlpha);
        }

        // TODO :: Highlight hovered icon
    }
}
