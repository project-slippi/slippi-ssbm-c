#ifndef SLIPPI_CSS_CHAT_H
#define SLIPPI_CSS_CHAT_H
#include "../../Core/Notifications/Chat/text.h"

#define CHAT_SOUND_NEW_MESSAGE 0xb7		// Global Sound ID
#define CHAT_SOUND_BLOCK_MESSAGE 0x3 	// Common Sound
#define CHAT_SOUND_OPEN_WINDOW 0x2 		// Common Sound
#define CHAT_SOUND_CANCEL_MESSAGE 0x0   // Common Sound

typedef struct packed(OutgoingChatMessageBuffer) {
	u8 cmd;			// SLIPPI EXI COMMAND
	u8 messageId;	// 0xYZ (Y = group id, Z=message id)
} OutgoingChatMessageBuffer;

typedef struct ChatWindowData {
	Text* text;		    // Text Struct to show headers and labels
	SlpCSSDesc* slpCss; // Slp CSS to pass around
	int groupId;	    // Chat Group ID (Misc, Compliments), this holds the input value
	int framesLeft;	    // FramesLeft before closing the window for inactivity
	int framesCounter;  // Frames Counter used to allow sending another chat command
} ChatWindowData;

typedef struct ChatInput {
	int playerIndex;
    int input;
} ChatInput;

/** functions **/
void FreeChat(void* ptr);
void FreeChatWindow(void* ptr);
void ListenForChatInput();
void UpdateChat();
void UpdateChatWindow(GOBJ* gobj);
void CloseChatWindow(JOBJ* jobj, ChatWindowData* data);
ChatInput* PadGetChatInput(bool checkForCommands);

#endif SLIPPI_CSS_CHAT_H
