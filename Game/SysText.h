#ifndef GAME_SYS_TEXT_H
#define GAME_SYS_TEXT_H

#include "../m-ex/MexTK/mex.h"
#include <stdbool.h>
#include "../Common.h"

#define CHAR_MAP_SIZE 287

// region CharMAPS
int CHAR_MAP[CHAR_MAP_SIZE] = {
        U'0', U'1', U'2', U'3', U'4', U'5', U'6', U'7', U'8', U'9', U'A',
        U'B', U'C', U'D', U'E', U'F', U'G', U'H', U'I', U'J', U'K',
        U'L', U'M', U'N', U'O', U'P', U'Q', U'R', U'S', U'T', U'U',
        U'V', U'W', U'X', U'Y', U'Z', U'a', U'b', U'c', U'd', U'e',
        U'f', U'g', U'h', U'i', U'j', U'k', U'l', U'm', U'n', U'o',
        U'p', U'q', U'r', U's', U't', U'u', U'v', U'w', U'x', U'y',
        U'z', U'ぁ', U'あ', U'ぃ', U'い', U'ぅ', U'う', U'ぇ', U'え', U'ぉ',
        U'お', U'か', U'が', U'き', U'ぎ', U'く', U'ぐ', U'け', U'げ', U'こ',
        U'ご', U'さ', U'ざ', U'し', U'じ', U'す', U'ず', U'せ',
        U'ぜ', U'そ', U'ぞ', U'た', U'だ', U'ち', U'ぢ', U'っ', U'つ', U'づ', U'て', U'で',
        U'と', U'ど', U'な', U'に', U'ぬ', U'ね', U'の', U'は', U'ば', U'ぱ',
        U'ひ', U'び', U'ぴ', U'ふ', U'ぶ', U'ぷ', U'へ', U'べ', U'ぺ', U'ほ',
        U'ぼ', U'ぽ', U'ま', U'み', U'む', U'め', U'も', U'ゃ', U'や', U'ゅ',
        U'ゆ', U'ょ', U'よ', U'ら', U'り', U'る', U'れ', U'ろ', U'ゎ', U'わ',
        U'を', U'ん', U'ァ', U'ア', U'ィ', U'イ', U'ゥ', U'ウ', U'ェ', U'エ',
        U'ォ', U'オ', U'カ', U'ガ', U'キ', U'ギ', U'ク', U'グ', U'ケ', U'ゲ',
        U'コ', U'ゴ', U'サ', U'ザ', U'シ', U'ジ', U'ス', U'ズ', U'セ', U'ゼ',
        U'ソ', U'ゾ', U'タ', U'ダ', U'チ', U'ヂ', U'ッ', U'ツ', U'ヅ', U'テ',
        U'デ', U'ト', U'ド', U'ナ', U'ニ', U'ヌ', U'ネ', U'ノ', U'ハ', U'バ',
        U'パ', U'ヒ', U'ビ', U'ピ', U'フ', U'ブ', U'プ', U'ヘ', U'ベ', U'ペ',
        U'ホ', U'ボ', U'ポ', U'マ', U'ミ', U'ム', U'メ', U'モ', U'ャ', U'ヤ',
        U'ュ', U'ユ', U'ョ', U'ヨ', U'ラ', U'リ', U'ル', U'レ', U'ロ', U'ヮ',
        U'ワ', U'ヲ', U'ン', U'ヴ', U'ヵ', U'ヶ', U'　', U'、', U'。', U',',
        U'.', U'•', U':', U';', U'?', U'!', U'^', U'_', U'—', U'/',
        U'~', U'|', U'\'', U'"', U'(', U')', U'[', U']', U'{', U'}',
        U'+', '-', U'×', U'=', U'<', U'>', U'¥', U'$', U'%', U'#',
        U'&', U'*', U'@', U'扱', U'押', U'軍', U'源', U'個', U'込', U'指',
        U'示', U'取', U'書', U'詳', U'人', U'生', U'説', U'体', U'団', U'電',
        U'読', U'発', U'抜', U'閑', U'本', U'明'
};
int HEX_MAP[CHAR_MAP_SIZE] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        0x829F, 0x82A0, 0x82A1, 0x82A2, 0x82A3, 0x82A4, 0x82A5, 0x82A6, 0x82A7, 0x82A8, 0x82A9, 0x82AA, 0x82AB, 0x82AC,
        0x82AD, 0x82AE, 0x82AF, 0x82B0, 0x82B1, 0x82B2, 0x82B3, 0x82B4, 0x82B5, 0x82B6, 0x82B7, 0x82B8, 0x82B9, 0x82BA,
        0x82BB, 0x82BC, 0x82BD, 0x82BE, 0x82BF, 0x82C0, 0x82C1, 0x82C2, 0x82C3, 0x82C4, 0x82C5, 0x82C6, 0x82C7, 0x82C8,
        0x82C9, 0x82CA, 0x82CB, 0x82CC, 0x82CD, 0x82CE, 0x82CF, 0x82D0, 0x82D1, 0x82D2, 0x82D3, 0x82D4, 0x82D5, 0x82D6,
        0x82D7, 0x82D8, 0x82D9, 0x82DA, 0x82DB, 0x82DC, 0x82DD, 0x82DE, 0x82DF, 0x82E0, 0x82E1, 0x82E2, 0x82E3, 0x82E4,
        0x82E5, 0x82E6, 0x82E7, 0x82E8, 0x82E9, 0x82EA, 0x82EB, 0x82EC, 0x82ED, 0x82F0, 0x82F1, 0x8340, 0x8341, 0x8342,
        0x8343, 0x8344, 0x8345, 0x8346, 0x8347, 0x8348, 0x8349, 0x834A, 0x834B, 0x834C, 0x834D, 0x834E, 0x834F, 0x8350,
        0x8351, 0x8352, 0x8353, 0x8354, 0x8355, 0x8356, 0x8357, 0x8358, 0x8359, 0x835A, 0x835B, 0x835C, 0x835D, 0x835E,
        0x835F, 0x8360, 0x8361, 0x8362, 0x8363, 0x8364, 0x8365, 0x8366, 0x8367, 0x8368, 0x8369, 0x836A, 0x836B, 0x836C,
        0x836D, 0x836E, 0x836F, 0x8370, 0x8371, 0x8372, 0x8373, 0x8374, 0x8375, 0x8376, 0x8377, 0x8378, 0x8379, 0x837A,
        0x837B, 0x837C, 0x837D, 0x837E, 0x8380, 0x8381, 0x8382, 0x8383, 0x8384, 0x8385, 0x8386, 0x8387, 0x8388, 0x8389,
        0x838A, 0x838B, 0x838C, 0x838D, 0x838E, 0x838F, 0x8392, 0x8393, 0x8394, 0x8395, 0x8396, 0x8140, 0x8141, 0x8142,
        0x8143, 0x8144, 0x8145, 0x8146, 0x8147, 0x8148, 0x8149, 0x814f,
        0x8151, 0x817d, 0x815e, 0x8160, 0x8162,
        0x8166, 0x8168, 0x8169, 0x816a, 0x816d, 0x816e, 0x816f,
        0x8170, 0x817b, 0x817c, 0x817e, 0x8181, 0x8183, 0x8184,
        0x815F,
        0x8190, 0x8193, 0x8194, 0x8195, 0x8196, 0x8197,
        0x88b5, 0x899f, 0x8c52, 0x8cb9, 0x8cc2, 0x8d9e,
        0x8e77, 0x8ea6, 0x8ee6, 0x8f91, 0x8fda, 0x906c, 0x90b6, 0x90e0, 0x91cc, 0x9263, 0x9364, 0x93c7, 0x94ad, 0x94b2,
        0x8ad5, 0x967b, 0x96be
};

// Extracted and refactored from https://github.com/AltimorTASDK/ssbm-1.03/blob/master/src/mod/src/melee/text.h
enum TEXT_OP_CODE {
    TEXT_OP_CODE_END = 0x00,
    TEXT_OP_CODE_RESET = 0x01,
    TEXT_OP_CODE_DELETE_PRECEDING = 0x02,
    TEXT_OP_CODE_LINE_BREAK = 0x03,
    TEXT_OP_CODE_ZERO_WIDTH = 0x04,
    TEXT_OP_CODE_PAUSE = 0x05,
    TEXT_OP_CODE_TYPE_SPEED = 0x06,
    TEXT_OP_CODE_OFFSET = 0x07,
    TEXT_OP_CODE_SET_POINTER_SUB = 0x08,
    TEXT_OP_CODE_SET_POINTER = 0x09,
    TEXT_OP_CODE_SPACING = 0x0A,
    TEXT_OP_CODE_RESET_SPACING = 0x0B,
    TEXT_OP_CODE_COLOR = 0x0C,
    TEXT_OP_CODE_CLEAR_COLOR = 0x0D,
    TEXT_OP_CODE_SCALE = 0x0E,
    TEXT_OP_CODE_RESET_SCALE = 0x0F,
    TEXT_OP_CODE_CENTERED = 0x10,
    TEXT_OP_CODE_RESET_CENTERED = 0x11,
    TEXT_OP_CODE_LEFT_ALIGNED = 0x12,
    TEXT_OP_CODE_RESET_LEFT_ALIGN = 0x13,
    TEXT_OP_CODE_RIGHT_ALIGNED = 0x14,
    TEXT_OP_CODE_RESET_RIGHT_ALIGN = 0x15,
    TEXT_OP_CODE_KERNING = 0x16,
    TEXT_OP_CODE_NO_KERNING = 0x17,
    TEXT_OP_CODE_FITTING = 0x18,
    TEXT_OP_CODE_NO_FITTING = 0x19,
    TEXT_OP_CODE_SPACE = 0x1A,
    TEXT_OP_CODE_COMMON_CHARACTER = 0x20,
    TEXT_OP_CODE_SPECIAL_CHARACTER = 0x40
};


typedef struct SysText  {
    char *chars;
    int charsCount;
} SysText ;

SysText *st_create();
void st_destroy(SysText* component);

#endif GAME_SYS_TEXT_H
