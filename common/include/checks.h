#ifndef CHECKS_H
#define CHECKS_H

#ifdef BOOTLOADER
#undef LEFT
#define RIGHT
#endif // BOOTLOADER

#ifndef BOOTLOADER
#define ENABLE_DEFAULT_INTERRUPTS 1
#endif // !BOOTLOADER

#if !defined(USB_ENABLED) && !defined(BLUETOOTH_ENABLED)
#error Either USB or BLUETOOTH have to be enabled
#endif // !USB_ENABLED && !BLUETOOTH_ENABLED

#if KB_KEY_THRESHOLD_DEFAULT < 1 || KB_KEY_THRESHOLD_DEFAULT > 100
#error `KB_KEY_THRESHOLD_DEFAULT` value must be a percentage [1-100]
#endif

#ifndef BOOTLOADER
#if (!defined(RIGHT) && !defined(LEFT)) || (defined(RIGHT) && defined(LEFT))
#error Either `LEFT` or `RIGHT` must be defined
#endif // !RIGHT && !LEFT
#endif // !BOOTLOADER

#endif // CHECKS_H
