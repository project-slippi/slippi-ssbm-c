#ifndef SLIPPI_CSS_MAIN_C
#define SLIPPI_CSS_MAIN_C

#include "Main.h"

#include "../../Core/Notifications/Notifications.c"
#include "Chat/Chat.c"

void UpdateOnlineCSS() {
    if (!IsSlippiOnlineCSS()) return;
    //bp();

    ListenForNotifications();
    ListenForChatInput();
}

#endif SLIPPI_CSS_MAIN_C
