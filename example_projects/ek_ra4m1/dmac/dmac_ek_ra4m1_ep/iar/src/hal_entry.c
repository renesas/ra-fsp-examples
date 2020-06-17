/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * This file is part of Renesas RA Flex Software Package (FSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

#include "common_utils.h"
#include "dmac_transfers.h"
#include "timer_initialise.h"
#include "transfer_initialise.h"
#include "external_irq_initialise.h"


void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;    /* Variable to help handle error codes from functions*/
    fsp_pack_version_t version = { RESET_VALUE };     /* fsp variable to store major/minor versions*/

    /* Array to store input from RTT terminal */
    uint8_t rtt_input_data[BUFFER_SIZE_DOWN] = { RESET_VALUE };
    uint8_t converted_rtt_input = RESET_VALUE;

    /* Fetch FSP version to display on RTT */
    R_FSP_VersionGet (&version);
    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3,EP_VERSION);
    APP_PRINT(BANNER_4,version.major, version.minor, version.patch );
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);

    APP_PRINT("\r\nProject initializes 3 DMAC tranfser instances, AGT generates 100ms interrupt \
            \rto g_transfer_led_blink. g_transfer_led_blink sends data to LED.\
            \rg_transfer_gpt_value transfer, send's current GPT timer value on every user pushbutton press.\
            \rg_transfer_write_ioport writes data to LED. User can provide input on JlinkRTTViewer\
            \rto displays stored GPT counter value from buffer.\n");

    /* DMAC g_transfer_led_blink (runs in Normal mode) transfers data
     * from source array to port control register(LED) for specified
     * number of counts(60).DMAC g_transfer_led_blink alters the state
     * of the LED and stops. */
    transfer_led_blink_operation();

    /* DMAC g_transfer_gpt_timer is configured to transfer 32-bit data
     * from a GPT counter register to the destination array. Interrupt
     * generated when user pushbutton is pressed, triggers the IRQ and
     * initiates DMAC g_transfer_gpt_timer . GPT timer continuous to run. */
    transfer_gpt_timer_operation();

    /* DMAC g_transfer_write_ioport is configured to transfer 32-bit
     * data(in repeat mode) from the source array to port control
     * register(LED). As a result, pressing the user pushbutton changes
     * the state of the LED.  */
    transfer_write_ioport_operation();

    /* Transfer is no more required as it is used only for writing data on ioport pin */
    dmac_transfer_deinit(&g_transfer_led_blink_ctrl, TRANSFER_LED_BLINK);

    APP_PRINT("\r\n***  Press user push-button to transfer GPT current timer value. ***");
    APP_PRINT("\r\n*** The GPT timer value will be updated in Round-Robin fashion. ***\r\n");
    APP_PRINT("\r\nPress 1 to view the buffer data \n");

    while(true)
    {
        /* Check for input data from RTT terminal, once data is available read. */
        if (APP_CHECK_DATA)
        {
            memset(rtt_input_data, RESET_VALUE , sizeof(rtt_input_data));
            /* Reading complete input instead of 1 byte to avoid processing of
             * successive extra bytes entered */
            APP_READ(rtt_input_data);
            /* Considering both input sending type from RTT and checking for valid input */
            if((NEW_LINE != rtt_input_data[EXPECTED_CHAR_END_INDEX]) && (NULL_CHAR != rtt_input_data[EXPECTED_CHAR_END_INDEX]))
            {
                APP_PRINT("\r\nProvide a valid input. To view the buffer data, provide 1 as RTT input\n");
            }
            else
            {
                converted_rtt_input = (uint8_t)atoi((char *)rtt_input_data);
                if( START_TRANSFER_GPT_VALUE != converted_rtt_input )
                {
                    APP_PRINT("\r\nProvide a valid input. To view the buffer data, provide 1 as RTT input\n");
                }
                else
                {
                    fsp_err = transfer_ioport_write_software_start(converted_rtt_input);
                    /* Handle error in-case of failure */
                    if (FSP_SUCCESS != fsp_err)
                    {
                        dmac_transfer_deinit(&g_transfer_write_ioport_ctrl, TRANSFER_WRITE_IOPORT);
                        gpt_timer_deinit();
                        dmac_transfer_deinit(&g_transfer_gpt_value_ctrl, TRANSFER_GPT_TIMER_VALUE);
                        external_irq_deinit();
                        agt_timer_deinit();
                        APP_ERR_TRAP(fsp_err);
                    }
                    else
                    {
                        dmac_transfer_print_data();
                    }
                }
            }
        }
    }
}


/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}
