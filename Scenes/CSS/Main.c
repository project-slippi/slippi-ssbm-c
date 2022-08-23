#ifndef SLIPPI_CSS_MAIN_C
#define SLIPPI_CSS_MAIN_C
#include "main.h"

#include "../Core/Notifications/notifications.c"
#include "Chat/chat.c"

void UpdateOnlineCSS(){
    if(!IsSlippiOnlineCSS()) return;
    //bp();

    ListenForNotifications();
    ListenForChatInput();
}

#endif SLIPPI_CSS_MAIN_C
