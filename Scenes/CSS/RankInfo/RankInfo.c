#ifndef SLIPPI_CSS_RANK_INFO_C
#define SLIPPI_CSS_RANK_INFO_C

#include "RankInfo.h"
#include "../../../Common.h"
#include "../../../Slippi.h"
#include "../../../ExiSlippi.h"
#include "../../../Files.h"
#include "../../../Game/SysText.c"
#include "../../../Game/Sounds.h"

void InitRankInfo() {
    // framesLeft = NARRATOR_LEN + RATING_CHANGE_LEN + FADE_OUT_LEN;
    framesLeft = NARRATOR_LEN + RANK_CHANGE_LEN;

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
    rankIconJobj = JOBJ_LoadJoint(slpCss->rankIcons->jobj);

    rankIconJobj->trans.X = -7.3f;
    rankIconJobj->trans.Y = -11.0f;

    JOBJ_AddSetAnim(rankIconJobj, slpCss->rankIcons, 0);
    // Set rank icon
    JOBJ_ReqAnimAll(rankIconJobj, (u32) rank);
    JOBJ_AnimAll(rankIconJobj);

    GObj_AddObject(gobj, 0x4, rankIconJobj);
    GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
}

void InitRankInfoText(RankInfo *rankInfo) {
    // Colors
    GXColor white = (GXColor){255, 255, 255, 255};

    // Create text object for rank info
    text = Text_CreateText(0, 0);
    text->kerning = 1;
    text->align = 0;
    text->use_aspect = 1;
    text->scale = (Vec2){0.01, 0.01};
    text->aspect.X *= 2.5;

    // Check if user has completed their placement matches
    if (rankInfo->ratingUpdateCount >= 5) {
        // Convert rating ordinal float to string
        char* ratingString[6];
        sprintf(ratingString, "%0.1f", rankInfo->ratingOrdinal);

        // Create Subtext objects
        rankSubtextId = Text_AddSubtext(text, -1100, 1540, RANK_STRINGS[rankInfo->rank]);
        Text_SetScale(text, rankSubtextId, 5, 5);
        ratingSubtextId = Text_AddSubtext(text, -1100, 1740, ratingString);
        Text_SetScale(text, ratingSubtextId, 4.5, 4.5);

        // Set color
        Text_SetColor(text, rankSubtextId, &white);
        Text_SetColor(text, ratingSubtextId, &white);
    }
    else {
        // Get string to show remaining placement matches
        char* placementString[15];
        sprintf(placementString, "%d SETS REQUIRED", 5 - rankInfo->ratingUpdateCount);

        // Create Subtext objects
        rankSubtextId = Text_AddSubtext(text, -1100, 1540, RANK_STRINGS[rankInfo->rank]);
        Text_SetScale(text, rankSubtextId, 5, 5);
        placementSubtextId = Text_AddSubtext(text, -1100, 1740, placementString);
        Text_SetScale(text, placementSubtextId, 4, 4);

        // Set color
        Text_SetColor(text, rankSubtextId, &white);
        Text_SetColor(text, placementSubtextId, &white);
    }
}

void UpdateRankInfo() {
    if (rankInfoResp->ratingChange != 0 | rankInfoResp->rankChange != 0) {
        UpdateRatingChange();
    }
}

float incrementRating(float ratingOrdinal, float ratingChange, int framesLeft) {
    // Rating to increment toward
    float newRating = ratingChange + ratingOrdinal;
    // Rating increment
    float delta = ((ratingChange * framesLeft) / -NARRATOR_LEN);
    return newRating + delta;
}

void UpdateRatingChange() {
    // Colors
    GXColor green = (GXColor) {3, 252, 28, 255};
    GXColor red = (GXColor) {255, 0, 0, 255};
    GXColor none = (GXColor) {0, 0, 0, 255};

    float displayRating = incrementRating(
            rankInfoResp->ratingOrdinal,
            rankInfoResp->ratingChange,
            framesLeft
        );

    char* ratingString[6];
    sprintf(ratingString, "%0.1f", displayRating);
    Text_SetText(text, ratingSubtextId, ratingString);

    if (framesLeft % 2 == 1 && framesLeft > RANK_CHANGE_LEN) {
        SFX_PlayRaw(
            (rankInfoResp->ratingChange > 0) ? TICK_UP : 
            (rankInfoResp->ratingChange < 0) ? TICK_DOWN : 0xD0, 
            (rankInfoResp->ratingChange > 0) ? 150: 
            (rankInfoResp->ratingChange < 0) ? 125 : 0, 
            64, 0, 0
            );
    }

    if (framesLeft == (NARRATOR_LEN + RANK_CHANGE_LEN) - NARRATOR_LEN) {
        if (rankInfoResp->rankChange > 0) {
            SFX_PlayRaw(RANK_UP_SMALL, 255, 64, 0, 0);
        }
        else if (rankInfoResp->rankChange < 0) {
            SFX_PlayRaw(RANK_DOWN_SMALL, 255, 64, 0, 0);
        }
    }

    if (framesLeft < RANK_CHANGE_LEN && framesLeft > 0) {
        if (rankInfoResp->rankChange != 0) { 
            // Rank icon bob animation
            if (framesLeft > (int) (RANK_CHANGE_LEN / 2)) {
                rankIconJobj->trans.Y = (float) (1 / (float) (RANK_CHANGE_LEN) * (float)(framesLeft - RANK_CHANGE_LEN / 2)) - 11.5f;
                JOBJ_SetMtxDirtySub(rankIconJobj);
            }
            else {
                rankIconJobj->trans.Y = (float) (1 / (float) (RANK_CHANGE_LEN) * (float)(-framesLeft - RANK_CHANGE_LEN / 2)) - 10.5f;
                JOBJ_SetMtxDirtySub(rankIconJobj);
            }
        }

        if (framesLeft == (int) (RANK_CHANGE_LEN / 2) + 1) {
            if (rankInfoResp->ratingChange != 0) {
                // Create rating change text
                char* changeString[7];
                sprintf(changeString, "%0.1f", rankInfoResp->ratingChange);

                // Create new subtext for rating change
                ratingChangeSubtextId = Text_AddSubtext(text, -650, 1750, changeString);
                Text_SetScale(text, ratingChangeSubtextId, 4, 4);
                if (rankInfoResp->ratingChange > 0) {
                    Text_SetColor(text, ratingChangeSubtextId, &green);
                }
                if (rankInfoResp->ratingChange < 0) {
                    Text_SetColor(text, ratingChangeSubtextId, &red);
                }
            }
            if (rankInfoResp->rankChange != 0) {
                // Update rank icon
                JOBJ_ReqAnimAll(rankIconJobj, (u32) ((int) rankInfoResp->rank + rankInfoResp->rankChange));
                JOBJ_AnimAll(rankIconJobj);
                // Update rank text
                Text_SetText(text, rankSubtextId, RANK_STRINGS[(int) rankInfoResp->rank + rankInfoResp->rankChange]);
            }
        }
    }

    if (framesLeft > 0) {
        framesLeft--;
    }       
}

#endif SLIPPI_CSS_RANK_INFO_C