#ifndef BL_VERSION_H
#define BL_VERSION_H

#include "utils/utils.h"

#define YKB_BL_FW_VERSION_MAJOR 0
#define YKB_BL_FW_VERSION_MINOR 1
#define YKB_BL_FW_VERSION_PATCH 0

#define YKB_BL_FW_VERSION_BASE                                                 \
    STR(YKB_BL_FW_VERSION_MAJOR)                                               \
    "." STR(YKB_BL_FW_VERSION_MINOR) "." STR(YKB_BL_FW_VERSION_PATCH)

#ifdef DEBUG
#define YKB_BL_FW_VERSION YKB_BL_FW_VERSION_BASE "-" GIT_HASH
#else
// Release version
#define YKB_BL_FW_VERSION YKB_BL_FW_VERSION_BASE
#endif // DEBUG

#define YKB_BL_FW_VERSION_PACKED                                               \
    ((YKB_BL_FW_VERSION_MAJOR << 16) | (YKB_BL_FW_VERSION_MINOR << 8) |        \
     (YKB_BL_FW_VERSION_PATCH))

#endif // BL_VERSION_H
