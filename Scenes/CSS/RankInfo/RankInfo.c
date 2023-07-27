#ifndef SLIPPI_CSS_RANK_INFO_C
#define SLIPPI_CSS_RANK_INFO_C

#include "RankInfo.h"
#include "../../../Common.h"
#include "../../../Slippi.h"
#include "../../../ExiSlippi.h"
#include "../../../Files.h"

ExiSlippi_GetRank_Response *rankInfoResp = NULL;

void InitRankInfo() {
    rankInfoResp = calloc(sizeof(ExiSlippi_GetRank_Response));

    // Send dolphin command to pull rank data
    ExiSlippi_GetRank_Query *q = calloc(sizeof(ExiSlippi_GetRank_Query));
    q->command = ExiSlippi_Command_GET_RANK;
    ExiSlippi_Transfer(q, sizeof(ExiSlippi_GetRank_Query), ExiSlippi_TransferMode_WRITE);
    ExiSlippi_Transfer(rankInfoResp, sizeof(ExiSlippi_GetRank_Response), ExiSlippi_TransferMode_READ);

    // Receieve rank data from dolphin
    InitRankInfoText(rankInfoResp);

    SlippiCSSDataTable *dt = GetSlpCSSDT();
    u8 rank = rankInfoResp->rank;

    // Select the correct icon for player
    InitRankIcon(dt->SlpCSSDatAddress, rank);
}

void InitRankIcon(SlpCSSDesc *slpCss, u8 rank) {
    GOBJ *gobj = GObj_Create(0x4, 0x5, 0x80);
    JOBJ *jobj = JOBJ_LoadJoint(slpCss->rankIcons->jobj);

    jobj->trans.X = -7.3f;
    jobj->trans.Y = -11.0f;

    JOBJ_AddSetAnim(jobj, slpCss->rankIcons, 0);
    // Set rank icon
    JOBJ_ReqAnimAll(jobj, (u32) rank);
    JOBJ_AnimAll(jobj);

    GObj_AddObject(gobj, 0x4, jobj);
    GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
}

void InitRankInfoText(RankInfo *rankInfo) {
    // Check if user has completed their placement matches
    if (rankInfo->ratingUpdateCount >= 5) {
        // Convert rating ordinal float to string
        char* ratingString[20];
        sprintf(ratingString, "%0.1f", rankInfo->ratingOrdinal);

        // Create rating ordinal text
        Text *text = Text_CreateText(0, 0);
        text->kerning = 1;
        text->align = 0;
        text->use_aspect = 1;
        text->scale = (Vec2){0.01, 0.01};
        text->aspect.X *= 2.5;
        int rankSubtextId = Text_AddSubtext(text, -1100, 1540, RANK_STRINGS[rankInfo->rank]);
        Text_SetScale(text, rankSubtextId, 5, 5);
        int ratingSubtextId = Text_AddSubtext(text, -1100, 1740, ratingString);
        Text_SetScale(text, ratingSubtextId, 4.5, 4.5);
        GXColor col = (GXColor){255, 255, 255, 155};
        Text_SetColor(text, rankSubtextId, &col);
    }
    else {
        // Get string to show remaining placement matches
        char* placementString[15];
        sprintf(placementString, "%d SETS REQUIRED", 5 - rankInfo->ratingUpdateCount);

        // Create rating ordinal text
        Text *text = Text_CreateText(0, 0);
        text->kerning = 1;
        text->align = 0;
        text->use_aspect = 1;
        text->scale = (Vec2){0.01, 0.01};
        text->aspect.X *= 2.5;
        int rankSubtextId = Text_AddSubtext(text, -1100, 1540, RANK_STRINGS[rankInfo->rank]);
        Text_SetScale(text, rankSubtextId, 5, 5);
        int ratingSubtextId = Text_AddSubtext(text, -1100, 1740, placementString);
        Text_SetScale(text, ratingSubtextId, 4, 4);
        GXColor col = (GXColor){255, 255, 255, 155};
        Text_SetColor(text, rankSubtextId, &col);
    }
}

#endif SLIPPI_CSS_RANK_INFO_C