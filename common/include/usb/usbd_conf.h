#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

#include "stm32wbxx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------- -----------*/
#define USBD_MAX_NUM_INTERFACES 2U
/*---------- -----------*/
#define USBD_MAX_NUM_CONFIGURATION 1U
/*---------- -----------*/
#define USBD_MAX_STR_DESC_SIZ 64U
/*---------- -----------*/
#define USBD_DEBUG_LEVEL 0U
/*---------- -----------*/
#define USBD_LPM_ENABLED 0U
/*---------- -----------*/
#define USBD_SELF_POWERED 1U
/*---------- -----------*/
#define HID_FS_BINTERVAL 0xAU

/****************************************/
/* #define for FS and HS identification */
#define DEVICE_FS 0

/** Alias for memory allocation. */
#define USBD_malloc (void *)USBD_static_malloc

/** Alias for memory release. */
#define USBD_free USBD_static_free

/** Alias for memory set. */
#define USBD_memset memset

/** Alias for memory copy. */
#define USBD_memcpy memcpy

/** Alias for delay. */
#define USBD_Delay systick_delay
/* DEBUG macros */

#define USBD_UsrLog(...)

#define USBD_ErrLog(...)

#define USBD_DbgLog(...)

/* Exported functions -------------------------------------------------------*/
void *USBD_static_malloc(uint32_t size);
void USBD_static_free(void *p);

#endif /* __USBD_CONF__H__ */
