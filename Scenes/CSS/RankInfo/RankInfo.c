#ifndef SLIPPI_CSS_RANK_INFO_C
#define SLIPPI_CSS_RANK_INFO_C

#include "RankInfo.h"

#include "../../../Files.h"
#include "../../../Game/Sounds.h"
#include "../../../Game/SysText.c"
#include "../../../Slippi.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))
#define ABSF(x) (((x) < 0.f) ? (-1.f*(x)) : (x))
#define ZEROS(x) memset(&(x), 0, sizeof(x))
#define CHECK_FLAG(field, bit) ((field) & (1 << (bit)))

static const char QUESTION_MARKS[] = "\x81\x48\x81\x48\x81\x48\x81\x48";  // "????"

static const char * RANK_STRINGS[] = {
    "PENDING",
    "BRONZE 1",
    "BRONZE 2",
    "BRONZE 3",
    "SILVER 1",
    "SILVER 2",
    "SILVER 3",
    "GOLD 1",
    "GOLD 2",
    "GOLD 3",
    "PLATINUM 1",
    "PLATINUM 2",
    "PLATINUM 3",
    "DIAMOND 1",
    "DIAMOND 2",
    "DIAMOND 3",
    "MASTER 1",
    "MASTER 2",
    "MASTER 3",
    "GRANDMASTER",
};

static const GXColors GX_COLORS = {
  /* WHITE      */ {255, 255, 255, 255},
  /* GRAY       */ {142, 145, 150, 255},
  /* RED        */ {255, 0, 0, 255},
  /* YELLOW     */ {255, 200, 0, 255},
  /* GREEN      */ {3, 252, 28, 255},
  /* LIGHT_GRAY */ {170, 173, 178, 255}
};

static char BUFFER[24];

static ExiSlippi_GetRank_Response rankInfoResp;

static GOBJ* gobj;
static JOBJ* rankIconJobj;

static uint ratingUpdateTimer; // A timer for tracking animation that starts at 0 and goes up
static uint ratingChangeLen; // How long the rating change sequence should last in frames
static uint loaderCount; // How many dots to display (0 == one, 1 == two, 2 == three)
static uint loadTimer; // A count-up timer that updates loaderCount every 15 frames

static Text*      text;
static SubtextIDs subtexts;
static int        lastRatingTextLen; // Used to offset loader

static bool isMenuTransition; // Set to true if we came from the menu, false if we came from in-game
static bool allowRankChanges; // Set in InitRankInfo
static u8 lastExecutedStatus; // Fetch status from Slippi Exi Transfer

void InitializeStaticVariables() {
  // Initialize data to zeros / null / defaults
  gobj = NULL;
  rankIconJobj = NULL;
  text = NULL;
  ratingUpdateTimer = 0;
  isMenuTransition = true;
  allowRankChanges = true;
  ZEROS(rankInfoResp);
  ZEROS(BUFFER);
  ZEROS(subtexts);
  lastExecutedStatus = LAST_EXECUTED_STATUS_NONE;
  lastRatingTextLen = 0;
}

void DeinitializeRankedPointers() {
  if (rankIconJobj) {
    JOBJ_RemoveAll(rankIconJobj);
  }
  if (gobj) {
    GObj_DestroyGXLink(gobj);
    GObj_FreeObject(gobj);
  }
  if (text) {
    Text_Destroy(text);
  }
}

void GetRankInfo() {
  // Get cached rank info from rust
  ExiSlippi_GetRank_Query* query = HSD_MemAlloc(sizeof(ExiSlippi_GetRank_Query));
  memset(query, 0, sizeof(ExiSlippi_GetRank_Query));
  query->command = ExiSlippi_Command_GET_RANK;
  ExiSlippi_Transfer(query, sizeof(ExiSlippi_GetRank_Query), ExiSlippi_TransferMode_WRITE);
  ExiSlippi_Transfer(&rankInfoResp, sizeof(ExiSlippi_GetRank_Response), ExiSlippi_TransferMode_READ);
  HSD_Free(query);
}

void FetchRankInfo() {
  // OSReport("Fetching rank info...\n");
  // Send dolphin command to pull rank data
  ExiSlippi_FetchRank_Query* query = HSD_MemAlloc(sizeof(ExiSlippi_FetchRank_Query));
  memset(query, 0, sizeof(ExiSlippi_FetchRank_Query));
  query->command = ExiSlippi_Command_FETCH_RANK;
  ExiSlippi_Transfer(query, sizeof(ExiSlippi_FetchRank_Query), ExiSlippi_TransferMode_WRITE);
  HSD_Free(query);
}

void InitRankInfo() {
  // Initialize our static variables we use in this file
  InitializeStaticVariables();

  // Load byte from address 0x80479D34. Stores previous minor scene index.
  u8 previousMinor = *((u8*)0x80479D34);
  isMenuTransition = previousMinor == 0;

  // Get rank info before fetching, this is the previous game's state
  GetRankInfo();

  if (CHECK_FLAG(rankInfoResp.visibility, VISIBILITY_LOCAL)) {
    // OSReport("rank info disabled %d\n", rankInfoResp.visibility);
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
  allowRankChanges = !isMenuTransition || rankInfoResp.status != RankInfo_FetchStatus_FETCHED;

  // If we are coming from in-game, we should show the current rank in the data at the start because we haven't
  // loaded the new rank data yet. We only show prev rank at the start once we've loaded the new data.
  InitRankIcon(dt->SlpCSSDatAddress, rankInfoResp.rank);
  InitRankInfoText(
      rankInfoResp.rank,
      rankInfoResp.ratingOrdinal,
      rankInfoResp.ratingUpdateCount,
      rankInfoResp.status);
}

void SetRankIcon(SlippiRank rank) {
  if (rankIconJobj) {
    // Set rank icon
    JOBJ_ForEachAnim(rankIconJobj, 6, 0x400, AOBJ_ReqAnim, 1, (float)rank);  // HSD_TypeMask::TOBJ 0x400
    JOBJ_AnimAll(rankIconJobj);
    JOBJ_ForEachAnim(rankIconJobj, 6, 0x400, AOBJ_StopAnim, 6, 0, 0);
  }
}

// Returns length of the rating string
void SetRankText(SlippiRank rank, float rating, uint matchesPlayed, RankInfo_FetchStatus status) {
  // Set rank name string
  Text_SetText(text, subtexts.rank, RANK_STRINGS[rank]);
  Text_SetColor(text, subtexts.rank, &GX_COLORS.WHITE);

  // OSReport("Rank / rating text being set. Status: %d\n", status);

  // If fetching and we have no rank yet, show question marks
  // This will show them both while we are in pending or if a fetch failed. That's fine
  if ((status == RankInfo_FetchStatus_FETCHING) && rank == 0) {
    // Show question mark if match data is unreported
    Text_SetText(text, subtexts.rating, QUESTION_MARKS);
    // Gray out rating text if elo is pending
    Text_SetColor(text, subtexts.rating, &GX_COLORS.GRAY);
    // Indicate the string is longer because the question marks are wide
    lastRatingTextLen = 6;
    return;
  }

  // Check if the user has completed their placement matches
  if (matchesPlayed >= PLACEMENT_THRESHOLD) {
    // Show rating if placed
    sprintf(BUFFER, "%0.1f", rating);
  } else {
    // Show remaining number of sets if not placed
    sprintf(BUFFER, "%d SETS REQUIRED", 5 - matchesPlayed);
  }

  Text_SetText(text, subtexts.rating, BUFFER);
  Text_SetColor(text, subtexts.rating, &GX_COLORS.LIGHT_GRAY);

  lastRatingTextLen = strlen(BUFFER);
}

void InitRankIcon(SlpCSSDesc* slpCss, SlippiRank rank) {
  gobj = GObj_Create(0x4, 0x5, 0x80);
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

void InitRankInfoText(SlippiRank rank, float rating, uint matchesPlayed, RankInfo_FetchStatus status) {
  // Create text object for rank info
  text = Text_CreateText(0, 0);
  text->kerning = 1;
  text->align = 0;
  text->use_aspect = 1;
  text->scale = (Vec2){0.01, 0.01};
  text->aspect.X *= 2.5;

  // Rank Label, i.e. "Rank"
  subtexts.rankLabel = Text_AddSubtext(text, -1110, 850, "Rank");
  Text_SetScale(text, subtexts.rankLabel, 4, 4);
  Text_SetColor(text, subtexts.rankLabel, &GX_COLORS.GRAY);

  // Rank Text, ex. "BRONZE 3", "GRANDMASTER" (can be "Error")
  subtexts.rank = Text_AddSubtext(text, -640, 1100, "");
  Text_SetScale(text, subtexts.rank, 4, 4);

  // Rating Text, ex. "2002.5", "994.3" (can be "Failed to get rank", "X SETS REQUIRED", and "????")
  subtexts.rating = Text_AddSubtext(text, -640, 1250, "");
  Text_SetScale(text, subtexts.rating, 3.5, 3.5);

  // Set the newly created texts to their appropriate values
  SetRankText(rank, rating, matchesPlayed, status);

  // Rank Loader, i.e. an oscillation of ".", "..", "...", or empty
  subtexts.loader = Text_AddSubtext(text, -640, 1224, "");
  Text_SetScale(text, subtexts.loader, 4.25, 4.25);
  Text_SetColor(text, subtexts.loader, &GX_COLORS.YELLOW);
}

void UpdateRankChangeAnim() {
  // Figure which animation frame we are on after surpassing ratingChangeLen (frame 0 starts at frame ratingChangeLen)
  if (ratingUpdateTimer >= ratingChangeLen) {
    const int rankAnimFrame = ratingUpdateTimer - ratingChangeLen;
    if (rankAnimFrame > 0 && rankAnimFrame < RANK_CHANGE_LEN) {
      JOBJ_ForEachAnim(rankIconJobj, 6, 0x20, AOBJ_ReqAnim, 1, (float)rankAnimFrame);  // HSD_TypeMask::JOBJ 0x20
      JOBJ_AnimAll(rankIconJobj);
    }
  }
}

void UpdateRankInfo() {
  // See where allowRankChanges is set for more info about when this happens.
  if (!allowRankChanges || !CHECK_FLAG(rankInfoResp.visibility, VISIBILITY_LOCAL)) { return; }

  // Make sure our rank info is up to date
  GetRankInfo();

  // If we have finished fetching data called during initialization
  if (rankInfoResp.status == RankInfo_FetchStatus_FETCHED) {
    // Only initialize the fetched status if we didn't previously process a fetched response status
    if (lastExecutedStatus != LAST_EXECUTED_STATUS_FETCHED) {
      // Determine the duration of the rating increase / decrease
      float change = rankInfoResp.ratingChange;
      if (ABSF(change) < LOW_RATING_THRESHOLD) {
        ratingChangeLen = RATING_CHANGE_LEN / 4;
      } else if (ABSF(change) < MED_RATING_THRESHOLD) {
        ratingChangeLen = RATING_CHANGE_LEN / 2;
      } else {
        ratingChangeLen = RATING_CHANGE_LEN;
      }

      SlippiCSSDataTable* dt = GetSlpCSSDT();

      // Here we load the "previous" values because we are about to transition from those to the current.
      uint matchesPlayed = rankInfoResp.ratingUpdateCount;
      SlippiRank prevRank = rankInfoResp.rank - rankInfoResp.rankChange;
      float prevRating = rankInfoResp.ratingOrdinal - rankInfoResp.ratingChange;
      SetRankIcon(prevRank);
      SetRankText(prevRank, prevRating, matchesPlayed, rankInfoResp.status);

      // Clear loader text
      Text_SetText(text, subtexts.loader, "");
    } else if (rankInfoResp.ratingUpdateCount >= PLACEMENT_THRESHOLD) {
      UpdateRatingChange();
      int rankChange = rankInfoResp.rankChange;
      if (rankChange != 0) {
        UpdateRankChangeAnim();
      }
    }

    // Allow other states to be initialized again
    lastExecutedStatus = LAST_EXECUTED_STATUS_FETCHED;
  } else if (rankInfoResp.status == RankInfo_FetchStatus_ERROR) {
    // Initialize error state
    if (lastExecutedStatus != LAST_EXECUTED_STATUS_ERROR) {
      // Clear loader
      Text_SetText(text, subtexts.loader, "");

      // Dislay rank fetch error
      Text_SetText(text, subtexts.rank, "Error");
      Text_SetColor(text, subtexts.rank, &GX_COLORS.WHITE);
      Text_SetText(text, subtexts.rating, "Failed to get rank");
      Text_SetColor(text, subtexts.rating, &GX_COLORS.RED);
    }

    // Prevent error logic from looping, allow rank initialization again
    lastExecutedStatus = LAST_EXECUTED_STATUS_ERROR;
  } else if (rankInfoResp.status == RankInfo_FetchStatus_FETCHING) {
    // Initialize fetching state
    if (lastExecutedStatus != LAST_EXECUTED_STATUS_FETCHING) {
      // Set the rank icon and text values based on what data we previously had. That means loading
      // the "current" values because we haven't loaded the new stuff yet.
      uint matchesPlayed = rankInfoResp.ratingUpdateCount;
      SetRankIcon(rankInfoResp.rank);
      SetRankText(rankInfoResp.rank, rankInfoResp.ratingOrdinal, matchesPlayed, rankInfoResp.status);

      // Set loader position
      float loaderPosX = -640.f + (lastRatingTextLen * 60.f);
      Text_SetPosition(text, subtexts.loader, loaderPosX, 1224);
    }

    // Update unreported loader
    if (loadTimer % 15 == 0) {
      switch (loaderCount) {
        case 0: {
          Text_SetText(text, subtexts.loader, ".");
          break;
        }
        case 1: {
          Text_SetText(text, subtexts.loader, "..");
          break;
        }
        case 2: {
          Text_SetText(text, subtexts.loader, "...");
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
  float completion = (float)ratingUpdateTimer / (float)ratingChangeLen;
  return (ratingOrdinal - ratingChange) + (ratingChange * completion);
}

int GetRatingChangeSFX(float ratingChange) {
  if (rankInfoResp.ratingChange > 0) {
    return TICK_UP;
  } else if (rankInfoResp.ratingChange < 0) {
    return TICK_DOWN;
  } else {
    return 0xD0;
  }
}

int GetRankChangeSFX() {
  if (rankInfoResp.rankChange > 0) {
    return RANK_UP_SMALL;
  } else if (rankInfoResp.rankChange < 0) {
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
        rankInfoResp.ratingOrdinal,
        rankInfoResp.ratingChange);

    sprintf(BUFFER, "%0.1f", displayRating);
    Text_SetText(text, subtexts.rating, BUFFER);

    // Play tick sound effect during rating change
    u8 tickVolume = rankInfoResp.ratingChange > 0 ? TICK_UP_VOL : TICK_DOWN_VOL;
    SFX_PlayRaw(
        GetRatingChangeSFX(rankInfoResp.ratingChange),
        tickVolume,
        64, 0, 0);
  } else if (ratingUpdateTimer == ratingChangeLen) {
    // Set rating text to exact amount to prevent interp inaccuracies
    sprintf(BUFFER, "%0.1f", rankInfoResp.ratingOrdinal);
    Text_SetText(text, subtexts.rating, BUFFER);
  }

  if (ratingUpdateTimer == ratingChangeLen + RANK_CHANGE_LEN) {
    // Play sound for rank up / down
    SFX_PlayRaw(GetRankChangeSFX(), 255, 64, 0, 0);
  }

  if (ratingUpdateTimer < ratingChangeLen + RANK_CHANGE_LEN + RATING_NOTIFICATION_LEN) {
    if (ratingUpdateTimer == ratingChangeLen + (RANK_CHANGE_LEN / 2) + 1) {
      // Handle rate-up/rate-down SFX and show rating change notification
      if (rankInfoResp.ratingChange != 0) {
        // Create rating change text
        char* signString = rankInfoResp.ratingChange > 0 ? "\x81\x7B" : "\x81\x7C";
        sprintf(BUFFER, "%s%0.1f", signString, ABSF(rankInfoResp.ratingChange));

        // Create subtext for rating change
        subtexts.ratingChange = Text_AddSubtext(text, -225, 1260, BUFFER);
        Text_SetScale(text, subtexts.ratingChange, 3.0, 3.0);

        // Determine text color
        if (rankInfoResp.ratingChange > 0) {
          Text_SetColor(text, subtexts.ratingChange, &GX_COLORS.GREEN);
          // Play sfx for end of counting
          SFX_PlayRaw(RATING_INCREASE, 255, 64, 0, 0);
        }
        if (rankInfoResp.ratingChange < 0) {
          Text_SetColor(text, subtexts.ratingChange, &GX_COLORS.RED);
          // Play sfx for end of counting
          SFX_PlayRaw(RATING_DECREASE, 255, 64, 0, 0);
        }
      } else {
        // In the case where there is no rating change, assume the result did not go through
        subtexts.ratingChange = Text_AddSubtext(text, -225, 1260, "result pending");
        Text_SetScale(text, subtexts.ratingChange, 3.0, 3.0);
        Text_SetColor(text, subtexts.ratingChange, &GX_COLORS.YELLOW);
      }

      // Set new rank icon and text
      if (rankInfoResp.rankChange != 0) {
        // Update rank icon
        SlippiRank newRank = rankInfoResp.rank;
        SetRankIcon(newRank);
        // Update rank text
        float newRating = rankInfoResp.ratingOrdinal;
        u8 matchesPlayed = rankInfoResp.ratingUpdateCount;
        SetRankText(newRank, newRating, matchesPlayed, RankInfo_FetchStatus_FETCHED);
      }
    }

    // Clear notification string after rating change
    if (ratingUpdateTimer == ratingChangeLen + RANK_CHANGE_LEN + RATING_NOTIFICATION_LEN - 1) {
      if (subtexts.ratingChange != 0) {
        Text_SetText(text, subtexts.ratingChange, "");
      }
    }
  }
  ratingUpdateTimer++;
}

#endif SLIPPI_CSS_RANK_INFO_C
