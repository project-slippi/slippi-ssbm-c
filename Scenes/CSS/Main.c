#ifndef SLIPPI_CSS_MAIN_C
#define SLIPPI_CSS_MAIN_C

#include "Main.h"

#include "../../Core/Notifications/Notifications.c"
#include "Chat/Chat.c"
#include "SheikSelector.c"

void UpdateOnlineCSS()
{
    if (!IsSlippiOnlineCSS())
        return;
    // bp();

    // Do not handle notifications or chat input if on Name Entry Screen
    if (IsOnCSSNameEntryScreen())
        return;

    ListenForNotifications();
    ListenForChatInput();
    UpdateSheikSelector();

    /*
        // FN_SZThink
        UpdateSheikZeldaSelector();
            - this can probably stay in this file, definitely doesnt need its own folder
            - if so then SheikSelector.c SheikSelector.h

        ASM ::
            # Determine if cursor is in bounds of unselected button
            loadwz r4, 0x804A0BC0 # This gets ptr to cursor position on CSS
            lfs f1, 0x10(r4) # Get y cursor pos

            # Get cursor y top boundary
            lfs f2, TPO_BOUNDS_BUTTON_Z_TOP(REG_TEXT_PROPERTIES)
            lbz r3, 0x70(REG_PORT_SELECTIONS_ADDR)
            cmpwi r3, 0x13 # Check if Sheik is selected
            beq FN_SZThink_TOP_BOUND_SET
            lfs f2, TPO_BOUNDS_BUTTON_S_TOP(REG_TEXT_PROPERTIES)
            FN_SZThink_TOP_BOUND_SET:

            # Check if cursor is outside top boundary
            fcmpo cr0, f1, f2
            bgt FN_SZThink_CHANGE_HANDLING_END

            # Check if cursor is outside bottom boundary
            lfs f3, TPO_BOUNDS_BUTTON_HEIGHT(REG_TEXT_PROPERTIES)
            fsubs f2, f2, f3
            fcmpo cr0, f1, f2
            blt FN_SZThink_CHANGE_HANDLING_END

            # Now we do left and right bounds
            lfs f1, 0xC(r4) # Get x cursor pos

            # Check if cursor is left of left bounds
            lfs f2, TPO_BOUNDS_BUTTON_LEFT(REG_TEXT_PROPERTIES)
            fcmpo cr0, f1, f2
            blt FN_SZThink_CHANGE_HANDLING_END

            # Check right boundary
            lfs f2, TPO_BOUNDS_BUTTON_RIGHT(REG_TEXT_PROPERTIES)
            fcmpo cr0, f1, f2
            bgt FN_SZThink_CHANGE_HANDLING_END

            # If we get here, the cursor is within the bounds of the unselected button
            li REG_IS_HOVERING, 1

            # Check if a button was pressed this frame
            load r4, 0x804c20bc
            lbz r3, -0x49B0(r13) # player index
            mulli r3, r3, 68
            add r3, r4, r3
            lwz r3, 0x8(r3) # get inputs
            rlwinm. r3, r3, 0, 23, 23 # check if a was pressed
            beq FN_SZThink_CHANGE_HANDLING_END

            # Toggle the selected character
            lbz r3, 0x70(REG_PORT_SELECTIONS_ADDR)
            cmpwi r3, 0x13 # Check if Sheik is selected
            beq FN_SZThink_SWITCH_TO_ZELDA

            # Switch to sheik
            li r3, 0x13
            stb r3, 0x70(REG_PORT_SELECTIONS_ADDR)

            b FN_SZThink_SWITCH_COMPLETE

            FN_SZThink_SWITCH_TO_ZELDA:
            # Switch to zelda
            li r3, 0x12
            stb r3, 0x70(REG_PORT_SELECTIONS_ADDR)

            FN_SZThink_SWITCH_COMPLETE:
            # Required to play correct sound and show correct nameplate
            load r4, 0x803f0cc8
            stb r3, 0x1(r4)

            # Play sound r3 is ext ID
            branchl r12, 0x80168c5c # SFX_getCharacterNameAnnouncer

            li r3, 0
            branchl r12, 0x8025db34 # CSS_CursorHighlightUpdateCSPInfo

            FN_SZThink_CHANGE_HANDLING_END:

            ################################################################################
            # Prepare to set styles
            ################################################################################
            .set REG_SUBTEXT_IDX_SELECTED, REG_VARIOUS_1
            .set REG_SUBTEXT_IDX_UNSELECTED, REG_VARIOUS_2
            .set REG_UNSELECTED_COLOR, REG_VARIOUS_3

            li REG_SUBTEXT_IDX_SELECTED, 0
            addi REG_SUBTEXT_IDX_UNSELECTED, REG_SUBTEXT_IDX_SELECTED, SUBTEXT_ITEM_COUNT_PER_BUTTON
            lbz r3, 0x70(REG_PORT_SELECTIONS_ADDR)
            cmpwi r3, 0x13 # Check if Sheik is selected
            beq FN_SZThink_SUBTEXT_IDX_INITIALIZED

            # Set subtext idx to Zelda button
            li REG_SUBTEXT_IDX_UNSELECTED, 0
            addi REG_SUBTEXT_IDX_SELECTED, REG_SUBTEXT_IDX_UNSELECTED, SUBTEXT_ITEM_COUNT_PER_BUTTON

            FN_SZThink_SUBTEXT_IDX_INITIALIZED:

            ################################################################################
            # Set selected styles
            ################################################################################
            mr r3, REG_TEXT_STRUCT
            addi r4, REG_SUBTEXT_IDX_SELECTED, 0
            addi r5, REG_TEXT_PROPERTIES, TPO_COLOR_GREEN
            branchl r12, Text_ChangeTextColor

            mr r3, REG_TEXT_STRUCT
            addi r4, REG_SUBTEXT_IDX_SELECTED, 1
            addi r5, REG_TEXT_PROPERTIES, TPO_COLOR_WHITE
            branchl r12, Text_ChangeTextColor

            ################################################################################
            # Set unselected styles
            ################################################################################
            addi REG_UNSELECTED_COLOR, REG_TEXT_PROPERTIES, TPO_COLOR_GRAY
            cmpwi REG_IS_HOVERING, 0
            beq FN_SZThink_UNSELECTED_COLOR_SET
            addi REG_UNSELECTED_COLOR, REG_TEXT_PROPERTIES, TPO_COLOR_WHITE
            FN_SZThink_UNSELECTED_COLOR_SET:

            mr r3, REG_TEXT_STRUCT
            addi r4, REG_SUBTEXT_IDX_UNSELECTED, 0
            mr r5, REG_UNSELECTED_COLOR
            branchl r12, Text_ChangeTextColor

            mr r3, REG_TEXT_STRUCT
            addi r4, REG_SUBTEXT_IDX_UNSELECTED, 1
            mr r5, REG_UNSELECTED_COLOR
            branchl r12, Text_ChangeTextColor
    */
}

void InitOnlineCSS()
{
    if (!IsSlippiOnlineCSS())
        return;

    InitChatMessages();
}

#endif SLIPPI_CSS_MAIN_C
