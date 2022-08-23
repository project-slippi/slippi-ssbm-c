#ifndef SLIPPI_CORE_NOTIFICATION_H
#define SLIPPI_CORE_NOTIFICATION_H
#include "../../slippi.h"

#define DEFAULT_ANIM_FRAMES 20 									// Frames that notification message animations live for 
#define NOTIFICATION_MESSAGE_SET_LENGTH 10						// Max Messages allowed to be shown
int NotificationMessagesSet[NOTIFICATION_MESSAGE_SET_LENGTH]; 	// Max mesages allowed

typedef enum SlippiNotificationType {
    SLP_NOT_NORMAL,
    SLP_NOT_CHAT,
    SLP_NOT_SYSTEM,
    SLP_NOT_ALERT,
    SLP_NOT_WARN,
    SLP_NOT_ERROR,
    SLP_NOT_INFO,
} SlippiNotificationType;

typedef enum SlippiNotificationState
{
	SLP_NOT_STATE_NONE,
    SLP_NOT_STATE_STARTING,	        // Message is animating to show
    SLP_NOT_STATE_IDLE,		        // Message finished animating and is just idle
    SLP_NOT_STATE_CLEANUP,		    // MEssage is animating to hide and exit
} SlippiNotificationState;

typedef struct NotificationMessage {
	JOBJSet* jobjSet;			
    SlippiNotificationType type;    // Slippi Notification Type			
	SlippiNotificationState state; 	// Initial message state
	Text* text;					    // Created Text Object
	void* desctructorFunc;          // Function used to destroy this object (Default to HSD_Free)
	int id; 					    // ID of this message
	int animationFrames; 			// Amount of frames the message is displayed for 
	int framesLeft; 			    // Amount of frames the message is displayed for (counter)
	int playerIndex;			    // Player Index
	int messageId;				    // Message Id
} NotificationMessage;


NotificationMessage* CreateNotificationMessage(int messageId, SlippiNotificationType type){
	NotificationMessage* msg = calloc(sizeof(NotificationMessage));
	msg->type = type;
	msg->state = SLP_NOT_STATE_STARTING;
	msg->framesLeft = DEFAULT_ANIM_FRAMES;
	msg->animationFrames = DEFAULT_ANIM_FRAMES;
	msg->messageId = messageId;
	msg->desctructorFunc = HSD_Free;
	return msg;
}

/** Functions **/
void ListenForNotifications();
void UpdateNotifications();
int GetNextNotificationMessageID();
bool CanAddNewMessage();
void UpdateNotificationMessage();

#endif SLIPPI_CORE_NOTIFICATION_H