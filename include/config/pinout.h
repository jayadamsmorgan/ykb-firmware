#ifndef CONFIG_PINOUT_H
#define CONFIG_PINOUT_H

#include "hal/gpio.h"

#ifndef PIN_LED_DBG
#define PIN_LED_DBG PB0
#endif // !PIN_LED_DBG

#ifndef PIN_MUX1_IN
#define PIN_MUX1_IN PA0
#endif // !PIN_MUX1_IN

#ifndef PIN_MUX1_A
#define PIN_MUX1_A PB2
#endif // !PIN_MUX1_A

#ifndef PIN_MUX1_B
#define PIN_MUX1_B PA4
#endif // !PIN_MUX1_B

#ifndef PIN_MUX1_C
#define PIN_MUX1_C PB4
#endif // !PIN_MUX1_C

#ifndef PIN_MUX1_D
#define PIN_MUX1_D PB5
#endif // !PIN_MUX1_D

#ifndef PIN_MUX2_IN
#define PIN_MUX2_IN PA1
#endif // !PIN_MUX2_IN

#ifndef PIN_MUX2_A
#define PIN_MUX2_A PB6
#endif // !PIN_MUX2_A

#ifndef PIN_MUX2_B
#define PIN_MUX2_B PB7
#endif // !PIN_MUX2_B

#ifndef PIN_MUX2_C
#define PIN_MUX2_C PB8
#endif // !PIN_MUX2_C

#ifndef PIN_MUX2_D
#define PIN_MUX2_D PB9
#endif // !PIN_MUX2_D

#ifndef PIN_MUX3_IN
#define PIN_MUX3_IN PA2
#endif // !PIN_MUX3_IN

#ifndef PIN_MUX3_A
#define PIN_MUX3_A PA10
#endif // !PIN_MUX3_A

#ifndef PIN_MUX3_B
#define PIN_MUX3_B PE4
#endif // !PIN_MUX3_B

#ifndef PIN_MUX3_C
#define PIN_MUX3_C PA15
#endif // !PIN_MUX3_C

#ifndef PIN_MUX3_D
#define PIN_MUX3_D PB1
#endif // !PIN_MUX3_D

#ifndef PIN_SK6812_LED
#define PIN_SK6812_LED PA3
#endif // !PIN_SK6812_LED

#ifndef PIN_USB_DMINUS
#define PIN_USB_DMINUS PA11
#endif // !PIN_USB_DMINUS

#ifndef PIN_USB_DPLUS
#define PIN_USB_DPLUS PA12
#endif // !PIN_USB_DPLUS

#endif // CONFIG_PINOUT_H
