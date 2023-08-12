#ifndef SLIPPI_CSS_MAIN_C
#define SLIPPI_CSS_MAIN_C

#include "Main.h"

#include "../../Core/Notifications/Notifications.c"
#include "Chat/Chat.c"
#include "RankInfo/RankInfo.c"

void UpdateOnlineCSS() {
    if (!IsSlippiOnlineCSS()) return;
    //bp();

    // Do not handle notifications or chat input if on Name Entry Screen
    if (IsOnCSSNameEntryScreen()) return;

    ListenForNotifications();
    ListenForChatInput();
    UpdateRankInfo();
}

void InitOnlineCSS() {
    if (!IsSlippiOnlineCSS()) return;

    InitChatMessages();

    // Only request rank info if this is ranked
    u8 onlineMode = R13_U8(R13_OFFSET_ONLINE_MODE);
    if (onlineMode == 0) {
        InitRankInfo();
    }
}

#endif SLIPPI_CSS_MAIN_C
