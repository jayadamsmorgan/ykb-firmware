#ifndef CLOCK_H
#define CLOCK_H

#include "hal/hal_err.h"

// Use HSE and the PLL to get a 64MHz SYSCLK
hal_err setup_clock();

#endif // CLOCK_H
