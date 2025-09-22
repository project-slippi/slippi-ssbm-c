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

static const char* const HEADER_STRINGS[] = {
    "Page: Up",
    "Page: Left",
    "Page: Right",
    "Page: Down",
};

#endif SLIPPI_CSS_CHAT_TEXT_H
