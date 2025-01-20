#include "system.h"

void spin(volatile uint32_t count) {
    while (count--)
        asm("nop");
}
