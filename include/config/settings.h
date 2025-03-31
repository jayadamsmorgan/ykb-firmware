#ifndef CONFIG_SETTINGS_H
#define CONFIG_SETTINGS_H

//
//
// Keyboard Settings
//

#ifndef KB_KEY_THRESHOLD_DEFAULT
#define KB_KEY_THRESHOLD_DEFAULT 750
#endif // KB_KEY_THRESHOLD_DEFAULT

#ifndef KB_ADC_SAMPLING_DEFAULT
#define KB_ADC_SAMPLING_DEFAULT ADC_SMP_92_5_CYCLES
#endif // KB_ADC_SAMPLING_DEFAULT

#ifndef HID_BUFFER_SIZE
#define HID_BUFFER_SIZE 8
#endif // HID_BUFFER_SIZE

// Left part settings:

#ifdef LEFT

#ifndef KB_KEY_COUNT
#define KB_KEY_COUNT 35
#endif // KB_KEY_COUNT

#endif // LEFT

// Right part settings:

#ifdef RIGHT

#ifndef KB_KEY_COUNT
#define KB_KEY_COUNT 35
#endif // KB_KEY_COUNT

#endif // RIGHT

//
//
// Multiplexors Settings
//
// Both KB_KEY_COUNT and MUXx_KEY_COUNT
// might be greater than the actual key count
// Because some of the channels are skipped
#ifdef LEFT

#ifndef MUX1_KEY_COUNT
#define MUX1_KEY_COUNT 11
#endif // MUX1_KEY_COUNT

#ifndef MUX2_KEY_COUNT
#define MUX2_KEY_COUNT 15
#endif // MUX2_KEY_COUNT

#ifndef MUX3_KEY_COUNT
#define MUX3_KEY_COUNT 9
#endif // MUX3_KEY_COUNT

#endif // LEFT

#ifdef RIGHT

#ifndef MUX1_KEY_COUNT
#define MUX1_KEY_COUNT 11
#endif // MUX1_KEY_COUNT

#ifndef MUX2_KEY_COUNT
#define MUX2_KEY_COUNT 15
#endif // MUX2_KEY_COUNT

#ifndef MUX3_KEY_COUNT
#define MUX3_KEY_COUNT 9
#endif // MUX3_KEY_COUNT

#endif // RIGHT

//
//
// Debug
//

#ifdef DEBUG

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif // LOG_LEVEL

#ifndef DBG_LONG_BLINK_DELAY
#define DBG_LONG_BLINK_DELAY 1000
#endif // DBG_LONG_BLINK_DELAY

#ifndef DBG_SHORT_BLINK_DELAY
#define DBG_SHORT_BLINK_DELAY 300
#endif // DBG_SHORT_BLINK_DELAY

#endif // DEBUG

//
//
// Change with caution:
//

#define MAX3(a, b, c)                                                          \
    (((a) > (b)) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))
#define MAX_CHANNELS_PER_MUX                                                   \
    MAX3(MUX1_KEY_COUNT, MUX2_KEY_COUNT, MUX3_KEY_COUNT)

//
//
// Checks
//
// Double checking settings are somewhat correct

#if KB_KEY_THRESHOLD_DEFAULT < 0 || KB_KEY_THRESHOLD_DEFAULT > 1023
#error `KB_KEY_THRESHOLD_DEFAULT` value must be [0-1023]
#endif

#if MUX1_KEY_COUNT < 0 || MUX1_KEY_COUNT > 16
#error Too much keys (channels) for MUX1
#endif
#if MUX2_KEY_COUNT < 0 || MUX2_KEY_COUNT > 16
#error Too much keys (channels) for MUX2
#endif
#if MUX3_KEY_COUNT < 0 || MUX3_KEY_COUNT > 16
#error Too much keys (channels) for MUX3
#endif

#if MUX1_KEY_COUNT + MUX2_KEY_COUNT + MUX3_KEY_COUNT != KB_KEY_COUNT
#error Incorrect amount of keys or MUX channels
#endif

#if (!defined(RIGHT) && !defined(LEFT)) || (defined(RIGHT) && defined(LEFT))
#error Either `LEFT` or `RIGHT` must be defined
#endif // !RIGHT && !LEFT

#endif // SETTINGS_H
