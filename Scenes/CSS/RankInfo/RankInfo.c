#ifndef SLIPPI_CSS_RANK_INFO_C
#define SLIPPI_CSS_RANK_INFO_C

#include "RankInfo.h"
#include "../../../Common.h"
#include "../../../Slippi.h"
#include "../../../ExiSlippi.h"
#include "../../../Files.h"
#include "../../../Game/SysText.c"
#include "../../../Game/Sounds.h"

JOBJ* rankIconJobj;
int currentFrame = 0;
int ratingChangeLen; // Rating change sequence length

void InitRankInfo() {
	rankInfoResp = calloc(sizeof(ExiSlippi_GetRank_Response));

	// Send dolphin command to pull rank data
	ExiSlippi_GetRank_Query *q = calloc(sizeof(ExiSlippi_GetRank_Query));
	q->command = ExiSlippi_Command_GET_RANK;
	ExiSlippi_Transfer(q, sizeof(ExiSlippi_GetRank_Query), ExiSlippi_TransferMode_WRITE);
	ExiSlippi_Transfer(rankInfoResp, sizeof(ExiSlippi_GetRank_Response), ExiSlippi_TransferMode_READ);

	OSReport("rank: %d\n", rankInfoResp->rank);
	OSReport("ratingOrdinal: %f\n", rankInfoResp->ratingOrdinal);
	OSReport("global: %d\n", rankInfoResp->global);
	OSReport("regional: %d\n", rankInfoResp->regional);
	OSReport("ratingUpdateCount: %d\n", rankInfoResp->ratingUpdateCount);
	OSReport("ratingChange: %f\n", rankInfoResp->ratingChange);
	OSReport("rankChange: %d\n", rankInfoResp->rankChange);

	// u8 rank = RANK_SILVER_3;
	// rankInfoResp->rank = rank;
	// rankInfoResp->ratingOrdinal = 1423.7f;
	// rankInfoResp->global = 0;
	// rankInfoResp->regional = 0;
	// rankInfoResp->ratingUpdateCount = 11;
	// rankInfoResp->ratingChange = 24.3f;
	// rankInfoResp->rankChange = 1;

    // Determine the duration of the rating increase / decrease
	float change = rankInfoResp->ratingChange;
	if (abs(change) < LOW_RATING_THRESHOLD) {
		ratingChangeLen = RATING_CHANGE_LEN * 0.25f;
	}
	else if (abs(change) < MED_RATING_THRESHOLD) {
		ratingChangeLen = RATING_CHANGE_LEN * 0.5f;
	}
	else {
		ratingChangeLen = RATING_CHANGE_LEN;
	}

    SlippiCSSDataTable *dt = GetSlpCSSDT();
	InitRankIcon(dt->SlpCSSDatAddress, rankInfoResp->rank);
    InitRankInfoText(rankInfoResp);
}

void SetRankIcon(u8 rank) {
    if (rankIconJobj) {
        // Set rank icon
        JOBJ_ForEachAnim(rankIconJobj, 6, 0x400, AOBJ_ReqAnim, 1, (float) rank); // HSD_TypeMask::TOBJ 0x400
        JOBJ_AnimAll(rankIconJobj);
        JOBJ_ForEachAnim(rankIconJobj, 6, 0x400, AOBJ_StopAnim, 6, 0, 0);
    }
}

void InitRankIcon(SlpCSSDesc *slpCss, u8 rank) {
    GOBJ *gobj = GObj_Create(0x4, 0x5, 0x80);
    rankIconJobj = JOBJ_LoadJoint(slpCss->rankIcons->jobj);

    rankIconJobj->trans.X = -7.3f;
    rankIconJobj->trans.Y = -11.0f;
    JOBJ_SetMtxDirtySub(rankIconJobj);

    // Get all animations
    JOBJ_AddSetAnim(rankIconJobj, slpCss->rankIcons, 0);
    JOBJ_ReqAnimAll(rankIconJobj, 0.0);

    // Check if the rank response is valid
    bool requestFailed = rank >= RANK_COUNT;
    if ( requestFailed ) {
        SetRankIcon(RANK_UNRANKED);
    }
    else {
        SetRankIcon(rank);
    }

    // Initialize bob animation
    JOBJ_ForEachAnim(rankIconJobj, 6, 0x20, AOBJ_ReqAnim, 1, 0.f); // HSD_TypeMask::JOBJ 0x20
    JOBJ_AnimAll(rankIconJobj);
    JOBJ_ForEachAnim(rankIconJobj, 6, 0x20, AOBJ_StopAnim, 6, 0, 0);

    GObj_AddObject(gobj, 0x4, rankIconJobj);
    GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
}

void InitRankInfoText(RankInfo *rankInfo) {
    // Create text object for rank info
    text = Text_CreateText(0, 0);
    text->kerning = 1;
    text->align = 0;
    text->use_aspect = 1;
    text->scale = (Vec2){0.01, 0.01};
    text->aspect.X *= 2.5;

    // Create string to show rating / remaining placement matches
    char* ratingString[15];

    // Check if the rank in the response is valid
    bool requestFailed = rankInfo->rank >= RANK_COUNT;

    // Create Subtext objects
    rankSubtextId = Text_AddSubtext(text, -1100, 1540, requestFailed ? "Error" : RANK_STRINGS[rankInfo->rank]);
    Text_SetScale(text, rankSubtextId, 5, 5);

    // Check if the user has completed their placement matches
    bool isPlaced = rankInfo->ratingUpdateCount >= PLACEMENT_THRESHOLD;
    if (requestFailed) {
        // Show rank fetch has failed
        sprintf(ratingString, "Failed to get rank");
    }
    else if (isPlaced) {
        // Show rating if placed
        sprintf(ratingString, "%0.1f", rankInfo->ratingOrdinal);
    }
    else {
        // Show remaining number of sets if not placed
        sprintf(ratingString, "%d SETS REQUIRED", 5 - rankInfo->ratingUpdateCount);
    }

    // Set info text
    float ratingTextHeight = requestFailed ? 1850 : 1740;
    ratingSubtextId = Text_AddSubtext(text, -1100, ratingTextHeight, ratingString);

    // Set color
    GXColor white = (GXColor){255, 255, 255, 255};
    GXColor red = (GXColor) {255, 0, 0, 255};
    Text_SetColor(text, rankSubtextId, &white);
    Text_SetColor(text, ratingSubtextId, requestFailed ? &red : &white);

    // Set scale of info text
    float ratingTextScale = isPlaced ? 4.5 : 4;
    Text_SetScale(text, ratingSubtextId, ratingTextScale, ratingTextScale);
}

void UpdateRankInfo() {
    bool hasRankChanged = rankInfoResp->ratingChange != 0 | rankInfoResp->rankChange != 0;
    bool isPlaced = rankInfoResp->ratingUpdateCount > 5;

    if (hasRankChanged && isPlaced) {
        UpdateRatingChange();
        UpdateRankChangeAnim();
    }
}

float interpRating(float ratingOrdinal, float ratingChange) {
    // Percent completion of rating increment
    float completion = currentFrame / (float) ratingChangeLen;
    return ratingOrdinal + ratingChange * completion;
}

int getRatingChangeSFX(float ratingChange) {
    if (rankInfoResp->ratingChange > 0) {
        return TICK_UP;
    }
    if (rankInfoResp->ratingChange < 0) {
        return TICK_DOWN;
    }
    else {
        return 0xD0;
    }
}

int getRankChangeSFX(float rankChange) {
    if (rankInfoResp->rankChange > 0) {
        return RANK_UP_SMALL;
    }
    else if (rankInfoResp->rankChange < 0) {
        return RANK_DOWN_SMALL;
    }
    else {
        return 0xD0;
    }
}

void UpdateRankChangeAnim() {
    if ( currentFrame >= ratingChangeLen ) {
        float rankAnimFrame = RANK_CHANGE_LEN - (currentFrame - ratingChangeLen);
        if ( rankAnimFrame > 0.f ) {
            JOBJ_ForEachAnim(rankIconJobj, 6, 0x20, AOBJ_ReqAnim, 1, rankAnimFrame); // HSD_TypeMask::JOBJ 0x20
            JOBJ_AnimAll(rankIconJobj);
        }
    }
}

void UpdateRatingChange() {
    // Colors
    GXColor green = (GXColor) {3, 252, 28, 255};
    GXColor red = (GXColor) {255, 0, 0, 255};
    GXColor white = (GXColor) {255, 255, 255, 255};

    // Increment rating ordinal text
    if (currentFrame % 2 == 1 && currentFrame < ratingChangeLen) {
		// Calculate rating increment for counting effect
		float displayRating = interpRating(
				rankInfoResp->ratingOrdinal,
				rankInfoResp->ratingChange
			);
		char* ratingString[6];

		sprintf(ratingString, "%0.1f", displayRating);
		Text_SetText(text, ratingSubtextId, ratingString);

		u8 tickVolume = 0;
		if (rankInfoResp->ratingChange > 0) {
			tickVolume = TICK_UP_VOL;
		}
		if (rankInfoResp->ratingChange < 0) {
			tickVolume = TICK_DOWN_VOL;
		}

        // Play tick sound effect during rating change
        SFX_PlayRaw(
			getRatingChangeSFX(rankInfoResp->ratingChange),
			tickVolume,
			64, 0, 0
		);
    }
    else if (currentFrame == ratingChangeLen) {
        // Set rating text to exact amount to prevent interp inaccuracies
		char* ratingString[6];
		sprintf(ratingString, "%0.1f", rankInfoResp->ratingOrdinal + rankInfoResp->ratingChange);
		Text_SetText(text, ratingSubtextId, ratingString);
    }

    if (currentFrame == ratingChangeLen + RANK_CHANGE_LEN) {
        // Play sound for rank up / down
        SFX_PlayRaw(getRankChangeSFX(rankInfoResp->ratingOrdinal), 255, 64, 0, 0);
    }

    if (currentFrame < ratingChangeLen + RANK_CHANGE_LEN + RATING_NOTIFICATION_LEN) {
        if (currentFrame == ratingChangeLen + (int) (RANK_CHANGE_LEN / 2) + 1) {
            // Handle rank-up SFX and show rating change notification
            if (rankInfoResp->ratingChange != 0) {
                // Create rating change text
                char* changeString[7];
                sprintf(changeString, "%0.1f", fabs(rankInfoResp->ratingChange));

                // Chars for + / - with null terminator
                int plus = 0x817B0000;
                int minus = 0x817C0000;
                int* signString = rankInfoResp->ratingChange > 0 ? &plus : &minus;

                // Create subtext for sign
                changeSignSubtextId = Text_AddSubtext(text, -600, 1750, signString);

                // Create subtext for rating change
                ratingChangeSubtextId = Text_AddSubtext(text, -475, 1750, changeString);
                Text_SetScale(text, changeSignSubtextId, 4, 4);
                Text_SetScale(text, ratingChangeSubtextId, 4, 4);

                // Determine text color
                if (rankInfoResp->ratingChange > 0) {
                    Text_SetColor(text, changeSignSubtextId, &green);
                    Text_SetColor(text, ratingChangeSubtextId, &green);
                    // Play sfx for end of counting
                    SFX_PlayRaw(RATING_INCREASE, 255, 64, 0, 0);
                }
                if (rankInfoResp->ratingChange < 0) {
                    Text_SetColor(text, changeSignSubtextId, &red);
                    Text_SetColor(text, ratingChangeSubtextId, &red);
                    // Play sfx for end of counting
                    SFX_PlayRaw(RATING_DECREASE, 255, 64, 0, 0);
                }
            }

            // Set new rank icon and text
            if (rankInfoResp->rankChange != 0) {
                // Update rank icon
                u8 newRank = (int) rankInfoResp->rank + rankInfoResp->rankChange;
                SetRankIcon(newRank);
                // Update rank text
                Text_SetText(text, rankSubtextId, RANK_STRINGS[(int) rankInfoResp->rank + rankInfoResp->rankChange]);
            }
        }

        // Clear notification string after rating change
        if (currentFrame == ratingChangeLen + RANK_CHANGE_LEN + RATING_NOTIFICATION_LEN - 1)
        {
            if (ratingChangeSubtextId != 0) {
                Text_SetText(text, changeSignSubtextId, "");
                Text_SetText(text, ratingChangeSubtextId, "");
            }
        }
    }
    currentFrame++;
}

#endif SLIPPI_CSS_RANK_INFO_C
