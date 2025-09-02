#ifndef CONFIG_MAPPINGS_H
#define CONFIG_MAPPINGS_H

#include "keys.h"

// Mappings:

#define KB_LAYER_COUNT 2

//
// Layer 0:
//

// LEFT:

#ifndef KEY00
#define KEY00 KEY_NOKEY
#endif // KEY00
#ifndef KEY01
#define KEY01 KEY_ESCAPE
#endif // KEY01
#ifndef KEY02
#define KEY02 KEY_LEFTALT
#endif // KEY02

#ifndef KEY10
#define KEY10 KEY_Q
#endif // KEY10
#ifndef KEY11
#define KEY11 KEY_A
#endif // KEY11
#ifndef KEY12
#define KEY12 KEY_Z
#endif // KEY12

#ifndef KEY20
#define KEY20 KEY_W
#endif // KEY20
#ifndef KEY21
#define KEY21 KEY_S
#endif // KEY21
#ifndef KEY22
#define KEY22 KEY_X
#endif // KEY22

#ifndef KEY30
#define KEY30 KEY_E
#endif // KEY30
#ifndef KEY31
#define KEY31 KEY_D
#endif // KEY31
#ifndef KEY32
#define KEY32 KEY_C
#endif // KEY32

#ifndef KEY40
#define KEY40 KEY_R
#endif // KEY40
#ifndef KEY41
#define KEY41 KEY_F
#endif // KEY41
#ifndef KEY42
#define KEY42 KEY_V
#endif // KEY42

#ifndef KEY50
#define KEY50 KEY_T
#endif // KEY50
#ifndef KEY51
#define KEY51 KEY_G
#endif // KEY51
#ifndef KEY52
#define KEY52 KEY_B
#endif // KEY52

#ifndef KEY60
#define KEY60 KEY_LEFTCONTROL
#endif // KEY60
#ifndef KEY61
#define KEY61 KEY_LEFTSHIFT
#endif // KEY61
#ifndef KEY62
#define KEY62 KEY_SPACEBAR
#endif // KEY62
#ifndef KEY63
#define KEY63 KEY_TAB
#endif // KEY63

// RIGHT:

#ifndef KEY70
#define KEY70 KEY_BACKSPACE
#endif // KEY70
#ifndef KEY71
#define KEY71 KEY_ENTER
#endif // KEY71
#ifndef KEY72
#define KEY72 KEY_LAYER
#endif // KEY72
#ifndef KEY73
#define KEY73 KEY_NOKEY
#endif // KEY73

#ifndef KEY80
#define KEY80 KEY_Y
#endif // KEY80
#ifndef KEY81
#define KEY81 KEY_H
#endif // KEY81
#ifndef KEY82
#define KEY82 KEY_N
#endif // KEY82

#ifndef KEY90
#define KEY90 KEY_U
#endif // KEY90
#ifndef KEY91
#define KEY91 KEY_J
#endif // KEY91
#ifndef KEY92
#define KEY92 KEY_M
#endif // KEY92

#ifndef KEY100
#define KEY100 KEY_I
#endif // KEY100
#ifndef KEY101
#define KEY101 KEY_K
#endif // KEY101
#ifndef KEY102
#define KEY102 KEY_COMMA_LESSTHAN
#endif // KEY102

#ifndef KEY110
#define KEY110 KEY_O
#endif // KEY110
#ifndef KEY111
#define KEY111 KEY_L
#endif // KEY111
#ifndef KEY112
#define KEY112 KEY_MINUS_UNDERSCORE
#endif // KEY112

#ifndef KEY120
#define KEY120 KEY_P
#endif // KEY120
#ifndef KEY121
#define KEY121 KEY_SQBRACKETL_CURBRACEL
#endif // KEY121
#ifndef KEY122
#define KEY122 KEY_KEYPAD_LCURBRACE
#endif // KEY122

#ifndef KEY130
#define KEY130 KEY_NOKEY
#endif // KEY130
#ifndef KEY131
#define KEY131 KEY_RIGHTSHIFT
#endif // KEY131
#ifndef KEY132
#define KEY132 KEY_RIGHTGUI
#endif // KEY132

//
// Layer 1:
//

// LEFT:

#ifndef KEY00_LAYER_1
#define KEY00_LAYER_1 KEY_NOKEY
#endif // KEY00_LAYER_1
#ifndef KEY01_LAYER_1
#define KEY01_LAYER_1 KEY_ESCAPE
#endif // KEY01_LAYER_1
#ifndef KEY02_LAYER_1
#define KEY02_LAYER_1 KEY_LEFTALT
#endif // KEY02_LAYER_1

#ifndef KEY10_LAYER_1
#define KEY10_LAYER_1 KEY_KEYPAD_EXCLAMATION
#endif // KEY10_LAYER_1
#ifndef KEY11_LAYER_1
#define KEY11_LAYER_1 KEY_KEYPAD_0_INSERT
#endif // KEY11_LAYER_1
#ifndef KEY12_LAYER_1
#define KEY12_LAYER_1 KEY_Z
#endif // KEY12_LAYER_1

#ifndef KEY20_LAYER_1
#define KEY20_LAYER_1 KEY_KEYPAD_ATSIGN
#endif // KEY20_LAYER_1
#ifndef KEY21_LAYER_1
#define KEY21_LAYER_1 KEY_KEYPAD_1_END
#endif // KEY21_LAYER_1
#ifndef KEY22_LAYER_1
#define KEY22_LAYER_1 KEY_SLASH_QUESTIONMARK
#endif // KEY22_LAYER_1

#ifndef KEY30_LAYER_1
#define KEY30_LAYER_1 KEY_KEYPAD_NUMBERSIGN
#endif // KEY30_LAYER_1
#ifndef KEY31_LAYER_1
#define KEY31_LAYER_1 KEY_KEYPAD_2_DOWNARROW
#endif // KEY31_LAYER_1
#ifndef KEY32_LAYER_1
#define KEY32_LAYER_1 KEY_EQUALS_PLUS
#endif // KEY32_LAYER_1

#ifndef KEY40_LAYER_1
// Needs remaping to dollar sign
#define KEY40_LAYER_1 KEY_KEYPAD_DOUBLEAMPERSAND
#endif // KEY40_LAYER_1
#ifndef KEY41_LAYER_1
#define KEY41_LAYER_1 KEY_KEYPAD_3_PAGEDOWN
#endif // KEY41_LAYER_1
#ifndef KEY42_LAYER_1
#define KEY42_LAYER_1 KEY_SEMICOLON_COLON
#endif // KEY42_LAYER_1

#ifndef KEY50_LAYER_1
#define KEY50_LAYER_1 KEY_KEYPAD_PERCENT
#endif // KEY50_LAYER_1
#ifndef KEY51_LAYER_1
#define KEY51_LAYER_1 KEY_KEYPAD_4_LEFTARROW
#endif // KEY51_LAYER_1
#ifndef KEY52_LAYER_1
// Should be remapped to just tilde
#define KEY52_LAYER_1 KEY_GRAVEACCENT_TILDE
#endif // KEY52_LAYER_1

#ifndef KEY60_LAYER_1
#define KEY60_LAYER_1 KEY_LEFTCONTROL
#endif // KEY60_LAYER_1
#ifndef KEY61_LAYER_1
#define KEY61_LAYER_1 KEY_LEFTSHIFT
#endif // KEY61_LAYER_1
#ifndef KEY62_LAYER_1
#define KEY62_LAYER_1 KEY_SPACEBAR
#endif // KEY62_LAYER_1
#ifndef KEY63_LAYER_1
#define KEY63_LAYER_1 KEY_TAB
#endif // KEY63_LAYER_1

// RIGHT:

#ifndef KEY70_LAYER_1
#define KEY70_LAYER_1 KEY_BACKSPACE
#endif // KEY70_LAYER_1
#ifndef KEY71_LAYER_1
#define KEY71_LAYER_1 KEY_ENTER
#endif // KEY71_LAYER_1
#ifndef KEY72_LAYER_1
#define KEY72_LAYER_1 KEY_LAYER
#endif // KEY72_LAYER_1
#ifndef KEY73_LAYER_1
#define KEY73_LAYER_1 KEY_NOKEY
#endif // KEY73_LAYER_1

#ifndef KEY80_LAYER_1
#define KEY80_LAYER_1 KEY_KEYPAD_CARET
#endif // KEY80_LAYER_1
#ifndef KEY81_LAYER_1
#define KEY81_LAYER_1 KEY_KEYPAD_5
#endif // KEY81_LAYER_1
#ifndef KEY82_LAYER_1
// Should be remapped to just graveaccent
#define KEY82_LAYER_1 KEY_GRAVEACCENT_TILDE
#endif // KEY82_LAYER_1

#ifndef KEY90_LAYER_1
#define KEY90_LAYER_1 KEY_KEYPAD_AMPERSAND
#endif // KEY90_LAYER_1
#ifndef KEY91_LAYER_1
#define KEY91_LAYER_1 KEY_KEYPAD_6_RIGHTARROW
#endif // KEY91_LAYER_1
#ifndef KEY92_LAYER_1
#define KEY92_LAYER_1 KEY_APOSTROPHE_QUOTES
#endif // KEY92_LAYER_1

#ifndef KEY100_LAYER_1
#define KEY100_LAYER_1 KEY_KEYPAD_STAR
#endif // KEY100_LAYER_1
#ifndef KEY101_LAYER_1
#define KEY101_LAYER_1 KEY_KEYPAD_7_HOME
#endif // KEY101_LAYER_1
#ifndef KEY102_LAYER_1
#define KEY102_LAYER_1 KEY_DOT_GREATERTHAN
#endif // KEY102_LAYER_1

#ifndef KEY110_LAYER_1
#define KEY110_LAYER_1 KEY_KEYPAD_LPAREN
#endif // KEY110_LAYER_1
#ifndef KEY111_LAYER_1
#define KEY111_LAYER_1 KEY_KEYPAD_8_UPARROW
#endif // KEY111_LAYER_1
#ifndef KEY112_LAYER_1
#define KEY112_LAYER_1 KEY_BACKSLASH_VERTICALBAR
#endif // KEY112_LAYER_1

#ifndef KEY120_LAYER_1
#define KEY120_LAYER_1 KEY_KEYPAD_RPAREN
#endif // KEY120_LAYER_1
#ifndef KEY121_LAYER_1
#define KEY121_LAYER_1 KEY_SQBRACKETR_CURBRACER
#endif // KEY121_LAYER_1
#ifndef KEY122_LAYER_1
#define KEY122_LAYER_1 KEY_KEYPAD_RCURBRACE
#endif // KEY122_LAYER_1

#ifndef KEY130_LAYER_1
#define KEY130_LAYER_1 KEY_NOKEY
#endif // KEY130_LAYER_1
#ifndef KEY131_LAYER_1
#define KEY131_LAYER_1 KEY_RIGHTSHIFT
#endif // KEY131_LAYER_1
#ifndef KEY132_LAYER_1
#define KEY132_LAYER_1 KEY_RIGHTGUI
#endif // KEY132_LAYER_1

#endif // CONFIG_MAPPINGS_H
