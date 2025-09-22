#ifndef SLIPPI_CSS_RANK_INFO_H
#define SLIPPI_CSS_RANK_INFO_H

#include "../../../Slippi.h"
#include "../../../Common.h"
#include "../../../ExiSlippi.h"

// RATING THRESHOLDS
#define HIGH_RATING_THRESHOLD 35.f
#define MED_RATING_THRESHOLD 20.f
#define LOW_RATING_THRESHOLD 10.f
#define PLACEMENT_THRESHOLD 5

// SFX
#define RANK_UP_SMALL 0xAA
#define RANK_UP_BIG 0xAB
#define RANK_DOWN_SMALL 0xBE
#define RANK_DOWN_BIG 0xBF
#define RATING_INCREASE 0x69
#define RATING_DECREASE 0x19E
#define TICK_UP_VOL 150
#define TICK_DOWN_VOL 100
#define TICK_UP 0xBB
#define TICK_DOWN 0xEA

// ANIMATION
#define RATING_CHANGE_LEN 140
#define RANK_CHANGE_LEN 10
#define RATING_NOTIFICATION_LEN 200
#define NARRATOR_LEN 140 /* 2 seconds of 'Choose your character!' */

// RANK FETCH DURATIONS
#define RETRY_FETCH_0_LEN 120
#define RETRY_FETCH_1_LEN 420

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

typedef struct GXColors {
  GXColor WHITE;
  GXColor GRAY;
  GXColor RED;
  GXColor YELLOW;
  GXColor GREEN;
  GXColor LIGHT_GRAY;
} GXColors;

typedef struct SubtextIDs {
  int loader;
  int ratingChange;
  int rank;
  int rating;
  int rankLabel;
} SubtextIDs;

enum RankVisibility {
  VISIBILITY_LOCAL,
  VISIBILITY_OPPONENT
};

typedef enum SlippiRank {
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
} SlippiRank;

enum LastExecutedStatus {
  LAST_EXECUTED_STATUS_NONE,
  LAST_EXECUTED_STATUS_FETCHING,
  LAST_EXECUTED_STATUS_FETCHED,
  LAST_EXECUTED_STATUS_ERROR
};

void InitRankInfoText(SlippiRank rank, float rating, uint matches_played, RankInfo_FetchStatus status);
void InitRankIcon(SlpCSSDesc *slpCss, SlippiRank rank);
void UpdateRatingChange();
void UpdateRankInfo();

#endif SLIPPI_CSS_RANK_INFO_H
