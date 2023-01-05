#ifndef SLIPPI_CSS_RANK_INFO_C
#define SLIPPI_CSS_RANK_INFO_C

#include "RankInfo.h"
#include "../../../Common.h"
#include "../../../Slippi.h"
#include "../../../Files.h"

void InitRankInfo() {
    // Send dolphin command to pull rank data
    SendGetRankInfoCommand();

    // Receieve rank data from dolphin
    RankInfoResponseBuffer *rirb = ReceiveRankInfo();
    // Create text and write rank info
    InitRankInfoText(rirb);

    SlippiCSSDataTable *dt = GetSlpCSSDT();
    u8 rank = rirb->localPlayerRank;

    // Select the correct icon for player
    InitRankIcon(dt->SlpCSSDatAddress, rank);
}

void InitRankIcon(SlpCSSDesc *slpCss, u8 rank) {
    GOBJ *gobj = GObj_Create(0x4, 0x5, 0x80);
    JOBJ *jobj = JOBJ_LoadJoint(slpCss->rankIcons->jobj);

    jobj->trans.X = -7.3f;
    jobj->trans.Y = -10.5f;

    JOBJ_AddSetAnim(jobj, slpCss->rankIcons, 0);
    // Set rank icon
    JOBJ_ReqAnimAll(jobj, (u32) rank);
    JOBJ_AnimAll(jobj);

    GObj_AddObject(gobj, 0x4, jobj);
    GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
}

void InitRankInfoText(RankInfoResponseBuffer *rirb) {
    // Convert rating ordinal float to string
    char* ratingString[20];
    sprintf(ratingString, "%0.1f", rirb->localPlayerRating);

    Text *text = Text_CreateText(0, 0);
    text->kerning = 1;
    text->align = 0;
    text->use_aspect = 1;
    text->scale = (Vec2){0.01, 0.01};
    text->aspect.X *= 2.5;
    int rankSubtextId = Text_AddSubtext(text, -1100, 1540, RANK_STRINGS[rirb->localPlayerRank]);
    Text_SetScale(text, rankSubtextId, 5, 5);
    int ratingSubtextId = Text_AddSubtext(text, -1100, 1740, ratingString);
    Text_SetScale(text, ratingSubtextId, 4, 4);
    GXColor col = (GXColor){255, 255, 255, 155};
    Text_SetColor(text, rankSubtextId, &col);
}

/**
 * Sends EXI Command to Dolphin for getting rank info
 */
void SendGetRankInfoCommand() {
    GetRankInfoBuffer *buffer = HSD_MemAlloc(sizeof(GetRankInfoBuffer));
    buffer->cmd = SLIPPI_CMD_GetRankInfo;
//    OSReport("SendOutgoingChatCommand buffer cmd:%i, msgId:%i size: %i\n", buffer->cmd, buffer->messageId, sizeof(OutgoingChatMessageBuffer));
    EXITransferBuffer(buffer, sizeof(GetRankInfoBuffer), EXI_TX_WRITE);
}

RankInfoResponseBuffer* ReceiveRankInfo() {
    RankInfoResponseBuffer *buffer = HSD_MemAlloc(sizeof(RankInfoResponseBuffer));
    EXITransferBuffer(buffer, sizeof(RankInfoResponseBuffer), EXI_TX_READ);
    return buffer;
}

#endif SLIPPI_CSS_RANK_INFO_C