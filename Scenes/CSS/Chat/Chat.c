#ifndef SLIPPI_CSS_CHAT_C
#define SLIPPI_CSS_CHAT_C

#include "chat.h"
#include "../../common.h"

#include "../../Core/Notifications/notifications.c"
#include "../../Core/Notifications/Chat/chat_notifications.h"
#include "../../Core/Notifications/Chat/text.c"

#define CHAT_WINDOW_FRAMES 60*2.5           // 4 seconds at 60fps
#define CHAT_ALLOW_COMMAND_FRAMES 60*0.2    // 1.5 seconds at 60fps

GOBJ *_chatMainGOBJ = NULL;
GOBJ *_chatWindowGOBJ = NULL;


void FreeChat(void *ptr) {
    _chatMainGOBJ = NULL;
    if (ptr) HSD_Free(ptr);
}

void FreeChatWindow(void *ptr) {
    _chatWindowGOBJ = NULL;
    if (ptr) HSD_Free(ptr);
}

/**
 * This method handles a proc that checks for player input 
 * on the SLIPPI online CSS to:
 * 1 - Handle Chat Window
 * 2 - Send Chat Command to Dolphin
 */
void ListenForChatInput() {
    // OSReport("ListenForChatInput 0x%x 0x%x 0x%x 0x%x 0x%x\n",_chatMainGOBJ, _chatMainGOBJ->previous, _chatMainGOBJ->next, _chatMainGOBJ->previousOrdered, _chatMainGOBJ->nextOrdered);
    if (GOBJ_IsAlive(_chatMainGOBJ)) return;
    OSReport("Initializing Chat Input Listener.... 0x%x\n", _chatMainGOBJ);
    GOBJ *gobj = GObj_Create(0x4, 0x5, 0x80);

    _chatMainGOBJ = gobj;

    GObj_AddUserData(gobj, 0x4, FreeChat, NULL);
    GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
    GObj_AddProc(gobj, UpdateChat, 0x4);
}

/**
 * Checks for input and handles chat window
 */
void UpdateChat() {
    // if window is already created return
    if (GOBJ_IsAlive(_chatWindowGOBJ)) return;

    ChatInput *chatInput = PadGetChatInput(false);
    if (chatInput->input <= 0) return; // Return if not input

    // TODO: Move this back
     SlpCSSDesc* slpCss = GetSlpCSSDT()->SlpCSSDatAddress;// Archive_GetPublicAddress(archive, "slpCSS");
//    HSD_Archive *archive = Archive_LoadFile("slpCSS.dat");
//    SlpCSSDesc *slpCss = Archive_GetPublicAddress(archive, "slpCSS");
    ChatWindowData *data = calloc(sizeof(ChatWindowData));

    // create chat window with required group id
    data->groupId = chatInput->input;
    data->slpCss = slpCss;
    data->framesLeft = CHAT_WINDOW_FRAMES;
    data->framesCounter = CHAT_ALLOW_COMMAND_FRAMES; // Allow sending right away

    //OSReport("data->groupID = %i\n", data->groupId);

    GOBJ *gobj = _chatWindowGOBJ = GObj_Create(0x4, 0x5, 0x80);
    JOBJ *jobj = JOBJ_LoadJoint(slpCss->chatWindow->jobj);

    jobj->trans.X = isWidescreen() ? -35.0f : -20.0f;
    jobj->trans.Y = -16.5f;

    GObj_AddObject(gobj, 0x4, jobj);
    GObj_AddUserData(gobj, 4, FreeChatWindow, data);
    GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
    GObj_AddProc(gobj, UpdateChatWindow, 0x4);

    SFX_PlayCommon(CHAT_SOUND_OPEN_WINDOW);
    GetSlpCSSDT()->chatWindowOpened = true;
}

/**
 * Proc for Chat Window that handles inputs and commands
 * 1 - Closes Window after X time has passed without input
 * 2 - Checks for input to Send Chat Command to Dolphin
 * 3 - Checks for input to Close Window
 */
void UpdateChatWindow(GOBJ *gobj) {
    // OSReport("UpdateChatWindow\n");
    ChatWindowData *data = gobj->userdata;
    JOBJ *jobj = (JOBJ *) gobj->hsd_object;

    data->framesCounter++; // Increase Frames Counter

    if (!data->text) {
        gobj->gx_pri = 128; // Hack to allow thing to be shown
        data->text = CreateChatWindowText(gobj, data->groupId);
        return;
    }

    // decrease frames
    data->framesLeft--;

    // Check inputs with window commands
    ChatInput *chatInput = PadGetChatInput(true);

    // Close Chat Window if B button pressed
    if (chatInput->input == PAD_BUTTON_B) {
        SFX_PlayCommon(CHAT_SOUND_CANCEL_MESSAGE);
        CloseChatWindow(jobj, data);
        return;
    }

    if (chatInput->input > 0) {
        // Should close and send chat message
        if (data->framesCounter < CHAT_ALLOW_COMMAND_FRAMES) {
            return;
        } else if (CanAddNewChatMessage()) {
            data->framesCounter = 0; // Reset frames counter since last message sent
            int chatCommand = (data->groupId << 4) + chatInput->input;
//            OSReport("chatInput->input: %i, a: 0x%x\n", chatInput->input, chatCommand);
            SendOutgoingChatCommand(chatCommand);
            CloseChatWindow(jobj, data);
            return;
        } else {
            SFX_PlayCommon(CHAT_SOUND_BLOCK_MESSAGE);
            return;
        }
    }


    if (data->framesLeft <= 0) {
        CloseChatWindow(jobj, data);
    }

}

/**
 * Closes and destroy the chat window object
 */
void CloseChatWindow(JOBJ *jobj, ChatWindowData *data) {
    JOBJ_SetFlagsAll(jobj, JOBJ_HIDDEN);
    Text *t = data->text;
    Text_Destroy(t);
    data->text = NULL;

    GObj_Destroy(_chatWindowGOBJ);
    GObj_RemoveProc(_chatWindowGOBJ);
    GetSlpCSSDT()->chatWindowOpened = false;
    _chatWindowGOBJ = NULL;
}

/**
 * Checks if any player has pressed a button that toggles
 * the chat window or a chat command
 */
ChatInput *PadGetChatInput(bool checkForCommands) {
    ChatInput *input = calloc(sizeof(ChatInput));
    input->input = -1;
    input->playerIndex = -1;

    //  inputs to be checked normally
    int normalInputs[4] = {
            PAD_BUTTON_DPAD_UP,
            PAD_BUTTON_DPAD_LEFT,
            PAD_BUTTON_DPAD_RIGHT,
            PAD_BUTTON_DPAD_DOWN,
    };

    // Inputs to be check additionally when window is open
    int *windowCommands[1] = {
            PAD_BUTTON_B,
    };

    // Check each player if matches any of the allowed commands
    // if found, return the ChatInput object with info on who
    // pressed the button 
    for (int playerIndex = 0; playerIndex < 4; playerIndex++) {
        HSD_Pad *pad = PadGet(playerIndex, PADGET_ENGINE);

        for (int i = 0; i < sizeof(normalInputs) / sizeof(int); i++) {
            int inputToCheck = normalInputs[i];
            if (pad->down & inputToCheck) {
                //OSReport("PadGetChatInput %i\n", inputToCheck);
                input->input = inputToCheck;
                input->playerIndex = playerIndex;
                return input;
            }
        }

        // skip to next iteration if we should not check for window commands
        if (!checkForCommands) continue;

        for (int i = 0; i < sizeof(windowCommands) / sizeof(int); i++) {
            int inputToCheck = windowCommands[i];
            if (pad->down & inputToCheck) {
                //OSReport("PadGetChatInput %i\n", inputToCheck);
                input->input = inputToCheck;
                input->playerIndex = playerIndex;
                return input;
            }
        }

    }
    return input;
}

/**
 * Sends EXI Command to Dolphin for a new chat message
 */
void SendOutgoingChatCommand(int messageId) {
    OutgoingChatMessageBuffer *buffer = HSD_MemAlloc(sizeof(OutgoingChatMessageBuffer));
    buffer->cmd = SLIPPI_CMD_SendChatMessage;
    buffer->messageId = messageId;
//    OSReport("SendOutgoingChatCommand buffer cmd:%i, msgId:%i size: %i\n", buffer->cmd, buffer->messageId, sizeof(OutgoingChatMessageBuffer));
    EXITransferBuffer(buffer, sizeof(OutgoingChatMessageBuffer), EXI_TX_WRITE);
}

#endif SLIPPI_CSS_CHAT_C