#ifndef SLIPPI_CSS_CHAT_TEXT_C
#define SLIPPI_CSS_CHAT_TEXT_C

#include "../../../Slippi.h"
#include "Text.h"
#include "../../Scenes/CSS/Chat/Chat.h"
#include "../../../Common.h"

int GetGroupIndex(int groupId) {
    int groupIndex = 0;

    switch (groupId) {
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

    return groupIndex;
}

char *GetHeaderText(int groupId) {
    int groupIndex = GetGroupIndex(groupId);
    return HEADER_STRINGS[groupIndex];
}

char *GetChatText(int groupId, int messageId, int playerIdx, bool useMessageIndex) {
	// OSReport("GetChatText groupId: %i, msgId: %i \n", groupId, messageId);

    int groupIndex = GetGroupIndex(groupId);
    int index = 0;

    if (useMessageIndex) {
        index = messageId;
    } else {
        switch (messageId) {
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
    return playerSettingsResp->settings[playerIdx].chatMessages[(groupIndex * 4) + (index - 1)];
};

Text *CreateChatWindowText(GOBJ *gobj, int groupId) {
    MatchStateResponseBuffer *msrb = MSRB();
    // OSReport("Idx: %d\n", msrb->localPlayerIndex);

    Text *text = Text_CreateText(0, 0);
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
    sprintf(title, "%s", GetHeaderText(groupId));
    // Create  Header
    int titleColor = isConnected() ? MSG_COLOR_CHAT_WINDOW : MSG_COLOR_CHAT_WINDOW_IDLE;
    CreateSubtext(text, &MSG_COLORS[titleColor], false, 0, (char **) {title}, 0.45f, x + offset, 79.0f, 0.0f, 0.0f);

    // Create Labels
    for (int i = CHAT_STR_UP; i <= CHAT_STR_DOWN; i++) {
        float margin = 25.0f * (i + 1); // starts with 2 lines from header
        float yPos = 79.0f + margin;
        char *label = GetChatText(groupId, i, msrb->localPlayerIndex, true);

        CreateSubtext(text, &MSG_COLORS[MSG_COLOR_WHITE], false, 0, (char **) {label}, 0.45f, labelX + offset, yPos, 0.0f,
                      0.0f);
    }

    return text;
}

#endif SLIPPI_CSS_CHAT_TEXT_C
