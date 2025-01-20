#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

#define BIT(X) (1UL << X)
#define BITS2(X1, X2) ((1UL << X1) | (1UL << X2))
#define BITS3(X1, X2, X3) ((1UL << X1) | (1UL << X2) | (1UL << X3))

void spin(volatile uint32_t count);

#endif // SYSTEM_H
