#ifndef SLIPPI_CSS_CHAT_TEXT_C
#define SLIPPI_CSS_CHAT_TEXT_C
#include "../../../Slippi.h"
#include "Text.h"

char* GetChatText(int groupId, int messageId, bool useMessageIndex){
//	OSReport("GetChatText groupId: %i, msgId: %i \n", groupId, messageId);

	int groupIndex = 0;
	int index = 0;

	switch (groupId)
	{
	case PAD_BUTTON_DPAD_UP:
		groupIndex = 0;
		break;
	case PAD_BUTTON_DPAD_LEFT:
		groupIndex = 1;
		break;
	case PAD_BUTTON_DPAD_RIGHT:
		groupIndex = 2;
		break;
	case PAD_BUTTON_DPAD_DOWN:
		groupIndex = 3;
		break;
	default:
    	groupIndex = -1;
		break;
	}


	if(useMessageIndex) 
	{
		index = messageId;
	}
	else {
		switch (messageId)
		{
		case PAD_BUTTON_DPAD_UP:
			index = 1;
			break;
		case PAD_BUTTON_DPAD_LEFT:
			index = 2;
			break;
		case PAD_BUTTON_DPAD_RIGHT:
			index = 3;
			break;
		case PAD_BUTTON_DPAD_DOWN:
			index = 4;
			break;
		default:
			index = messageId;
			break;
		}
	}


//	OSReport("GetChatText s: %s\n", ChatGroups[groupIndex][index]);
    return ChatGroups[groupIndex][index];
};

Text* CreateChatWindowText(GOBJ* gobj, int groupId){
	Text* text = Text_CreateText(0, 0);
	// OSReport("text.gxLink: %i gxPri: %i", text->gobj->gx_link, text->gobj->gx_pri);

	text->kerning = 1;
	text->align = 0;
	text->trans.Z = 0.0f;
	text->scale.X = 0.1f;
	text->scale.Y = 0.1f; 

	float x = isWidescreen() ? -452.0f : -300.0f;
	float labelX = x + 15.0f;

	float offset = 0; // should change if widescreen
	
	char title[30];
	sprintf(title, "Chat: %s", GetChatText(groupId, CHAT_STR_HEADER, true));
	// Create  Header
	int titleColor = isConnected() ? MSG_COLOR_CHAT_WINDOW :  MSG_COLOR_CHAT_WINDOW_IDLE;
	createSubtext(text, &MSG_COLORS[titleColor], 0x0, 0, (char**){title}, 0.45f, x+offset, 79.0f, 0.0f, 0.0f);

	// Create Labels
	for(int i=CHAT_STR_UP;i<=CHAT_STR_DOWN;i++){
	    float margin = 25.0f*(i+1); // starts with 2 lines from header
	    float yPos = 79.0f+margin;
		char* label = GetChatText(groupId, i, true);

        //TODO: for some reason if I don't print the yPosition, it gets reset to 0 for all labels except index 0....
        //If you don't believe me, try commenting the line below
        OSReport("yPos: %f\n", yPos);
        createSubtext(text, &MSG_COLORS[MSG_COLOR_WHITE], 0x0, 0, (char**){label}, 0.45f, labelX+offset, yPos, 0.0f, 0.0f);
	}

	return text;
}

#endif SLIPPI_CSS_CHAT_TEXT_C
