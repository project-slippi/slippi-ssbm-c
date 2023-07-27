#ifndef GAME_SYS_TEXT_C
#define GAME_SYS_TEXT_C

#include "SysText.h"

SysText *st_create() {
    SysText *component = calloc(sizeof(SysText));

    return component;
}

/**
 * Pushes a character into the chars element of a SysText
 * @param tb
 * @param character
 * @return tb
 */
SysText *st_push(SysText *tb, int charCount, ...) {
    int origLength = tb->charsCount;
    //OSReport("SysText#st_push charCount:0x%x tb.charCount:0x%x \n", charCount, tb->charsCount);
    va_list args;

    if (!tb->chars) {
        // TODO: be in the lookout for this being too small, or just dynamically resize it
        tb->chars = (char *) calloc(100);
    }

    va_start(args, charCount);
    // This is just a naive memcpy
    for (int i = 0; i < charCount; i++) {
        char character = va_arg(args, int);

        tb->chars[origLength + i] = character;
        tb->charsCount++;
    }

    va_end(args);
    //OSReport("SysText#st_push chars: 0x%x tb.charCount 0x%x \n", *(tb->chars), tb->charsCount);

    return tb;
}

SysText *st_color(u8 r, u8 g, u8 b) {
    return st_push(st_create(), 4, TEXT_OP_CODE_COLOR, r, g, b);
}

SysText *st_end_color() {
    return st_push(st_create(), 1, TEXT_OP_CODE_CLEAR_COLOR);
};

SysText *st_kern() {
    return st_push(st_create(), 1, TEXT_OP_CODE_KERNING);
};

SysText *st_space() {
    return st_push(st_create(), 1, TEXT_OP_CODE_SPACE);
};

SysText st_end_kern() {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_NO_KERNING},
            .charsCount = 1,
    };
};

SysText *st_left() {
    return st_push(st_create(), 1, TEXT_OP_CODE_LEFT_ALIGNED);
};

SysText st_end_left() {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_RESET_LEFT_ALIGN},
            .charsCount = 1,
    };
};

SysText st_center() {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_CENTERED},
            .charsCount = 1,
    };
};

SysText st_end_center() {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_RESET_CENTERED},
            .charsCount = 1,
    };
};

SysText st_right() {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_RIGHT_ALIGNED},
            .charsCount = 1,
    };
};

SysText st_end_right() {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_RESET_RIGHT_ALIGN},
            .charsCount = 1,
    };
};

SysText st_fit() {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_FITTING},
            .charsCount = 1,
    };
};

SysText st_end_fit() {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_NO_FITTING},
            .charsCount = 1,
    };
};

SysText st_scale(u16 x, u16 y) {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_SCALE,
                               (char) (x >> 8), (char) (x & 0xFF),
                               (char) (y >> 8), (char) (y & 0xFF)},
            .charsCount = 5,
    };
};

SysText st_reset_scale() {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_RESET_SCALE},
            .charsCount = 1,
    };
};

SysText st_offset(s16 x, s16 y) {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_OFFSET,
                               (char) (x >> 8), (char) (x & 0xFF),
                               (char) (y >> 8), (char) (y & 0xFF)},
            .charsCount = 5,
    };
};

SysText st_pause(u16 delay) {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_PAUSE, (char) (delay >> 8), (char) (delay & 0xFF)},
            .charsCount = 3,
    };
};

SysText st_type_speed(u16 per_char, u16 per_line) {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_TYPE_SPEED,
                               (char) (per_char >> 8), (char) (per_char & 0xFF),
                               (char) (per_line >> 8), (char) (per_line & 0xFF)},
            .charsCount = 5,
    };
};

SysText st_br() {
    return (SysText) {
            .chars = (char[]) {(char) TEXT_OP_CODE_LINE_BREAK},
            .charsCount = 1,
    };
};

SysText *st_character(int ch, bool isShiftJis) {
    int pos = -1;
    for (int i = 0; i < 287; i++) {
        bool matches = (isShiftJis && (int) HEX_MAP[i] == (int) ch) ||
                       (int) CHAR_MAP[i] == (int) ch;
        if (matches) {
            pos = i;
            break;
        }

    }

    if (pos >= 0) {
        u16 sht = (u16) (((u16) TEXT_OP_CODE_COMMON_CHARACTER << 8) | pos);
        u8 r = (u8) (sht >> 8);
        u8 r2 = (u8) (sht & 0xFF);
        return st_push(st_create(), 2, r, r2);
    }

    return st_space();
}

SysText *st_build_text(char *str, bool isShiftJis) {
    //OSReport("SysText#st_build_text isShiftJis %i text: %s\n", isShiftJis, str);
    SysText *component = st_create();

    for (int i = 0; i < strlen(str); i++) {
        int ch = (int) str[i];

        // check for special chars
        if (ch >= 0x81 && i + 1 < strlen(str)) {
            ch = str[i] << 8 | str[i + 1];
            i++;
        }
        SysText *char_component = st_character(ch, isShiftJis);
        for (int j = 0; j < char_component->charsCount; j++) {
            st_push(component, 1, char_component->chars[j]);
        }
    }

    return component;
};

SysText *st_text(char *str) {
    return st_build_text(str, false);
};

SysText *st_sjis_text(char *str) {
    return st_build_text(str, true);
};

char *st_build(int componentsCount, ...) {
    int fixedSize = 400;
    char *placeholder = calloc(fixedSize);
    va_list args;
    va_start(args, componentsCount);

    int offset = 0;
    for (int i = 0; i < componentsCount; i++) {
        SysText *component = va_arg(args, SysText*);
        memcpy(placeholder + (offset), component->chars, component->charsCount);
        HSD_Free(component->chars);
        HSD_Free(component);
        offset += component->charsCount;
    }
    va_end(args);
    return placeholder;
}

#endif GAME_SYS_TEXT_C
