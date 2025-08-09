#ifndef SLIPPI_CSS_MAIN_C
#define SLIPPI_CSS_MAIN_C

#include "Main.h"

#include "../../Core/Notifications/Notifications.c"
#include "Chat/Chat.c"
#include "SheikSelector.c"

void UpdateOnlineCSS() {
  if (!IsSlippiOnlineCSS())
    return;
  // bp();

  // Do not handle notifications or chat input if on Name Entry Screen
  if (IsOnCSSNameEntryScreen())
    return;

  ListenForNotifications();
  ListenForChatInput();
  UpdateSheikSelector();
}

void InitOnlineCSS() {
  if (!IsSlippiOnlineCSS())
    return;

  InitChatMessages();
  InitSheikSelector();
}

#endif SLIPPI_CSS_MAIN_C
