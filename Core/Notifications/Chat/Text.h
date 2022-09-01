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
    CHAT_MSG_COMMON_1,
    CHAT_MSG_COMMON_2,
    CHAT_MSG_COMMON_3,
    CHAT_MSG_COMMON_4,

    CHAT_MSG_COMPLIMENTS,
    CHAT_MSG_COMPLIMENTS_1,
    CHAT_MSG_COMPLIMENTS_2,
    CHAT_MSG_COMPLIMENTS_3,
    CHAT_MSG_COMPLIMENTS_4,

    CHAT_MSG_REACTIONS,
    CHAT_MSG_REACTIONS_1,
    CHAT_MSG_REACTIONS_2,
    CHAT_MSG_REACTIONS_3,
    CHAT_MSG_REACTIONS_4,

    CHAT_MSG_MISC,
    CHAT_MSG_MISC_1,
    CHAT_MSG_MISC_2,
    CHAT_MSG_MISC_3,
    CHAT_MSG_MISC_4,
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
        "sorry",
        "my b",
        "lol",
        "wow",

        "Misc",
        "gotta go",
        "one sec",
        "let's play again later",
        "bad connection",
};

int ChatGroups[4][5] = {
        {
                CHAT_MSG_COMMON,
                CHAT_MSG_COMMON_1,
                CHAT_MSG_COMMON_2,
                CHAT_MSG_COMMON_3,
                CHAT_MSG_COMMON_4,
        },
        {
                CHAT_MSG_COMPLIMENTS,
                CHAT_MSG_COMPLIMENTS_1,
                CHAT_MSG_COMPLIMENTS_2,
                CHAT_MSG_COMPLIMENTS_3,
                CHAT_MSG_COMPLIMENTS_4,
        },
        {
                CHAT_MSG_REACTIONS,
                CHAT_MSG_REACTIONS_1,
                CHAT_MSG_REACTIONS_2,
                CHAT_MSG_REACTIONS_3,
                CHAT_MSG_REACTIONS_4,
        },
        {
                CHAT_MSG_MISC,
                CHAT_MSG_MISC_1,
                CHAT_MSG_MISC_2,
                CHAT_MSG_MISC_3,
                CHAT_MSG_MISC_4,
        }
};

#endif SLIPPI_CSS_CHAT_TEXT_H
