#ifndef SLIPPI_CSS_RANK_INFO_H
#define SLIPPI_CSS_RANK_INFO_H

typedef struct packed(GetRankInfoBuffer) {
    u8 cmd;            // SLIPPI EXI COMMAND
} GetRankInfoBuffer;

enum RANKS {
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
};

static char* RANK_STRINGS[] = {
    "UNRANKED",
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

void InitRankInfoText(RankInfoResponseBuffer *rirb);
void InitRankIcon(SlpCSSDesc *slpCss, u8 rank);
void SendGetRankInfoCommand();
RankInfoResponseBuffer* ReceiveRankInfo();

#endif SLIPPI_CSS_RANK_INFO_H

