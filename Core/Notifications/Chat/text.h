#ifndef SLIPPI_CSS_CHAT_TEXT_H
#define SLIPPI_CSS_CHAT_TEXT_H
#include "../../../slippi.h"

enum CHAT_LABELS {
	CHAT_STR_HEADER,
	CHAT_STR_UP,
	CHAT_STR_LEFT,
	CHAT_STR_RIGHT,
	CHAT_STR_DOWN,
};

enum MSG_COLOR_NAMES {
	MSG_COLOR_WHITE,
	MSG_COLOR_RED, 
	MSG_COLOR_BLUE,
	MSG_COLOR_YELLOW,
	MSG_COLOR_GREEN,
	MSG_COLOR_CHAT_WINDOW,
	MSG_COLOR_CHAT_WINDOW_IDLE,
};

static GXColor MSG_COLORS[] = {
    {255, 255, 255, 255},
	{229, 76, 76, 255},
	{59, 189, 255, 255},
	{255, 203, 4, 255},
	{0, 178, 2, 255},
	{255, 234, 47, 255},
	{201, 195, 135, 255},
};

char ChatGroups[4][5][45] = {
	{
		"Common",
		"ggs",
		"one more",
		"brb",
		"good luck"
	},
	{
		"Compliments",
		"well played",
		"that was fun",
		"thanks",
		"too good"
	},
	{
		"Reactions",
		"oof",
		"my b",
		"lol",
		"wow"
	},
	{
		"Misc",
		"okay",
		"thinking",
		"let's play again later",
		"bad connection"
	}
};
char ChatGroup_Common[5][45] = {
	"Common",
	"ggs",
	"one more",
	"brb",
	"good luck"
};
char ChatGroup_Compliments[5][45] = {
	"Compliments",
	"well played",
	"that was fun",
	"thanks",
	"too good"
};
char ChatGroup_Reactions[5][45] = {
	"Reactions",
	"oof",
	"my b",
	"lol",
	"wow"
};
char ChatGroup_Misc[5][45] = {
    "Misc",
    "okay",
    "thinking",
    "let's play again later",
    "bad connection"
};
#endif SLIPPI_CSS_CHAT_TEXT_H
