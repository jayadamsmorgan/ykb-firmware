#ifndef SETTINGS_H
#define SETTINGS_H

//
//
// Keyboard Settings
//
#ifndef KB_KEY_COUNT
#define KB_KEY_COUNT 34
#endif // KB_KEY_COUNT

#ifndef KB_KEY_THRESHOLD_DEFAULT
#define KB_KEY_THRESHOLD_DEFAULT 500
#endif // KB_KEY_THRESHOLD_DEFAULT

//
//
// Multiplexors Settings
//
// Firmware assumes Hall sensors are connected
// channels [0 - (MUXX_KEY_COUNT-1)]
#ifndef MUX1_KEY_COUNT
#define MUX1_KEY_COUNT 12
#endif // MUX1_KEY_COUNT

#ifndef MUX2_KEY_COUNT
#define MUX2_KEY_COUNT 11
#endif // MUX2_KEY_COUNT

#ifndef MUX3_KEY_COUNT
#define MUX3_KEY_COUNT 11
#endif // MUX3_KEY_COUNT

//
//
// Debug
//
#ifndef DBG_LONG_BLINK_DELAY
#define DBG_LONG_BLINK_DELAY 1000
#endif // DBG_LONG_BLINK_DELAY

#ifndef DBG_SHORT_BLINK_DELAY
#define DBG_SHORT_BLINK_DELAY 300
#endif // DBG_SHORT_BLINK_DELAY

//
//
// Checks
//
// Double checking settings are somewhat correct

#if MUX1_KEY_COUNT + MUX2_KEY_COUNT + MUX3_KEY_COUNT != KB_KEY_COUNT
#error Incorrect amount of keys or MUX channels
#endif

#endif // SETTINGS_H
