#ifndef SLIPPI_CSS_SETUP_C
#define SLIPPI_CSS_SETUP_C

#include "Main.c"

//Runs every frame during CSS
void minor_think() {
    void (*CSS_think)() = (void *) 0x802669F4;
    CSS_think();
    UpdateOnlineCSS();
}

//Runs when CSS is loaded
void minor_load() {
    void (*CSS_load)() = (void *) 0x8026688C;
    CSS_load();

    // Only request rank info if this is ranked
    u8 onlineMode = R13_U8(R13_OFFSET_ONLINE_MODE);
    if (onlineMode == 0) {
        InitRankInfo();
    }
    // OSReport("CSS_load\n");
}

//Runs when leaving CSS
void minor_exit() {
    void (*CSS_exit)() = (void *) 0x80266D70;
    CSS_exit();
    // OSReport("CSS_exit\n");
}

#endif SLIPPI_CSS_SETUP_C