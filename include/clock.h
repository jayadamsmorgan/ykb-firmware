#ifndef CLOCK_H
#define CLOCK_H

typedef enum { HSE, HSI } CLOCK_SOURCE;

void select_clock_source(CLOCK_SOURCE source);

unsigned get_sys_freq();

#endif // CLOCK_H
