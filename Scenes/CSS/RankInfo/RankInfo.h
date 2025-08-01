#ifndef SLIPPI_CSS_RANK_INFO_H
#define SLIPPI_CSS_RANK_INFO_H

#include "../../../Common.h"
#include "../../../ExiSlippi.h"

typedef struct RankInfo {
  u8 visibility;
  u8 status;
  u8 rank;
  float ratingOrdinal;
  u8 global;
  u8 regional;
  u32 ratingUpdateCount;
  float ratingChange;
  s8 rankChange;
} RankInfo;

enum RankVisibility {
  VISIBILITY_LOCAL,
  VISIBILITY_OPPONENT
};

enum SlippiRank {
  RANK_UNRANKED,
  RANK_BRONZE_1,
  RANK_BRONZE_2,
  RANK_BRONZE_3,
  RANK_SILVER_1,
  RANK_SILVER_2,
  RANK_SILVER_3,
  RANK_GOLD_1,
  RANK_GOLD_2,
  RANK_GOLD_3,
  RANK_PLATINUM_1,
  RANK_PLATINUM_2,
  RANK_PLATINUM_3,
  RANK_DIAMOND_1,
  RANK_DIAMOND_2,
  RANK_DIAMOND_3,
  RANK_MASTER_1,
  RANK_MASTER_2,
  RANK_MASTER_3,
  RANK_GRANDMASTER,
  RANK_COUNT
};

static char *RANK_STRINGS[] = {
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

Text *text;
int loaderSubtextId;
int changeSignSubtextId;
int ratingChangeSubtextId;
int rankSubtextId;
int ratingSubtextId;

bool rankInitialized = false;

const float HIGH_RATING_THRESHOLD = 35.f;
const float MED_RATING_THRESHOLD = 20.f;
const float LOW_RATING_THRESHOLD = 10.f;
const u8 PLACEMENT_THRESHOLD = 5;

// SFX
const int RANK_UP_SMALL = 0xAA;
const int RANK_UP_BIG = 0xAB;
const int RANK_DOWN_SMALL = 0xBE;
const int RANK_DOWN_BIG = 0xBF;

const int RATING_INCREASE = 0x69;
const int RATING_DECREASE = 0x19E;

const int TICK_UP_VOL = 150;
const int TICK_DOWN_VOL = 100;
const int TICK_UP = 0xBB;
const int TICK_DOWN = 0xEA;

// ANIMATION
const int RATING_CHANGE_LEN = 140;
const int RANK_CHANGE_LEN = 10;
const int RATING_NOTIFICATION_LEN = 200;
const int NARRATOR_LEN = 140;  // 2 seconds of 'Choose your character!'

// RANK FETCH DURATIONS
const int RETRY_FETCH_0_LEN = 120;
const int RETRY_FETCH_1_LEN = 420;

void InitRankInfoText(u8 rank, float rating, uint matches_played, RankInfo_FetchStatus status);
void InitRankIcon(SlpCSSDesc *slpCss, u8 rank);
void UpdateRatingChange();
void UpdateRankInfo();

#endif SLIPPI_CSS_RANK_INFO_H
