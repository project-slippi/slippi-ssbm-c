#ifndef SLIPPI_COMMON_H
#define SLIPPI_COMMON_H
#include <stdbool.h>
#include "m-ex/MexTK/mex.h"

//#define LOCAL_TESTING

#define NULL ((void *)0)
#define spawn(X) calloc(sizeof(X)) 

/** Functions **/
void* (*bp)(void) = (void *)0x8021b2d8;

int SLP_GetFreeBytes(){
    return OSCheckHeap(HSD_GetHeapID());
}
void SLP_PrintFreeMemoryT(char* tag) {
    OSReport("SLP.PrintFreeMemory: %s %.2fkb\n", tag, (float)SLP_GetFreeBytes() / 1024);
}
void SLP_PrintFreeMemory() {
    SLP_PrintFreeMemoryT("");
}
/*
 * Initial value of current heap free bytes;
 * see SLP_StartMemTrace
 */
int _slp_mem_trace_start = 0; // TODO: make this a stack
void SLP_StartMemTrace() {
    OSReport("SLP.StartMemTrace Current free memory: %.2fkb\n", (float)SLP_GetFreeBytes() / 1024);
    _slp_mem_trace_start = SLP_GetFreeBytes();
}

void SLP_StopMemTrace(){
    int currentFreeBytes = SLP_GetFreeBytes();
    OSReport("SLP.StopMemTrace Current free memory: %.2fkb\n", (float)currentFreeBytes / 1024);
    OSReport("Memory Diff: %.2fkb\n", (float)(currentFreeBytes-_slp_mem_trace_start)/1024);
    _slp_mem_trace_start = 0;
}
/** HSD Functions **/

bool GOBJ_IsAlive(GOBJ* gobj)
{
    return gobj != NULL;
}

void JOBJ_SetAllAlpha(JOBJ *jobj, float alpha) {

    if (!jobj) return;

    DOBJ *dobj = jobj->dobj;
    while (dobj) {
        if (dobj->mobj && dobj->mobj->mat)
            dobj->mobj->mat->alpha = alpha;
        dobj = dobj->next;
    }

    JOBJ_SetAllAlpha(jobj->child, alpha);
    JOBJ_SetAllAlpha(jobj->sibling, alpha);
}

void JOBJ_SetMatFrame(JOBJ* jobj, float frame) {
JOBJ_ForEachAnim(jobj, 0x6, 0x400, AOBJ_ReqAnim, 1, frame);
JOBJ_AnimAll(jobj);
JOBJ_ForEachAnim(jobj, 0x6, 0x400, AOBJ_StopAnim, 6, 0, 0);
}
// TODO: set in melee.link
void *(*MOBJ_SetFlags)(MOBJ *mobj, u32 flags) = 0x80362D38;
// TODO: set in melee.link
void *(*MOBJ_ClearFlags)(MOBJ *mobj, u32 flags) = 0x80362D50;

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

/**
 * Updates a Text color attributes
 * @param text  Text* pointer
 * @param colorOffset Offset of text->text_start where encoded <Color> starts
 * @param color 0x00RRGGBB
 * @return
 */
void Text_UpdateColor(Text* text, u8 colorOffset, u8 r, u8 g, u8 b){
    text->color.r = r;
    text->color.g = g;
    text->color.b = b;

    *(text->text_start+colorOffset+0x1) = text->color.r;
    *(text->text_start+colorOffset+0x2) = text->color.g;
    *(text->text_start+colorOffset+0x3) = text->color.b;
}

void Text_UpdateColor2(Text* text, u8 colorOffset, u32 color){
    text->color.r = color >> 10; // ff
    text->color.g = (color >> 8) ^ (text->color.r << 8) ; // ffff ^ ff00 = ff
    text->color.b = (((text->color.r << 8)+text->color.g) << 8) ^ color;
    // (ff00 + ff) ^ ffff00

    // OSReport("Text_UpdateColor2 r=0x%x g=0x%x b=0x%x\n", text->color.r, text->color.b, text->color.g);

    *(text->text_start+colorOffset+0x1) = text->color.r;
    *(text->text_start+colorOffset+0x2) = text->color.g;
    *(text->text_start+colorOffset+0x3) = text->color.b;
}


#endif SLIPPI_COMMON_H
