// keymap_mame.h : Defines DirectInput scan codes for the hardware switches,
// based on default keyboard controls in MAME.
//

#pragma once



// Player 1
#define P1_START          DIK_1
#define P1_START_FLAGS    KEYEVENTF_SCANCODE
#define P1_UP             DIK_UP
#define P1_UP_FLAGS       KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY
#define P1_DOWN           DIK_DOWN
#define P1_DOWN_FLAGS     KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY
#define P1_LEFT           DIK_LEFT
#define P1_LEFT_FLAGS     KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY
#define P1_RIGHT          DIK_RIGHT
#define P1_RIGHT_FLAGS    KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY
#define P1_BTN1           DIK_LCONTROL
#define P1_BTN1_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN2           DIK_LALT
#define P1_BTN2_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN3           DIK_SPACE
#define P1_BTN3_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN4           DIK_LSHIFT
#define P1_BTN4_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN5           DIK_Z
#define P1_BTN5_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN6           DIK_X
#define P1_BTN6_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN7           DIK_C
#define P1_BTN7_FLAGS     KEYEVENTF_SCANCODE
#define P1_BTN8           DIK_V
#define P1_BTN8_FLAGS     KEYEVENTF_SCANCODE


// Player 2
#define P2_START          DIK_2
#define P2_START_FLAGS    KEYEVENTF_SCANCODE
#define P2_UP             DIK_R
#define P2_UP_FLAGS       KEYEVENTF_SCANCODE
#define P2_DOWN           DIK_F
#define P2_DOWN_FLAGS     KEYEVENTF_SCANCODE
#define P2_LEFT           DIK_D
#define P2_LEFT_FLAGS     KEYEVENTF_SCANCODE
#define P2_RIGHT          DIK_G
#define P2_RIGHT_FLAGS    KEYEVENTF_SCANCODE
#define P2_BTN1           DIK_A
#define P2_BTN1_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN2           DIK_S
#define P2_BTN2_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN3           DIK_Q
#define P2_BTN3_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN4           DIK_W
#define P2_BTN4_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN5           DIK_K // Unmapped
#define P2_BTN5_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN6           DIK_L // Unmapped
#define P2_BTN6_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN7           DIK_I // Unmapped
#define P2_BTN7_FLAGS     KEYEVENTF_SCANCODE
#define P2_BTN8           DIK_O // Unmapped
#define P2_BTN8_FLAGS     KEYEVENTF_SCANCODE


// System
#define S_COIN1           DIK_5
#define S_COIN1_FLAGS     KEYEVENTF_SCANCODE
#define S_COIN2           DIK_6
#define S_COIN2_FLAGS     KEYEVENTF_SCANCODE
#define S_TEST            DIK_F2
#define S_TEST_FLAGS      KEYEVENTF_SCANCODE
#define S_TILT            DIK_T
#define S_TILT_FLAGS      KEYEVENTF_SCANCODE
#define S_SERVICE1        DIK_9
#define S_SERVICE1_FLAGS  KEYEVENTF_SCANCODE
#define S_SERVICE2        DIK_0
#define S_SERVICE2_FLAGS  KEYEVENTF_SCANCODE
#define S_ESC             DIK_ESCAPE // Exit
#define S_ESC_FLAGS       KEYEVENTF_SCANCODE
