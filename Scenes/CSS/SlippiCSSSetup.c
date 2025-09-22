#ifndef SLIPPI_CSS_SETUP_C
#define SLIPPI_CSS_SETUP_C

#include "Main.c"

// Sanity check gate
static bool has_initialized = false;

// Runs every frame during CSS
void minor_think() {
    void (*CSS_think)() = (void *) 0x802669F4;
    CSS_think();
    if (has_initialized) {
        UpdateOnlineCSS();
    }
}

// Runs when CSS is loaded
void minor_load() {
    void (*CSS_load)() = (void *) 0x8026688C;
    CSS_load();
    InitOnlineCSS();
    has_initialized = true;
    // OSReport("CSS_load\n");
}

//Runs when leaving CSS
void minor_exit() {
    void (*CSS_exit)() = (void *) 0x80266D70;
    CSS_exit();
    DeinitOnlineCSS();
    has_initialized = false;
    // OSReport("CSS_exit\n");
}

#endif SLIPPI_CSS_SETUP_C