#ifndef SLIPPI_CORE_CHAT_NOTIFICATION_H
#define SLIPPI_CORE_CHAT_NOTIFICATION_H

#include "../../../Slippi.h"
#include "../Notifications.h"
#include "../../../Game/SysText.c"

#define CHAT_SOUND_NEW_MESSAGE 0xb7        // Global Sound ID
#define CHAT_SOUND_BLOCK_MESSAGE 0x3    // Common Sound
#define CHAT_SOUND_OPEN_WINDOW 0x2        // Common Sound
#define CHAT_SOUND_CANCEL_MESSAGE 0x0   // Common Sound

#define CHAT_FRAMES 20                    // Frames that a chat notification message lives for
#define CHAT_MAX_PLAYER_MESSAGES 4        // Max Messagees allowed per player

enum ChatMessageGenTypes {
 CHAT_MESSAGE_GEN_TYPE_SUBTEXT,
 CHAT_MESSAGE_GEN_TYPE_SYSTEXT,
 CHAT_MESSAGE_GEN_TYPE_EXI,
};

enum ChatMessageSpecialIDs{
    CHAT_MESSAGE_ID_DISABLED = 0x10,
};

#ifndef CHAT_MESSAGE_GEN_TYPE
#define CHAT_MESSAGE_GEN_TYPE CHAT_MESSAGE_GEN_TYPE_SYSTEXT
#endif

/** functions **/
void ListenForChatNotifications();

void UpdateChatNotifications();

void FreeChatNotifications(void *ptr);

void CreateAndAddChatMessage(SlpCSSDesc *slpCss, MatchStateResponseBuffer *msrb, int playerIndex, int messageId);

void UpdateChatMessage(GOBJ *gobj);

Text *CreateChatMessageText(NotificationMessage *msg);

Text *CreateChatMessageTextFromSubText(NotificationMessage *msg);

Text *CreateChatMessageTextFromLocalSysText(NotificationMessage *msg);

Text *CreateChatMessageTextFromEXIDevice(NotificationMessage *msg);

SysText* BuildChatTextData(char* playerName, u8 playerIndex, u8 groupId, u8 messageId);

bool IsValidChatGroupId(int groupId);

bool IsSpecialChatMessageId(int messageId);

bool IsValidChatMessageId(int messageId);

void FreeChatMessage(void *ptr);

bool CanAddNewChatMessage();

int GetMaxAllowedLocalMessages();


NotificationMessage *CreateChatMessage(int playerIndex, int messageId) {
    NotificationMessage *msg = calloc(sizeof(NotificationMessage));
    msg->type = SLP_NOT_CHAT;
    msg->state = SLP_NOT_STATE_STARTING;
    msg->framesLeft = CHAT_FRAMES;
    msg->animationFrames = CHAT_FRAMES;
    msg->playerIndex = playerIndex;
    msg->messageId = messageId;
    msg->desctructorFunc = FreeChatMessage;

    return msg;
}

#endif SLIPPI_CORE_CHAT_NOTIFICATION_H