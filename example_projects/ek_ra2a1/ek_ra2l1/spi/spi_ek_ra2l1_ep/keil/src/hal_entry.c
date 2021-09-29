/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "spi_ep.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);
/*******************************************************************************************************************//**
 * @addtogroup r_spi_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 *        called by main() when no RTOS is used.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    unsigned char rByte[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    uint8_t converted_rtt_input = RESET_VALUE;

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3,EP_VERSION);
    APP_PRINT(BANNER_4,version.major, version.minor, version.patch );
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT(BANNER_7);

    /* Initialize SPI driver */
    err = spi_init();

    if (FSP_SUCCESS != err)
    {   /* SPI module init failed */
        APP_PRINT("\r\n ** SPI INIT FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\n ** SPI INIT SUCCESSFUL ** \r\n");
    }

    /* Menu for User Selection */
    APP_PRINT("\r\nSelect from the below Menu options\r\n");
    APP_PRINT("\r\nPress 1 for Write() and Read()");
    APP_PRINT("\r\nPress 2 for WriteRead()");
    APP_PRINT("\r\nPress 3 to Exit\r\n");

    while (EXIT != converted_rtt_input)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte);
            converted_rtt_input = (uint8_t)atoi((char *)rByte);

            switch (converted_rtt_input)
            {
                case WRITE_AND_READ:
                    err = spi_write_and_read ();
                    if (FSP_SUCCESS != err)
                    {
                        APP_PRINT("** SPI WRITE AND READ Demo failed **  \r\n");
                        spi_clean_up();
                        APP_ERR_TRAP(err);
                    }
                    else
                    {
                        APP_PRINT("\r\n** SPI WRITE AND READ Demo Successful**  \r\n");
                    }
                    break;
                case WRITE_READ:
                    err = spi_write_read ();
                    if (FSP_SUCCESS != err)
                    {
                        APP_PRINT("\r\n** SPI WRITE Demo failed **  \r\n");
                        spi_clean_up();
                        APP_ERR_TRAP(err);
                    }
                    else
                    {
                        APP_PRINT("** SPI WRITE AND READ Demo Successful**  \r\n");
                    }
                    break;
                case EXIT:
                    err = spi_exit_demo();
                    if (FSP_SUCCESS != err)
                    {
                        APP_PRINT("** SPI EXIT Demo failed ** \r\n");
                        APP_ERR_TRAP(err);
                    }
                    else
                    {
                        APP_PRINT("** SPI EXIT Demo successful ** \r\n");
                    }
                    break;
                default:
                    /* Menu for User Selection */
                    APP_PRINT("\r\nSelect from the below Menu options\r\n");
                    APP_PRINT("\r\nPress 1 for Write() and Read()");
                    APP_PRINT("\r\nPress 2 for WriteRead()");
                    APP_PRINT("\r\nPress 3 to Exit\r\n");
                    break;
            }
            if((WRITE_AND_READ <= converted_rtt_input) && (EXIT > converted_rtt_input))
            {
                APP_PRINT ("\r\nEnter any other key to go back to the main menu\r\n");
            }
        }
    }

    APP_PRINT(RTT_CTRL_TEXT_BRIGHT_CYAN "\r\nExiting SPI demo....\r\n" RTT_CTRL_RESET);
}

/*******************************************************************************************************************//**
 * @brief This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  Event where at in the start up process the code is currently at
 * @retval      None
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
