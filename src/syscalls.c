#include "utils/utils.h"

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

int _close(int file) {
    UNUSED(file);
    return -1;
}

int _lseek(int file, int ptr, int dir) {
    UNUSED(file);
    UNUSED(ptr);
    UNUSED(dir);
    return 0;
}

ATTR_WEAK int _read(int file, char *ptr, int len) {
    UNUSED(file);
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        *ptr++ = __io_getchar();
    }

    return len;
}

ATTR_WEAK int _write(int file, char *ptr, int len) {
    UNUSED(file);
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        __io_putchar(*ptr++);
    }
    return len;
}
