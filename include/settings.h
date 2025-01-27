#ifndef SETTINGS_H
#define SETTINGS_H

//
//
// Multiplexors Settings
//
// Firmware assumes Hall sensors are connected
// channels [0 - (MUXX_KEY_COUNT-1)]
#ifndef MUX1_KEY_COUNT
#define MUX1_KEY_COUNT 12
#endif // !MUX1_KEY_COUNT

#ifndef MUX2_KEY_COUNT
#define MUX2_KEY_COUNT 11
#endif // !MUX2_KEY_COUNT

#ifndef MUX3_KEY_COUNT
#define MUX3_KEY_COUNT 11
#endif // !MUX3_KEY_COUNT

//
//
// Keyboard Settings
//
#ifndef KB_KEY_THRESHOLD_DEFAULT
#define KB_KEY_THRESHOLD_DEFAULT 200
#endif // KB_KEY_THRESHOLD_DEFAULT

#endif // SETTINGS_H
