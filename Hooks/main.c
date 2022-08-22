#include "../../MexTK/mex.h"
#include "../common.h"
#include "../slippi.h"

#include "../CSS/main.c"

void OnBoot() {
    OSReport("OnBoot!\n");
}

void OnSceneChange() {
    OSReport("OnSceneChange!\n");
}

void OnCSSLoad() {
    OSReport("OnCSSLoad!\n");
}

void OnSSSLoad() {
    OSReport("OnSSSLoad!\n");
}

void UpdateCSS() {
    // OSReport("UpdateCSS!\n");
    UpdateOnlineCSS();
}

void OnButtonRemap(int playerIndex) {
    // OSReport("OnButtonRemap %i !\n", playerIndex);
}