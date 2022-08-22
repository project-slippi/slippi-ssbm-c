#ifndef SLIPPI_CORE_CHAT_NOTIFICATION_H
#define SLIPPI_CORE_CHAT_NOTIFICATION_H
#include "../../../slippi.h"
#include "../notifications.h"

#define CHAT_SOUND_NEW_MESSAGE 0xb7		// Global Sound ID
#define CHAT_SOUND_BLOCK_MESSAGE 0x3 	// Common Sound
#define CHAT_SOUND_OPEN_WINDOW 0x2 		// Common Sound
#define CHAT_SOUND_CANCEL_MESSAGE 0x0   // Common Sound

#define CHAT_FRAMES 20 					// Frames that a chat notification message lives for
#define CHAT_MAX_PLAYER_MESSAGES 4		// Max Messagees allowed per player


/** functions **/
void ListenForChatNotifications();
void UpdateChatNotifications();
void FreeChatNotifications(void* ptr);
void CreateAndAddChatMessage(SlpCSSDesc* slpCss, MatchStateResponseBuffer* msrb, int playerIndex, int messageId);
void UpdateChatMessage(GOBJ* gobj);
Text* CreateChatMessageText(NotificationMessage* msg);
Text* CreateChatMessageText2(NotificationMessage* msg);
bool IsValidChatGroupId(int groupId);
bool IsValidChatMessageId(int messageId);
void FreeChatMessage(void* ptr);
bool CanAddNewChatMessage();
int GetMaxAllowedLocalMessages();


NotificationMessage* CreateChatMessage(int playerIndex, int messageId){
	NotificationMessage* msg = calloc(sizeof(NotificationMessage));
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