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

void UpdateSheikSelector()
{
    bool isLockedIn = SLIPPI_CSS_DATA_REF->dt->msrb->isLocalPlayerReady;
    if (isLockedIn)
    {
        // Exit if player has locked their character selection
        return;
    }

    Vec2 cursorPos = GetCursorPos();
    OSReport("cursor pos (%f, %f)\n", cursorPos.X, cursorPos.Y);

    u8 selectedChar = GetSelectedChar(); // Get selected character
    // OSReport("char %d\n", selectedChar);
    bool sheikSelected = selectedChar == CKIND_SHEIK;
    bool zeldaSelected = selectedChar == CKIND_ZELDA;
    if (sheikSelected || zeldaSelected)
    {
        OSReport("ZELDA or SHEIK %d\n", selectedChar);
    }
}
