#include "ipcc.h"

#include "logging.h"

ipcc_handle_t ipcc_handle;

hal_err setup_ipcc() {
    LOG_INFO("Setting up...");

    ipcc_handle.instance = IPCC;

    hal_err err = ipcc_init(&ipcc_handle);
    if (err) {
        LOG_CRITICAL("Unable to init: Error %d", err);
        return err;
    }

    LOG_INFO("Setup complete.");

    return OK;
}
