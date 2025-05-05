#ifndef VERSION_H
#define VERSION_H

#include "utils/utils.h"

#ifdef LEFT
#define YKB_FW_VERSION_PRE "L-"
#endif // LEFT
#ifdef RIGHT
#define YKB_FW_VERSION_PRE "R-"
#endif // RIGHT

#define YKB_FW_VERSION_MAJOR 0
#define YKB_FW_VERSION_MINOR 1
#define YKB_FW_VERSION_PATCH 0

#define YKB_FW_VERSION_BASE                                                    \
    YKB_FW_VERSION_PRE STR(YKB_FW_VERSION_MAJOR) "." STR(                      \
        YKB_FW_VERSION_MINOR) "." STR(YKB_FW_VERSION_PATCH)

#ifdef DEBUG
#define YKB_FW_VERSION YKB_FW_VERSION_BASE "-" GIT_HASH
#else
// Release version
#define YKB_FW_VERSION YKB_FW_VERSION_BASE
#endif // DEBUG

#define YKB_FW_VERSION_PACKED                                                  \
    ((YKB_FW_VERSION_MAJOR << 16) | (YKB_FW_VERSION_MINOR << 8) |              \
     (YKB_FW_VERSION_PATCH))

#endif // VERSION_H
