#ifndef SLIPPI_CSS_CHAT_TEXT_H
#define SLIPPI_CSS_CHAT_TEXT_H

#include "../../../Slippi.h"

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
        {229, 76,  76,  255},
        {59,  189, 255, 255},
        {255, 203, 4,   255},
        {0,   178, 2,   255},
        {255, 234, 47,  255},
        {201, 195, 135, 255},
};

enum CHAT_MSG_NAMES {

    CHAT_MSG_COMMON,
    CHAT_MSG_GGS,
    CHAT_MSG_ONE_MORE,
    CHAT_MSG_BRB,
    CHAT_MSG_GOOD_LUCK,

    CHAT_MSG_COMPLIMENTS,
    CHAT_MSG_WELL_PLAYED,
    CHAT_MSG_THAT_WAS_FUN,
    CHAT_MSG_THANKS,
    CHAT_MSG_TOO_GOOD,

    CHAT_MSG_REACTIONS,
    CHAT_MSG_OOF,
    CHAT_MSG_MY_B,
    CHAT_MSG_LOL,
    CHAT_MSG_WOW,

    CHAT_MSG_MISC,
    CHAT_MSG_OKAY,
    CHAT_MSG_THINKING,
    CHAT_MSG_LETS_PLAY_AGAIN_LATER,
    CHAT_MSG_BAD_CONNECTION,
};

static char* CHAT_MSG_STRINGS[] = {
        "Common",
        "ggs",
        "one more",
        "brb",
        "good luck",

        "Compliments",
        "well played",
        "that was fun",
        "thanks",
        "too good",

        "Reactions",
        "oof",
        "my b",
        "lol",
        "wow",

        "Misc",
        "okay",
        "thinking",
        "let's play again later",
        "bad connection",
};

int ChatGroups[4][5] = {
        {
                CHAT_MSG_COMMON,
                CHAT_MSG_GGS,
                CHAT_MSG_ONE_MORE,
                CHAT_MSG_BRB,
                CHAT_MSG_GOOD_LUCK
        },
        {
                CHAT_MSG_COMPLIMENTS,
                CHAT_MSG_WELL_PLAYED,
                CHAT_MSG_THAT_WAS_FUN,
                CHAT_MSG_THANKS,
                CHAT_MSG_TOO_GOOD
        },
        {
                CHAT_MSG_REACTIONS,
                CHAT_MSG_OOF,
                CHAT_MSG_MY_B,
                CHAT_MSG_LOL,
                CHAT_MSG_WOW
        },
        {
                CHAT_MSG_MISC,
                CHAT_MSG_OKAY,
                CHAT_MSG_THINKING,
                CHAT_MSG_LETS_PLAY_AGAIN_LATER,
                CHAT_MSG_BAD_CONNECTION
        }
};

//char ChatGroups[4][5][45] = {
//        {
//                "Common",
//                "ggs",
//                "one more",
//                "brb",
//                "good luck"
//        },
//        {
//                "Compliments",
//                "well played",
//                "that was fun",
//                "thanks",
//                "too good"
//        },
//        {
//                "Reactions",
//                "oof",
//                "my b",
//                "lol",
//                "wow"
//        },
//        {
//                "Misc",
//                "okay",
//                "thinking",
//                "let's play again later",
//                "bad connection"
//        }
//};
#endif SLIPPI_CSS_CHAT_TEXT_H
