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
}

void InitOnlineCSS() {
    if (!IsSlippiOnlineCSS()) return;

    InitChatMessages();
}

#endif SLIPPI_CSS_MAIN_C
