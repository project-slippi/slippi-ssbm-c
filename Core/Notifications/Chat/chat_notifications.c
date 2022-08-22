#ifndef SLIPPI_CORE_CHAT_NOTIFICATION_C
#define SLIPPI_CORE_CHAT_NOTIFICATION_C
#include "chat_notifications.h"
#include "text.c"

#include "../../../common.h"
#include "../notifications.c"

int ChatMessagesLocalCount = 0;
int ChatMessagesRemoteCount = 0;

GOBJ* _chatNotificationsGOBJ;

void FreeChatNotifications(void* ptr){
	_chatNotificationsGOBJ = NULL;
}

void ListenForChatNotifications(){
    if(GOBJ_IsAlive(_chatNotificationsGOBJ)) return;
	OSReport("ListenForChatNotifications\n");

	ChatMessagesLocalCount = 0;
	ChatMessagesRemoteCount = 0;

	GOBJ* gobj = GObj_Create(0x4, 0x5, 0x80);

    _chatNotificationsGOBJ = gobj;

	GObj_AddUserData(gobj, 0x4, FreeChatNotifications, NULL);
	GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
	GObj_AddProc(gobj, UpdateChatNotifications, 0x4);
}

void UpdateChatNotifications(){
	SlippiCSSDataTable* dt = GetSlpCSSDT();
	MatchStateResponseBuffer* msrb = dt->msrb;

	int messageId = 0;
	int playerIndex = msrb->chatMsgPlayerIndex;

	if(playerIndex >= 0){
	
		if(msrb->userChatMsgId){
			messageId = msrb->userChatMsgId;
		} else if(msrb->oppChatMsgId){
			messageId = msrb->oppChatMsgId;
		}
	}

	if(messageId <= 0) return;
	if(playerIndex < 0 || playerIndex > 3) return;
	
	int groupId = messageId >> 4; // 18 >> 4 == 1
	int finalMessageId = (groupId << 4) ^ messageId; // 18 XOR 10 == 8
	if(!IsValidChatGroupId(groupId) || !IsValidChatMessageId(finalMessageId))
	{
		OSReport("Invalid Chat Command: %i!\n", messageId);	
		return;
	}
	CreateAndAddChatMessage(dt->SlpCSSDatAddress, msrb, playerIndex, messageId);
}

void FreeChatMessage(void* ptr){
    if(!ptr) return;

    NotificationMessage* chatMessage = (NotificationMessage*)ptr;
    bool isLocalMessage = chatMessage->playerIndex == MSRB()->localPlayerIndex;

    if(isLocalMessage){
        ChatMessagesLocalCount--;
    } else {
        ChatMessagesRemoteCount--;
    }
    OSReport("Free -> Local: %i Remote: %i\n", ChatMessagesLocalCount, ChatMessagesRemoteCount);

    HSD_Free(ptr);
}

void CreateAndAddChatMessage(SlpCSSDesc* slpCss, MatchStateResponseBuffer* msrb, int playerIndex, int messageId){
	bool isLocalMessage = playerIndex == msrb->localPlayerIndex;
	// Prevent going over the limit
	if((ChatMessagesRemoteCount + ChatMessagesLocalCount) >= NOTIFICATION_MESSAGE_SET_LENGTH)
		return;

	NotificationMessage* chatMessage = CreateChatMessage(playerIndex, messageId);
	CreateAndAddNotificationMessage(slpCss, chatMessage);
//	chatMessage->text = CreateChatMessageText(chatMessage);
	chatMessage->text = CreateChatMessageText2(chatMessage);
	SFX_Play(CHAT_SOUND_NEW_MESSAGE);
	
	if(isLocalMessage){
		ChatMessagesLocalCount++;
	} else {
		ChatMessagesRemoteCount++;
	}
	OSReport("Local: %i Remote: %i\n", ChatMessagesLocalCount, ChatMessagesRemoteCount);
}

Text* CreateChatMessageText(NotificationMessage* msg){
	MatchStateResponseBuffer * msrb = MSRB();
	bool isLocalMessage = msg->playerIndex == msrb->localPlayerIndex;

	Text* text = Text_CreateTextWithGX(0, 0, 3, 129);
	text->kerning = 1;
	text->align = 0;
	text->trans.Z = 5.0f;

	float textScale = 0.1;
	text->scale.X = textScale;
	text->scale.Y = textScale; 

	text->hidden = true; // hide by default

	// Dolphin returns the group and message id joined together so we need to split them
	int groupId = msg->messageId >> 4;
	int messageId = (groupId << 4) ^ msg->messageId;
	char* playerName = isLocalMessage ? msrb->localName : msrb->p1Name+(msg->playerIndex*31);

	char* name = strcat(playerName,": "); 
	char* message = GetChatText(groupId, messageId, false);
	float xPos = isWidescreen() ? -446.0f : -296.0f;
	float yPos = -252.0f + ((msg->id)*32.0f);
	int colorIndex = msrb->localPlayerIndex+1;
	float scale = 0.4f;

	createSubtext(text, &MSG_COLORS[colorIndex], 0x0, 0, (char**){name}, scale, xPos, yPos, 0.0f, 0.0f);
	createSubtext(text, &MSG_COLORS[0], 0x0, 0, (char**){message}, scale, strlen(name)*6.8f + xPos, yPos, 0.0f, 0.0f);

	return text;
}

Text* CreateChatMessageText2(NotificationMessage* msg){
    // Hack the text alloc info to use a different gx
    stc_textcanvas_first[0]->gx_link = 3;
    stc_textcanvas_first[0]->gx_pri = 129;
    Text* text = createSlippiPremadeText(msg->playerIndex+1, msg->messageId, 2, 0, -29.5f, -23.25f+(msg->id*3.2f), 5.0f, 0.04f);
    stc_textcanvas_first[0]->gx_link = 1;
    stc_textcanvas_first[0]->gx_pri = 0x80;

    text->hidden = true; // hide by default

    return text;
}

/**
 * Max number of messages a user can send locally
 * Halves the amount of messages allowed if the player count > 2
 * @return
 */
int GetMaxAllowedLocalMessages(){
    int res = GetRemotePlayerCount() > 1 ? CHAT_MAX_PLAYER_MESSAGES / 2 : CHAT_MAX_PLAYER_MESSAGES;
    return res;
}

bool CanAddNewChatMessage(){
    return CanAddNewMessage() && ChatMessagesLocalCount < GetMaxAllowedLocalMessages();
}

bool IsValidChatGroupId(int groupId){
	switch (groupId)
	{
	case PAD_BUTTON_DPAD_UP:
	case PAD_BUTTON_DPAD_LEFT:
	case PAD_BUTTON_DPAD_RIGHT:
	case PAD_BUTTON_DPAD_DOWN:
		return true;
	default:
		return false;
	}

	return false;
};

bool IsValidChatMessageId(int messageId){
	// For now use same logic
	return IsValidChatGroupId(messageId);
}

#endif SLIPPI_CORE_CHAT_NOTIFICATION_C