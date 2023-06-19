#ifndef SLIPPI_CORE_NOTIFICATION_C
#define SLIPPI_CORE_NOTIFICATION_C

#include "Notifications.h"
#include "Chat/ChatNotifications.c"

GOBJ *_notificationsGOBJ = NULL;
int LastNotificationMessageID = -1; // Initial Notification Message ID to always add to the bottom of the list

void FreeNotifications(void *ptr) {
    _notificationsGOBJ = NULL;
    LastNotificationMessageID = -1;
}

void InitNotifications() {
    if (GOBJ_IsAlive(_notificationsGOBJ)) return;
    // OSReport("Initializing Notifications Listener.... 0x%x\n", _notificationsGOBJ);


    for (int i = 0; i < NOTIFICATION_MESSAGE_SET_LENGTH; i++)
        NotificationMessagesSet[i] = false;

    GOBJ *gobj = GObj_Create(0x4, 0x5, 0x80);
    _notificationsGOBJ = gobj;

    GObj_AddUserData(gobj, 0x4, FreeNotifications, NULL);
    GObj_AddGXLink(gobj, GXLink_Common, 1, 129);
    GObj_AddProc(gobj, UpdateNotifications, 0x4);
}

/**
 * This method handles a proc that checks for notifications coming in
 * to popup on the screen.
 */
void ListenForNotifications() {
    InitNotifications();
    ListenForChatNotifications();
}

void UpdateNotifications() {
    // TODO: should check some static address queue and Create a NotificationMessage
}


void CreateAndAddNotificationMessage(SlpCSSDesc *slpCss, NotificationMessage *message) {
    GOBJ *gobj = GObj_Create(0x4, 0x5, 0x80);
    JOBJ *jobj = JOBJ_LoadJoint(slpCss->chatMessage->jobj);

    int id;
    while (!CanAddNewMessage() || (id = GetNextNotificationMessageID()) < 0) {} // stall
    message->id = id;
    message->jobjSet = slpCss->chatMessage;


    if (isWidescreen()) jobj->trans.X = -14.0f;
    float yMargin = 3.2f;
    jobj->trans.Y = -(message->id * yMargin);

    JOBJ_AddSetAnim(jobj, slpCss->chatMessage, 0);
    JOBJ_ReqAnimAll(jobj, 0.0f);

    void *destructor = message->desctructorFunc ? message->desctructorFunc : DestroyNotificationMessage;

    GObj_AddUserData(gobj, 0x4, destructor, message);
    GObj_AddObject(gobj, 0x4, jobj);
    GObj_AddGXLink(gobj, GXLink_Common, 3, 129);
    GObj_AddProc(gobj, UpdateNotificationMessage, 0x4);
}


void UpdateNotificationMessage(GOBJ *gobj) {
    NotificationMessage *msg = gobj->userdata;
    JOBJ *jobj = (JOBJ *) gobj->hsd_object;
    int framesLeft = msg->framesLeft--;
    JOBJ_AnimAll(jobj);
    // OSReport("UpdateChatMessage ID: %i, FramesLeft: %i\n", msg->id, msg->framesLeft);

    // Skip update until timer runs out
    if (framesLeft > 0) return;

    switch (msg->state) {
        case SLP_NOT_STATE_STARTING:
            // Restart timer and move to next state
            msg->state = SLP_NOT_STATE_IDLE;
            msg->framesLeft = msg->animationFrames * 8.5;

            if (msg->text) {
                msg->text->hidden = false;
            }

            break;
        case SLP_NOT_STATE_IDLE:
            // Restart timer and move to next state
            msg->state = SLP_NOT_STATE_CLEANUP;
            msg->framesLeft = msg->animationFrames;

            // Destroy/Hide Text Here
            // OSReport("UpdateChatMessage.Idle: ptr: 0x%x", (void*)(msg->text+0x5c));
            if (msg->text) msg->text->hidden = true;

            // Animate chat message to hide
            JOBJ_AddSetAnim(jobj, msg->jobjSet, 1);
            JOBJ_ReqAnimAll(jobj, 0.0f);
            break;
        case SLP_NOT_STATE_CLEANUP:
            NotificationMessagesSet[msg->id] = false;

//		OSReport("Deleted Message with ID: %i\n", msg->id);
            GObj_Destroy(gobj);
            // If there's no message after this one, restart LastNotificationMessageID
            for (int i = 0; i < NOTIFICATION_MESSAGE_SET_LENGTH; i++) if (NotificationMessagesSet[i]) return;
            LastNotificationMessageID = -1;
            break;
        default:
            break;
    }

}

/**
 * Finds next available notification message id
 */
int GetNextNotificationMessageID() {
    int i = LastNotificationMessageID + 1;
    if (i < NOTIFICATION_MESSAGE_SET_LENGTH) {
        if (!NotificationMessagesSet[i]) {
            NotificationMessagesSet[i] = true;
//            OSReport("GetNextNotificationMessageID: %i\n", i);
            return LastNotificationMessageID = i;
        }
    } else {
        for (int i = 0; i < NOTIFICATION_MESSAGE_SET_LENGTH; i++) {
            if (!NotificationMessagesSet[i]) {
                NotificationMessagesSet[i] = true;
//                OSReport("GetNextNotificationMessageID: %i\n", i);
                return LastNotificationMessageID = i;
            }
        }
    }

    return -1;
}

/**
 * Checks if a new message can be added
 */
bool CanAddNewMessage() {
    int activeMessages = 0;
    for (int i = 0; i < NOTIFICATION_MESSAGE_SET_LENGTH; i++) {
        if (NotificationMessagesSet[i]) activeMessages++;
    }
    return activeMessages < NOTIFICATION_MESSAGE_SET_LENGTH;
}


#endif SLIPPI_CORE_NOTIFICATION_C