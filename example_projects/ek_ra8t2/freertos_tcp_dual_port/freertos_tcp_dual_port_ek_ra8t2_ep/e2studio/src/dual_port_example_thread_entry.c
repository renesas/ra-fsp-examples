#include <dual_port_example.h>
#include "common_utils.h"

void dual_port_example_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_pack_version_t version = {RESET_VALUE};

    FSP_PARAMETER_NOT_USED(pvParameters);

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    TERM_INIT();

    /* Example project information printed on the RTT */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);

    APP_PRINT(EP_INFO);

    vInitializeNetworkInterfaces();

    while (1)
    {
        vTaskDelay (1);
    }
}



