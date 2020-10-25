// keymap_ppsspp.h : Defines DirectInput scan codes for the hardware switches,
// based on keyboard controls in eX-BOARD games and Deathsmiles II.
//

#pragma once



// Player 1
#define P1_START          DIK_W
#define P1_START_FLAGS    KEYEVENTF_SCANCODE
#define P1_UP             DIK_UP
#define P1_UP_FLAGS       KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY
#define P1_DOWN           DIK_DOWN
#define P1_DOWN_FLAGS     KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY
#define P1_LEFT           DIK_LEFT
#define P1_LEFT_FLAGS     KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY
#define P1_RIGHT          DIK_RIGHT
#define P1_RIGHT_FLAGS    KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY
#define P1_BTN1           DIK_Z
#define P1_BTN1_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN2           DIK_X
#define P1_BTN2_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN3           DIK_C
#define P1_BTN3_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN4           DIK_V
#define P1_BTN4_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN5           DIK_N
#define P1_BTN5_FLAGS     KEYEVENTF_SCANCODE


// Player 2
#define P2_START          DIK_E
#define P2_START_FLAGS    KEYEVENTF_SCANCODE
#define P2_UP             DIK_NUMPAD8
#define P2_UP_FLAGS       KEYEVENTF_SCANCODE
#define P2_DOWN           DIK_NUMPAD2
#define P2_DOWN_FLAGS     KEYEVENTF_SCANCODE
#define P2_LEFT           DIK_NUMPAD4
#define P2_LEFT_FLAGS     KEYEVENTF_SCANCODE
#define P2_RIGHT          DIK_NUMPAD6
#define P2_RIGHT_FLAGS    KEYEVENTF_SCANCODE
#define P2_BTN1           DIK_A
#define P2_BTN1_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN2           DIK_S
#define P2_BTN2_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN3           DIK_D
#define P2_BTN3_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN4           DIK_F
#define P2_BTN4_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN5           DIK_T
#define P2_BTN5_FLAGS     KEYEVENTF_SCANCODE


// System
#define S_COIN1           DIK_K
#define S_COIN1_FLAGS     KEYEVENTF_SCANCODE
#define S_COIN2           DIK_J
#define S_COIN2_FLAGS     KEYEVENTF_SCANCODE
#define S_TEST1           DIK_H
#define S_TEST1_FLAGS     KEYEVENTF_SCANCODE
#define S_ESC1            DIK_ESCAPE // Exit (Deathsmiles II only)
#define S_ESC1_FLAGS      KEYEVENTF_SCANCODE
