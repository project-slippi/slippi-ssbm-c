#ifndef SLIPPI_CSS_RANK_INFO_C
#define SLIPPI_CSS_RANK_INFO_C

#include "RankInfo.h"

#include "../../../Common.h"
#include "../../../ExiSlippi.h"
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

void GetRankInfo(ExiSlippi_GetRank_Response* resp) {
  // Get cached rank info from rust
  ExiSlippi_GetRank_Query* q = calloc(sizeof(ExiSlippi_GetRank_Query));
  q->command = ExiSlippi_Command_GET_RANK;
  ExiSlippi_Transfer(q, sizeof(ExiSlippi_GetRank_Query), ExiSlippi_TransferMode_WRITE);
  ExiSlippi_Transfer(resp, sizeof(ExiSlippi_GetRank_Response), ExiSlippi_TransferMode_READ);
  HSD_Free(q);
}

void FetchRankInfo() {
  OSReport("Fetching rank info...");
  // Send dolphin command to pull rank data
  ExiSlippi_FetchRank_Query* q = calloc(sizeof(ExiSlippi_FetchRank_Query));
  q->command = ExiSlippi_Command_FETCH_RANK;
  ExiSlippi_Transfer(q, sizeof(ExiSlippi_FetchRank_Query), ExiSlippi_TransferMode_WRITE);
  HSD_Free(q);
}

void InitRankInfo() {
  // Allocate rank info data
  rankInfoResp = calloc(sizeof(ExiSlippi_GetRank_Response));

  GetRankInfo(rankInfoResp);

  bool localRankVisible = rankInfoResp->visibility & (1 << VISIBILITY_LOCAL);
  if (!localRankVisible) {
    OSReport("rank info disabled %d\n", rankInfoResp->visibility);
    return;
  }

  // DEBUG
  // rankInfoResp->status = RankInfo_ResponseStatus_UNREPORTED;
  // rankInfoResp->rank = RANK_MASTER_3;
  // rankInfoResp->ratingOrdinal = 1345.4f;
  // rankInfoResp->global = 0;
  // rankInfoResp->regional = 0;
  // rankInfoResp->ratingUpdateCount = 100;
  // rankInfoResp->rankChange = 0;
  // rankInfoResp->ratingChange = 0.f;

  //   OSReport("InitRankInfo()\n");
  //   OSReport("user status: %d\n", rankInfoResp->status);
  //   OSReport("user rank: %d\n", rankInfoResp->rank);
  //   OSReport("user rating: %f\n", rankInfoResp->ratingOrdinal);
  //   OSReport("user global: %d\n", rankInfoResp->global);
  //   OSReport("user regional: %d\n", rankInfoResp->regional);
  //   OSReport("user rank change: %d\n", rankInfoResp->rankChange);
  //   OSReport("user rating change: %f\n", rankInfoResp->ratingChange);

  SlippiCSSDataTable* dt = GetSlpCSSDT();

  s8 rank = rankInfoResp->rank;
  InitRankIcon(dt->SlpCSSDatAddress, rankInfoResp->rank);
  InitRankInfoText(
      rank < 0 ? rank + 1 : rank,  // Show pending if rank is empty
      rankInfoResp->ratingOrdinal,
      rankInfoResp->ratingUpdateCount,
      rankInfoResp->status == RankInfo_ResponseStatus_UNREPORTED);

  // Send dolphin command to pull rank data
  FetchRankInfo();
}

void SetRankIcon(u8 rank) {
  if (rankIconJobj) {
    // Set rank icon
    JOBJ_ForEachAnim(rankIconJobj, 6, 0x400, AOBJ_ReqAnim, 1, (float)rank);  // HSD_TypeMask::TOBJ 0x400
    JOBJ_AnimAll(rankIconJobj);
    JOBJ_ForEachAnim(rankIconJobj, 6, 0x400, AOBJ_StopAnim, 6, 0, 0);
  }
}

void SetRankText(u8 rank, float rating, uint matches_played, bool unreported) {
  GXColor white = (GXColor){255, 255, 255, 255};
  GXColor gray = (GXColor){150, 150, 150, 255};
  char* rankString[15];

  // Set rank name string
  sprintf(rankString, RANK_STRINGS[rank]);

  Text_SetText(text, rankSubtextId, rankString);
  Text_SetScale(text, rankSubtextId, 5, 5);
  Text_SetColor(text, rankSubtextId, &white);

  float RANK_TEXT_HEIGHT = 1620;
  Text_SetPosition(text, rankSubtextId, -1100, RANK_TEXT_HEIGHT);

  // Check if the user has completed their placement matches
  bool isPlaced = matches_played >= PLACEMENT_THRESHOLD;
  char* ratingString[15];
  // Check if the user has completed their placement matches
  if (isPlaced) {
    // Show rating if placed
    sprintf(ratingString, "%0.1f", rating);
  } else {
    // Show remaining number of sets if not placed
    sprintf(ratingString, "%d SETS REQUIRED", 5 - matches_played);
  }

  float ratingTextScale = (isPlaced || unreported) ? 4.5f : 4.f;
  Text_SetText(text, ratingSubtextId, ratingString);
  Text_SetScale(text, ratingSubtextId, ratingTextScale, ratingTextScale);

  float RATING_TEXT_HEIGHT = 1820;
  Text_SetPosition(text, ratingSubtextId, -1100, RATING_TEXT_HEIGHT);
  if (unreported) {
    // Create question mark if match data is unreported
    unsigned short* questionMark = calloc(9);
    questionMark[0] = 0x8148;  // '?'
    questionMark[1] = 0x8148;  // '?'
    questionMark[2] = 0x8148;  // '?'
    questionMark[3] = 0x8148;  // '?'

    // Gray out rating text if elo is pending
    Text_SetText(text, ratingSubtextId, questionMark);
    Text_SetColor(text, ratingSubtextId, &gray);
  } else {
    Text_SetColor(text, ratingSubtextId, &white);
  }
}

void InitRankIcon(SlpCSSDesc* slpCss, u8 rank) {
  GOBJ* gobj = GObj_Create(0x4, 0x5, 0x80);
  rankIconJobj = JOBJ_LoadJoint(slpCss->rankIcons->jobj);

  rankIconJobj->trans.X = -7.3f;
  rankIconJobj->trans.Y = -11.0f;
  rankIconJobj->scale.X = 3.15f;
  rankIconJobj->scale.Y = 3.15f;
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

void InitRankInfoText(u8 rank, float rating, uint matches_played, bool unreported) {
  // Create text object for rank info
  text = Text_CreateText(0, 0);
  text->kerning = 1;
  text->align = 0;
  text->use_aspect = 1;
  text->scale = (Vec2){0.01, 0.01};
  text->aspect.X *= 2.5;

  GXColor white = (GXColor){255, 255, 255, 255};
  GXColor gray = (GXColor){150, 150, 150, 255};
  GXColor red = (GXColor){255, 0, 0, 255};
  GXColor yellow = (GXColor){255, 200, 0, 255};
  GXColor blue = (GXColor){60, 188, 255, 255};

  // Create rank text
  rankSubtextId = Text_AddSubtext(text, -1100, 1540, "");
  // Create rating text
  ratingSubtextId = Text_AddSubtext(text, -1100, 1740, "");
  // Set text
  SetRankText(rank, rating, matches_played, unreported);

  // Initialize rank loader
  bool isPlaced = matches_played >= PLACEMENT_THRESHOLD;
  float loaderPosX = (!isPlaced && unreported) ? -700.f : -650.f;
  loaderSubtextId = Text_AddSubtext(text, loaderPosX, 1840, "");
  Text_SetScale(text, loaderSubtextId, 4.25, 4.25);
  Text_SetColor(text, loaderSubtextId, &yellow);
}

void UpdateRankInfo() {
  bool enabled = rankInfoResp->visibility & (1 << VISIBILITY_LOCAL);
  if (!enabled) {
    return;
  }

  // Check if rank fetcher has timed out after retrying
  bool timeout = loadTimer > (RETRY_FETCH_0_LEN + RETRY_FETCH_1_LEN);

  u8 responseStatus = rankInfoResp->status;
  s8 rank = rankInfoResp->rank;
  if (rank > 0 && responseStatus != RankInfo_ResponseStatus_UNREPORTED) {
    // bool error = rankInfoResp->status == RankInfo_ResponseStatus_ERROR;
    bool hasRankChanged = rankInfoResp->ratingChange != 0 | rankInfoResp->rankChange != 0;
    bool isPlaced = rankInfoResp->ratingUpdateCount > PLACEMENT_THRESHOLD;

    // Only request rank info if this is ranked
    if (!rankInitialized) {
      rankInitialized = true;

      // OSReport("UpdateRankInfo()\n");
      // OSReport("user status: %d\n", rankInfoResp->status);
      // OSReport("user rank: %d\n", rankInfoResp->rank);
      // OSReport("user rating: %f\n", rankInfoResp->ratingOrdinal);
      // OSReport("user global: %d\n", rankInfoResp->global);
      // OSReport("user regional: %d\n", rankInfoResp->regional);
      // OSReport("user rank change: %d\n", rankInfoResp->rankChange);
      // OSReport("user rating change: %f\n", rankInfoResp->ratingChange);

      // Determine the duration of the rating increase / decrease
      float change = rankInfoResp->ratingChange;
      if (abs(change) < LOW_RATING_THRESHOLD) {
        ratingChangeLen = RATING_CHANGE_LEN * 0.25f;
      } else if (abs(change) < MED_RATING_THRESHOLD) {
        ratingChangeLen = RATING_CHANGE_LEN * 0.5f;
      } else {
        ratingChangeLen = RATING_CHANGE_LEN;
      }

      SlippiCSSDataTable* dt = GetSlpCSSDT();
      // Update rank information
      u8 rank = rankInfoResp->rank;
      float rating = rankInfoResp->ratingOrdinal;
      uint matchesPlayed = rankInfoResp->ratingUpdateCount;
      bool unreported = responseStatus == RankInfo_ResponseStatus_UNREPORTED;
      SetRankIcon(rank);
      SetRankText(rank, rating, matchesPlayed, unreported);

      // Clear loader
      Text_SetText(text, loaderSubtextId, "");
    } else if (hasRankChanged && isPlaced) {
      UpdateRatingChange();
      int rankChange = rankInfoResp->rankChange;
      if (rankChange != 0) {
        UpdateRankChangeAnim();
      }
    }
  } else {
    if (!timeout) {
      // Get rank data from rust
      GetRankInfo(rankInfoResp);

      /*
      // DEBUG
      if ( rankInfoResp->status != RankInfo_ResponseStatus_SUCCESS )
      {
          OSReport("loadTimer: %d\n", loadTimer);
          rankInfoResp->status = RankInfo_ResponseStatus_SUCCESS;
          rankInfoResp->rank = RANK_DIAMOND_3;
          rankInfoResp->ratingOrdinal = 1345.4f;
          rankInfoResp->global = 0;
          rankInfoResp->regional = 0;
          rankInfoResp->rankChange = 1;
          rankInfoResp->ratingChange = 102.f;
      }
      else {
          rankInfoResp->ratingChange = rankInfoResp->ratingOrdinal - 1345.4f;
      }
      */
    } else if (loadTimer == RETRY_FETCH_0_LEN + RETRY_FETCH_1_LEN + 1) {
      // Clear any rank text
      Text_SetText(text, rankSubtextId, "");
      Text_SetText(text, ratingSubtextId, "");
      // Clear loader
      Text_SetText(text, loaderSubtextId, "");

      // Dislay rank fetch error
      char* errorString[6];
      char* errorMsgString[19];

      sprintf(errorString, "Error");
      sprintf(errorMsgString, "Failed to get rank");

      GXColor white = (GXColor){255, 255, 255, 255};
      int errorSubtextId = Text_AddSubtext(text, -1100, 1540, errorString);
      Text_SetScale(text, errorSubtextId, 5, 5);
      Text_SetColor(text, errorSubtextId, &white);

      GXColor red = (GXColor){255, 0, 0, 255};
      int errorMsgSubtextId = Text_AddSubtext(text, -1100, 1790, errorMsgString);
      Text_SetScale(text, errorMsgSubtextId, 4, 4);
      Text_SetColor(text, errorMsgSubtextId, &red);
    }
  }

  if (rankInfoResp->status == RankInfo_ResponseStatus_UNREPORTED) {
    // Update unreported loader
    if (loadTimer % 15 == 0 && !timeout) {
      switch (loaderCount) {
        case 0: {
          char* dotString = ".";
          Text_SetText(text, loaderSubtextId, dotString);
          break;
        }
        case 1: {
          char* dotString = "..";
          Text_SetText(text, loaderSubtextId, dotString);
          break;
        }
        case 2: {
          char* dotString = "...";
          Text_SetText(text, loaderSubtextId, dotString);
          break;
        }
      }
      loaderCount++;
      loaderCount = loaderCount % 3;
    }

    // Periodically retry rank fetch to check for match report
    // Check first after 'Choose your character!', then 5 seconds after that
    if (loadTimer == RETRY_FETCH_0_LEN || loadTimer == RETRY_FETCH_1_LEN) {
      FetchRankInfo();

      // OSReport("Fetching rank info after %.2f seconds...\n", (float) loadTimer / 60.f);
      // OSReport("status: %d\n", rankInfoResp->status);
      // OSReport("rank: %d\n", rankInfoResp->rank);
      // OSReport("ratingOrdinal: %f\n", rankInfoResp->ratingOrdinal);
      // OSReport("global: %d\n", rankInfoResp->global);
      // OSReport("regional: %d\n", rankInfoResp->regional);
      // OSReport("ratingUpdateCount: %d\n", rankInfoResp->ratingUpdateCount);
      // OSReport("ratingChange: %f\n", rankInfoResp->ratingChange);
      // OSReport("rankChange: %d\n", rankInfoResp->rankChange);
      // OSReport("\n");

      SFX_PlayRaw(RATING_DECREASE, 255, 64, 0, 0);
    }
    loadTimer++;
  }
}

float InterpRating(float ratingOrdinal, float ratingChange) {
  // Percent completion of rating increment
  float completion = ratingUpdateTimer / (float)ratingChangeLen;
  return ratingOrdinal + ratingChange * completion;
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

void UpdateRankChangeAnim() {
  if (ratingUpdateTimer >= ratingChangeLen) {
    float rankAnimFrame = RANK_CHANGE_LEN - (ratingUpdateTimer - ratingChangeLen);
    if (rankAnimFrame > 0.f) {
      JOBJ_ForEachAnim(rankIconJobj, 6, 0x20, AOBJ_ReqAnim, 1, rankAnimFrame);  // HSD_TypeMask::JOBJ 0x20
      JOBJ_AnimAll(rankIconJobj);
    }
  }
}

void UpdateRatingChange() {
  // Colors
  GXColor green = (GXColor){3, 252, 28, 255};
  GXColor red = (GXColor){255, 0, 0, 255};
  GXColor white = (GXColor){255, 255, 255, 255};

  // Increment rating ordinal text
  if (ratingUpdateTimer % 2 == 1 && ratingUpdateTimer < ratingChangeLen) {
    // Calculate rating increment for counting effect
    float displayRating = InterpRating(
        rankInfoResp->ratingOrdinal,
        rankInfoResp->ratingChange);
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
        GetRatingChangeSFX(rankInfoResp->ratingChange),
        tickVolume,
        64, 0, 0);
  } else if (ratingUpdateTimer == ratingChangeLen) {
    // Set rating text to exact amount to prevent interp inaccuracies
    char* ratingString[6];
    sprintf(ratingString, "%0.1f", rankInfoResp->ratingOrdinal + rankInfoResp->ratingChange);
    Text_SetText(text, ratingSubtextId, ratingString);
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
        char* changeString[7];
        sprintf(changeString, "%0.1f", fabs(rankInfoResp->ratingChange));

        // Chars for + / - with null terminator
        int plus = 0x817B0000;
        int minus = 0x817C0000;
        int* signString = rankInfoResp->ratingChange > 0 ? &plus : &minus;

        // Create subtext for sign
        changeSignSubtextId = Text_AddSubtext(text, -600, 1830, signString);

        // Create subtext for rating change
        ratingChangeSubtextId = Text_AddSubtext(text, -475, 1830, changeString);
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
        u8 newRank = (int)rankInfoResp->rank + rankInfoResp->rankChange;
        SetRankIcon(newRank);
        // Update rank text
        float newRating = rankInfoResp->ratingOrdinal + rankInfoResp->ratingChange;
        u8 matchesPlayed = rankInfoResp->ratingUpdateCount;
        bool unreported = false;
        SetRankText(newRank, newRating, matchesPlayed, unreported);
      }
    }

    // Clear notification string after rating change
    if (ratingUpdateTimer == ratingChangeLen + RANK_CHANGE_LEN + RATING_NOTIFICATION_LEN - 1) {
      if (ratingChangeSubtextId != 0) {
        Text_SetText(text, changeSignSubtextId, "");
        Text_SetText(text, ratingChangeSubtextId, "");
      }
    }
  }
  ratingUpdateTimer++;
}

#endif SLIPPI_CSS_RANK_INFO_C
