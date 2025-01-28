extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

int _close(int) { return -1; }

int _lseek(int, int, int) { return 0; }

__attribute__((weak)) int _read(int, char *ptr, int len) {
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        *ptr++ = __io_getchar();
    }

    return len;
}

__attribute__((weak)) int _write(int, char *ptr, int len) {
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        __io_putchar(*ptr++);
    }
    return len;
}
