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
	// framesLeft = NARRATOR_LEN + RATING_CHANGE_LEN + FADE_OUT_LEN;
	// framesLeft = NARRATOR_LEN + RANK_CHANGE_LEN;

	/*
	 * START_DELAY + RATING_CHANGE_LEN + RANK_CHANGE_LEN
	 * change start delay depending on amount gained
		 * low rating gain -> high start delay (RATING_CHANGE_LEN * 0.33)
		 * med rating gain -> med start delay (RATING_CHANGE_LEN * 0.66)
		 * high rating gain -> no start delay (RATING_CHANGE_LEN * 1.0)
	 */

	rankInfoResp = calloc(sizeof(ExiSlippi_GetRank_Response));

	// Send dolphin command to pull rank data
	/*
	ExiSlippi_GetRank_Query *q = calloc(sizeof(ExiSlippi_GetRank_Query));
	q->command = ExiSlippi_Command_GET_RANK;
	ExiSlippi_Transfer(q, sizeof(ExiSlippi_GetRank_Query), ExiSlippi_TransferMode_WRITE);
	ExiSlippi_Transfer(rankInfoResp, sizeof(ExiSlippi_GetRank_Response), ExiSlippi_TransferMode_READ);
	*/

	u8 rank = RANK_GOLD_1;
	rankInfoResp->rank = rank;
	rankInfoResp->ratingOrdinal = 1500.f;
	rankInfoResp->global = 0;
	rankInfoResp->regional = 0;
	rankInfoResp->ratingUpdateCount = 234;
	rankInfoResp->ratingChange = 5.3f;
	rankInfoResp->rankChange = 1;

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

	OSReport("ratingChangeLen: %d\n", ratingChangeLen);

    InitRankInfoText(rankInfoResp);

    SlippiCSSDataTable *dt = GetSlpCSSDT();
	InitRankIcon(dt->SlpCSSDatAddress, rank);
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

    // Set rank icon
    SetRankIcon(rank);

    // Initialize bob animation
    JOBJ_ForEachAnim(rankIconJobj, 6, 0x20, AOBJ_ReqAnim, 1, 0.f); // HSD_TypeMask::JOBJ 0x20
    JOBJ_AnimAll(rankIconJobj);
    JOBJ_ForEachAnim(rankIconJobj, 6, 0x20, AOBJ_StopAnim, 6, 0, 0);

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

    // Check if placement matches have been completed
    if (rankInfo->ratingUpdateCount >= PLACEMENT_THRESHOLD) {
        // Get rating ordinal as string
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
        UpdateRankChangeAnim();
    }
}

float incrementRating(float ratingOrdinal, float ratingChange) {
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

    if (currentFrame % 2 == 1 && currentFrame < ratingChangeLen) {
		// Calculate rating increment for counting effect
		float displayRating = incrementRating(
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

    // TODO :: Adjust this timing by 1 or 2 frames
    if (currentFrame == ratingChangeLen + RANK_CHANGE_LEN) {
        // Play sound for rank up / down
        SFX_PlayRaw(getRankChangeSFX(rankInfoResp->ratingOrdinal), 255, 64, 0, 0);
    }

    if (currentFrame < ratingChangeLen + RANK_CHANGE_LEN) {
        if (currentFrame == ratingChangeLen + (int) (RANK_CHANGE_LEN / 2) + 1) {
            if (rankInfoResp->ratingChange != 0) {
                // Create rating change text
                char* changeString[7];
                sprintf(changeString, "%0.1f", rankInfoResp->ratingChange);

                // Create new subtext for rating change
                ratingChangeSubtextId = Text_AddSubtext(text, -650, 1750, changeString);
                Text_SetScale(text, ratingChangeSubtextId, 4, 4);
                // Determine text color
                if (rankInfoResp->ratingChange > 0) {
                    Text_SetColor(text, ratingChangeSubtextId, &green);
                    // Play sfx for end of counting
                    SFX_PlayRaw(RATING_INCREASE, 255, 64, 0, 0);
                }
                if (rankInfoResp->ratingChange < 0) {
                    Text_SetColor(text, ratingChangeSubtextId, &red);
                    // Play sfx for end of counting
                    SFX_PlayRaw(RATING_DECREASE, 255, 64, 0, 0);
                }
            }
            if (rankInfoResp->rankChange != 0) {
                // Update rank icon
                u8 newRank = (int) rankInfoResp->rank + rankInfoResp->rankChange;
                SetRankIcon(newRank);
                // Update rank text
                Text_SetText(text, rankSubtextId, RANK_STRINGS[(int) rankInfoResp->rank + rankInfoResp->rankChange]);
            }
        }
    }
    currentFrame++;
}

#endif SLIPPI_CSS_RANK_INFO_C
