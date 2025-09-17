#ifndef SLIPPI_CSS_RANK_INFO_C
#define SLIPPI_CSS_RANK_INFO_C

#include "RankInfo.h"

#include "../../../Files.h"
#include "../../../Game/Sounds.h"
#include "../../../Game/SysText.c"
#include "../../../Slippi.h"

ExiSlippi_GetRank_Response* rankInfoResp;
JOBJ* rankIconJobj;
uint ratingUpdateTimer = 0;
uint ratingChangeLen;  // Rating change sequence length
uint loaderCount = 0;
uint loadTimer = 0;

static char BUFFER[24] = {0};

void GetRankInfo(ExiSlippi_GetRank_Response* resp) {
  // Get cached rank info from rust
  ExiSlippi_GetRank_Query* q = calloc(sizeof(ExiSlippi_GetRank_Query));
  q->command = ExiSlippi_Command_GET_RANK;
  ExiSlippi_Transfer(q, sizeof(ExiSlippi_GetRank_Query), ExiSlippi_TransferMode_WRITE);
  ExiSlippi_Transfer(resp, sizeof(ExiSlippi_GetRank_Response), ExiSlippi_TransferMode_READ);
  HSD_Free(q);
}

void FetchRankInfo() {
  // OSReport("Fetching rank info...\n");
  // Send dolphin command to pull rank data
  ExiSlippi_FetchRank_Query* q = calloc(sizeof(ExiSlippi_FetchRank_Query));
  q->command = ExiSlippi_Command_FETCH_RANK;
  ExiSlippi_Transfer(q, sizeof(ExiSlippi_FetchRank_Query), ExiSlippi_TransferMode_WRITE);
  HSD_Free(q);
}

void InitRankInfo() {
  // Allocate rank info data
  rankInfoResp = calloc(sizeof(ExiSlippi_GetRank_Response));

  // Load byte from address 0x80479D34. Stores previous minor scene index.
  u8 previousMinor = *(u8*)0x80479D34;
  isMenuTransition = previousMinor == 0;

  // Get rank info before fetching, this is the previous game's state
  GetRankInfo(rankInfoResp);

  bool localRankVisible = rankInfoResp->visibility & (1 << VISIBILITY_LOCAL);
  if (!localRankVisible) {
    // OSReport("rank info disabled %d\n", rankInfoResp->visibility);
    return;
  }

  // Send dolphin command to pull rank data. Only request a fetch when we transition from in-game / game setup.
  // Previous minor version will be 0 only when transitioning from the menus, I think.
  if (!isMenuTransition) {
    FetchRankInfo();
  }

  SlippiCSSDataTable* dt = GetSlpCSSDT();

  // If we are coming from CSS, always show current rank with no animations.
  // We will also allow the rank to change if when we load the CSS initially we are currently fetching or in error
  allowRankChanges = !isMenuTransition || rankInfoResp->status != RankInfo_FetchStatus_FETCHED;

  // If we are coming from in-game, we should show the current rank in the data at the start because we haven't
  // loaded the new rank data yet. We only show prev rank at the start once we've loaded the new data.
  InitRankIcon(dt->SlpCSSDatAddress, rankInfoResp->rank);
  InitRankInfoText(
      rankInfoResp->rank,
      rankInfoResp->ratingOrdinal,
      rankInfoResp->ratingUpdateCount,
      rankInfoResp->status);
}

void SetRankIcon(u8 rank) {
  if (rankIconJobj) {
    // Set rank icon
    JOBJ_ForEachAnim(rankIconJobj, 6, 0x400, AOBJ_ReqAnim, 1, (float)rank);  // HSD_TypeMask::TOBJ 0x400
    JOBJ_AnimAll(rankIconJobj);
    JOBJ_ForEachAnim(rankIconJobj, 6, 0x400, AOBJ_StopAnim, 6, 0, 0);
  }
}


// Returns length of the rating string
void SetRankText(u8 rank, float rating, uint matches_played, RankInfo_FetchStatus status) {
  // Set rank name string
  Text_SetText(text, rankSubtextId, RANK_STRINGS[rank]);
  Text_SetColor(text, rankSubtextId, &GX_COLORS.WHITE);

  // OSReport("Rank / rating text being set. Status: %d\n", status);

  // If fetching and we have no rank yet, show question marks
  // This will show them both while we are in pending or if a fetch failed. That's fine
  bool isFetching = status == RankInfo_FetchStatus_FETCHING;
  if (isFetching && rank == 0) {
    // Show question mark if match data is unreported
    Text_SetText(text, ratingSubtextId, QUESTION_MARKS);
    // Gray out rating text if elo is pending
    Text_SetColor(text, ratingSubtextId, &GX_COLORS.GRAY);
    // Indicate the string is longer because the question marks are wide
    lastRatingTextLen = 6;
    return;
  }

  // Check if the user has completed their placement matches
  if (matches_played >= PLACEMENT_THRESHOLD) {
    // Show rating if placed
    sprintf(BUFFER, "%0.1f", rating);
  } else {
    // Show remaining number of sets if not placed
    sprintf(BUFFER, "%d SETS REQUIRED", 5 - matches_played);
  }

  Text_SetText(text, ratingSubtextId, BUFFER);
  Text_SetColor(text, ratingSubtextId, &GX_COLORS.LIGHT_GRAY);

  lastRatingTextLen = strlen(BUFFER);
}

void InitRankIcon(SlpCSSDesc* slpCss, u8 rank) {
  GOBJ* gobj = GObj_Create(0x4, 0x5, 0x80);
  rankIconJobj = JOBJ_LoadJoint(slpCss->rankIcons->jobj);

  rankIconJobj->trans.X = -9.f;
  rankIconJobj->trans.Y = -12.5f;
  rankIconJobj->scale.X = 2.f;
  rankIconJobj->scale.Y = 2.f;
  JOBJ_SetMtxDirtySub(rankIconJobj);

  // Get all animations
  JOBJ_AddSetAnim(rankIconJobj, slpCss->rankIcons, 0);
  JOBJ_ReqAnimAll(rankIconJobj, 0.0);

  SetRankIcon(rank);

  // Initialize bob animation
  JOBJ_ForEachAnim(rankIconJobj, 6, 0x20, AOBJ_ReqAnim, 1, 0.f);  // HSD_TypeMask::JOBJ 0x20
  JOBJ_AnimAll(rankIconJobj);
  JOBJ_ForEachAnim(rankIconJobj, 6, 0x20, AOBJ_StopAnim, 6, 0, 0);

  GObj_AddObject(gobj, 0x4, rankIconJobj);
  GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
}

void InitRankInfoText(u8 rank, float rating, uint matches_played, RankInfo_FetchStatus status) {
  // Create text object for rank info
  text = Text_CreateText(0, 0);
  text->kerning = 1;
  text->align = 0;
  text->use_aspect = 1;
  text->scale = (Vec2){0.01, 0.01};
  text->aspect.X *= 2.5;

  // Create rank label text
  rankLabelSubtextId = Text_AddSubtext(text, -1110, 850, "Rank");
  Text_SetScale(text, rankLabelSubtextId, 4, 4);
  Text_SetColor(text, rankLabelSubtextId, &GX_COLORS.GRAY);

  // Create rank text
  rankSubtextId = Text_AddSubtext(text, -640, 1100, "");
  Text_SetScale(text, rankSubtextId, 4, 4);
  // Create rating text
  ratingSubtextId = Text_AddSubtext(text, -640, 1250, "");
  Text_SetScale(text, ratingSubtextId, 3.5, 3.5);
  // Set text
  SetRankText(rank, rating, matches_played, status);

  // Initialize rank loader
  loaderSubtextId = Text_AddSubtext(text, -640, 1224, "");
  Text_SetScale(text, loaderSubtextId, 4.25, 4.25);
  Text_SetColor(text, loaderSubtextId, &GX_COLORS.YELLOW);
}

void UpdateRankChangeAnim() {
  if (ratingUpdateTimer >= ratingChangeLen) {
    const int rankAnimFrame = ratingUpdateTimer - ratingChangeLen;
    if (rankAnimFrame > 0 && rankAnimFrame < RANK_CHANGE_LEN) {
      JOBJ_ForEachAnim(rankIconJobj, 6, 0x20, AOBJ_ReqAnim, 1, (float)rankAnimFrame);  // HSD_TypeMask::JOBJ 0x20
      JOBJ_AnimAll(rankIconJobj);
    }
  }
}

void UpdateRankInfo() {
  bool enabled = rankInfoResp->visibility & (1 << VISIBILITY_LOCAL);
  if (!enabled) {
    return;
  }

  // See where this is set for more info about when this happens.
  if (!allowRankChanges) {
    return;
  }

  // Make sure the rank info is up to date
  GetRankInfo(rankInfoResp);

  u8 responseStatus = rankInfoResp->status;
  s8 rank = rankInfoResp->rank;

  if (responseStatus == RankInfo_FetchStatus_FETCHED) {
    // bool error = rankInfoResp->status == RankInfo_ResponseStatus_ERROR;
    bool hasChanged = rankInfoResp->ratingChange != 0 || rankInfoResp->rankChange != 0;
    bool isPlaced = rankInfoResp->ratingUpdateCount >= PLACEMENT_THRESHOLD;

    // Only initialize the fetched status if we didn't previously process a fetched response status
    if (lastExecutedStatus != LAST_EXECUTED_STATUS_FETCHED) {
      // Determine the duration of the rating increase / decrease
      float change = rankInfoResp->ratingChange;
      if (fabs(change) < LOW_RATING_THRESHOLD) {
        ratingChangeLen = RATING_CHANGE_LEN * 0.25f;
      } else if (fabs(change) < MED_RATING_THRESHOLD) {
        ratingChangeLen = RATING_CHANGE_LEN * 0.5f;
      } else {
        ratingChangeLen = RATING_CHANGE_LEN;
      }

      SlippiCSSDataTable* dt = GetSlpCSSDT();
      // Here we load the "previous" values because we are about to transition from those to the current.
      uint matchesPlayed = rankInfoResp->ratingUpdateCount;
      u8 prevRank = rankInfoResp->rank - rankInfoResp->rankChange;
      float prevRating = rankInfoResp->ratingOrdinal - rankInfoResp->ratingChange;
      SetRankIcon(prevRank);
      SetRankText(prevRank, prevRating, matchesPlayed, responseStatus);

      // Clear loader
      Text_SetText(text, loaderSubtextId, "");
    } else if (isPlaced) {
      UpdateRatingChange();
      int rankChange = rankInfoResp->rankChange;
      if (rankChange != 0) {
        UpdateRankChangeAnim();
      }
    }

    // Allow other states to be initialized again
    lastExecutedStatus = LAST_EXECUTED_STATUS_FETCHED;
  } else if (responseStatus == RankInfo_FetchStatus_ERROR) {
    // Initialize error state
    if (lastExecutedStatus != LAST_EXECUTED_STATUS_ERROR) {
      // Clear loader
      Text_SetText(text, loaderSubtextId, "");

      // Dislay rank fetch error
      Text_SetText(text, rankSubtextId, "Error");
      Text_SetColor(text, rankSubtextId, &GX_COLORS.WHITE);
      Text_SetText(text, ratingSubtextId, "Failed to get rank");
      Text_SetColor(text, ratingSubtextId, &GX_COLORS.RED);
    }

    // Prevent error logic from looping, allow rank initialization again
    lastExecutedStatus = LAST_EXECUTED_STATUS_ERROR;
  } else if (rankInfoResp->status == RankInfo_FetchStatus_FETCHING) {
    // Initialize fetching state
    if (lastExecutedStatus != LAST_EXECUTED_STATUS_FETCHING) {
      // Set the rank icon and text values based on what data we previously had. That means loading
      // the "current" values because we haven't loaded the new stuff yet.
      uint matchesPlayed = rankInfoResp->ratingUpdateCount;
      SetRankIcon(rankInfoResp->rank);
      SetRankText(rankInfoResp->rank, rankInfoResp->ratingOrdinal, matchesPlayed, responseStatus);

      // Set loader position
      float loaderPosX = -640.f + (lastRatingTextLen * 60.f);
      Text_SetPosition(text, loaderSubtextId, loaderPosX, 1224);
    }

    // Update unreported loader
    if (loadTimer % 15 == 0) {
      switch (loaderCount) {
        case 0: {
          Text_SetText(text, loaderSubtextId, ".");
          break;
        }
        case 1: {
          Text_SetText(text, loaderSubtextId, "..");
          break;
        }
        case 2: {
          Text_SetText(text, loaderSubtextId, "...");
          break;
        }
      }
      loaderCount = (loaderCount + 1) % 3;
    }

    loadTimer++;

    // Reset rank initialization and error shown flags
    lastExecutedStatus = LAST_EXECUTED_STATUS_FETCHING;
  }
}

float InterpRating(float ratingOrdinal, float ratingChange) {
  // Percent completion of rating increment
  float completion = ratingUpdateTimer / (float)ratingChangeLen;
  return (ratingOrdinal - ratingChange) + (ratingChange * completion);
}

int GetRatingChangeSFX(float ratingChange) {
  if (rankInfoResp->ratingChange > 0) {
    return TICK_UP;
  }
  if (rankInfoResp->ratingChange < 0) {
    return TICK_DOWN;
  } else {
    return 0xD0;
  }
}

int GetRankChangeSFX() {
  if (rankInfoResp->rankChange > 0) {
    return RANK_UP_SMALL;
  } else if (rankInfoResp->rankChange < 0) {
    return RANK_DOWN_SMALL;
  } else {
    return 0xD0;
  }
}

void UpdateRatingChange() {
  // Increment rating ordinal text
  if (ratingUpdateTimer % 2 == 1 && ratingUpdateTimer < ratingChangeLen) {
    // Calculate rating increment for counting effect
    float displayRating = InterpRating(
        rankInfoResp->ratingOrdinal,
        rankInfoResp->ratingChange);


    sprintf(BUFFER, "%0.1f", displayRating);
    Text_SetText(text, ratingSubtextId, BUFFER);

    u8 tickVolume = 0;
    if (rankInfoResp->ratingChange > 0) {
      tickVolume = TICK_UP_VOL;
    }
    if (rankInfoResp->ratingChange < 0) {
      tickVolume = TICK_DOWN_VOL;
    }

    // Play tick sound effect during rating change
    SFX_PlayRaw(
        GetRatingChangeSFX(rankInfoResp->ratingChange),
        tickVolume,
        64, 0, 0);
  } else if (ratingUpdateTimer == ratingChangeLen) {
    // Set rating text to exact amount to prevent interp inaccuracies
    sprintf(BUFFER, "%0.1f", rankInfoResp->ratingOrdinal);
    Text_SetText(text, ratingSubtextId, BUFFER);
  }

  if (ratingUpdateTimer == ratingChangeLen + RANK_CHANGE_LEN) {
    // Play sound for rank up / down
    SFX_PlayRaw(GetRankChangeSFX(), 255, 64, 0, 0);
  }

  if (ratingUpdateTimer < ratingChangeLen + RANK_CHANGE_LEN + RATING_NOTIFICATION_LEN) {
    if (ratingUpdateTimer == ratingChangeLen + (int)(RANK_CHANGE_LEN / 2) + 1) {
      // Handle rank-up SFX and show rating change notification
      if (rankInfoResp->ratingChange != 0) {
        // Create rating change text
        char* signString = rankInfoResp->ratingChange > 0 ? "\x81\x7B" : "\x81\x7C";
        sprintf(BUFFER, "%s%0.1f", signString, fabs(rankInfoResp->ratingChange));

        // Create subtext for rating change
        ratingChangeSubtextId = Text_AddSubtext(text, -225, 1260, BUFFER);
        Text_SetScale(text, ratingChangeSubtextId, 3.0, 3.0);

        // Determine text color
        if (rankInfoResp->ratingChange > 0) {
          Text_SetColor(text, ratingChangeSubtextId, &GX_COLORS.GREEN);
          // Play sfx for end of counting
          SFX_PlayRaw(RATING_INCREASE, 255, 64, 0, 0);
        }
        if (rankInfoResp->ratingChange < 0) {
          Text_SetColor(text, ratingChangeSubtextId, &GX_COLORS.RED);
          // Play sfx for end of counting
          SFX_PlayRaw(RATING_DECREASE, 255, 64, 0, 0);
        }
      } else {
        // In the case where there is no rating change, assume the result did not go through
        ratingChangeSubtextId = Text_AddSubtext(text, -225, 1260, "result pending");
        Text_SetScale(text, ratingChangeSubtextId, 3.0, 3.0);
        Text_SetColor(text, ratingChangeSubtextId, &GX_COLORS.YELLOW);
      }

      // Set new rank icon and text
      if (rankInfoResp->rankChange != 0) {
        // Update rank icon
        u8 newRank = rankInfoResp->rank;
        SetRankIcon(newRank);
        // Update rank text
        float newRating = rankInfoResp->ratingOrdinal;
        u8 matchesPlayed = rankInfoResp->ratingUpdateCount;
        SetRankText(newRank, newRating, matchesPlayed, RankInfo_FetchStatus_FETCHED);
      }
    }

    // Clear notification string after rating change
    if (ratingUpdateTimer == ratingChangeLen + RANK_CHANGE_LEN + RATING_NOTIFICATION_LEN - 1) {
      if (ratingChangeSubtextId != 0) {
        Text_SetText(text, ratingChangeSubtextId, "");
      }
    }
  }
  ratingUpdateTimer++;
}

#endif SLIPPI_CSS_RANK_INFO_C
