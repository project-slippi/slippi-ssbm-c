#ifndef SLIPPI_COMMON_H
#define SLIPPI_COMMON_H
#include <stdbool.h>
#include "../MexTK/mex.h";

#define NULL ((void *)0)
#define spawn(X) calloc(sizeof(X)) 

/** Functions **/
void* (*bp)(void) = (void *)0x8021b2d8;

bool GOBJ_IsAlive(GOBJ* gobj)
{
    return gobj != NULL;
}

/**
 * Creates a Text and allows setting custom gx link and priority
 * This is because the text->gobj link/pri doesn't really affect it when updated
 * @param SisIndex
 * @param canvasID
 * @param gx_link
 * @param gx_pri
 * @return
 */
Text* Text_CreateTextWithGX(int SisIndex, int canvasID, u8 gx_link, u8 gx_pri){
    // Update GX link and pri that this is going to be created with
    stc_textcanvas_first[0]->gx_link = gx_link;
    stc_textcanvas_first[0]->gx_pri = gx_pri;

    Text* text = Text_CreateText(SisIndex, canvasID);

    // restore gx link and pri
    stc_textcanvas_first[0]->gx_link = 1;
    stc_textcanvas_first[0]->gx_pri = 128;
    return text;
}

#endif SLIPPI_COMMON_H
