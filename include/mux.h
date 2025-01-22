#ifndef MUX_H
#define MUX_H

#include <stdbool.h>
#include <stdint.h>

#define HIGH true
#define LOW false

#ifndef PIN_MUX_0
#define PIN_MUX_0 0
#endif // PIN_MUX_0

#ifndef PIN_MUX_1
#define PIN_MUX_1 1
#endif // PIN_MUX_1

#ifndef PIN_MUX_2
#define PIN_MUX_2 2
#endif // PIN_MUX_2

#ifndef PIN_MUX_CHAN_A
#define PIN_MUX_CHAN_A 3
#endif // PIN_MUX_CHAN_A

#ifndef PIN_MUX_CHAN_B
#define PIN_MUX_CHAN_B 4
#endif // PIN_MUX_CHAN_B

#ifndef PIN_MUX_CHAN_C
#define PIN_MUX_CHAN_C 5
#endif // PIN_MUX_CHAN_C

#ifndef PIN_MUX_CHAN_D
#define PIN_MUX_CHAN_D 6
#endif // PIN_MUX_CHAN_D

#ifndef PIN_MUX_INPUT
#define PIN_MUX_INPUT 7
#endif // PIN_MUX_INPUT

// Initialize multiplexors.
void mux_init();

// Select active multiplexor with MOSFETs
void mux_select(uint8_t multiplexor);

// Select active channel on active multiplexor
void mux_select_chan(uint8_t channel);

// Read from active channel
uint8_t mux_read();

#endif // MUX_H
