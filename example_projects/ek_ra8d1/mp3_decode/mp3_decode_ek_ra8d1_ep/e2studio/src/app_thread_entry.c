/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <app_thread.h>
#include "common_utils.h"
#include "mp3_decode_ep.h"

extern bool g_usb_safely_ejected_flag;
extern FF_Disk_t my_disk;

/***********************************************************************************************************************
 * Function Name: app_thread_entry
 * Description  : Application main thread
 ***********************************************************************************************************************/
void app_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    R_FSP_VersionGet(&version);
    TERM_INIT();

    APP_PRINT(BANNER_INFO,
              EP_VERSION,
              version.version_id_b.major,
              version.version_id_b.minor,
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    init_freertos_plus_fat();

    while (1)
    {
        /* Wait until USB insert or remove event */
        EventBits_t uxBits = xEventGroupWaitBits(g_usb_event_group,
                                                 USB_INSERT_BIT | USB_REMOVE_BIT,
                                                 pdFALSE,
                                                 pdFALSE,
                                                 portMAX_DELAY);

        if (uxBits & USB_INSERT_BIT)
        {
            /* Initialize FreeRTOS+FAT if not yet */
            if (my_disk.xStatus.bIsInitialised == 0U)
            {
                err = usb_init();
                handle_fsp_error(err, "usb_init() failed\r\n");

                APP_PRINT(MINI_MP3_MENU);
            }

            /* Handle user commands */
            if (APP_CHECK_KEY)
            {
                handle_user_input();
            }
        }
        else if (uxBits & USB_REMOVE_BIT)
        {
            /* Check safe removal */
            if (!g_usb_safely_ejected_flag)
            {
                APP_PRINT("\r\nUSB was not safely ejected.\r\n");
                APP_PRINT("Please reinsert the USB drive to safely eject properly.\r\n");

                /* Wait until USB reinserted */
                xEventGroupWaitBits(g_usb_event_group,
                                    USB_INSERT_BIT,
                                    pdFALSE,
                                    pdFALSE,
                                    portMAX_DELAY);
                APP_PRINT(MINI_MP3_MENU);
            }
            else
            {
                /* Mark disk uninitialized for next connection */
                my_disk.xStatus.bIsInitialised = 0U;
            }
        }
        else
        {
            ; /* Do nothing */
        }

        vTaskDelay(pdMS_TO_TICKS(50));  /* Avoid busy waiting */
    }
}
