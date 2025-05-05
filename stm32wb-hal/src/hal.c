#include "hal.h"

#include "hal_cortex.h"
#include "hal_flash.h"
#include "hal_systick.h"

hal_err hal_init() {

    hal_err err;

    flash_enable_prefetch();

    cortex_nvic_set_priority_group(NVIC_PRIORITY_GROUP_4);

    err = systick_init();
    if (err) {
        return err;
    }

    return OK;
}
