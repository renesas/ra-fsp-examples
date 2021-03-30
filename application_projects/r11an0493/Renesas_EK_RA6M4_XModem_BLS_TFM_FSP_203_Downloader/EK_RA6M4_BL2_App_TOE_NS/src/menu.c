/*
 * menu.c
 *
 *  Created on: 2 Oct 2019
 *      Author: b3800274
 */
#include "menu.h"
#include "header.h"
#include "comms/comms.h"
#include "xmodem.h"
#include "downloader_thread.h"
#include "header.h"
#include "psa/error.h"
#include "psa/initial_attestation.h"
#include "psa/internal_trusted_storage.h"
#include "tfm_psa/psa/crypto.h"
#include "hal_data.h"
#include "tfm_ns_interface.h"
#include "tfm_platform_api.h"
#include "tfm_ioctl_api.h"

const char * gp_banner[] = {
                            "  /\\/\\   ___ _ __  _   _",
                            " /    \\ / _ \\ '_ \\| | | |",
                            "/ /\\/\\ \\  __/ | | | |_| |",
                            "\\/    \\/\\___|_| |_|\\__,_|",
};

void menu(void)
{
    volatile fsp_err_t err = 0;
    uint8_t tx_str[80];
    uint8_t rx_str[20];

    uint32_t tfm_err_status;
    uint32_t flash_test_result = 0;

    FSP_PARAMETER_NOT_USED(err);

    sprintf((char *)tx_str, "\n\r");
    comms_send(tx_str, strlen((char *)tx_str));

    for(uint8_t b=0; b<4; b++)
    {
        comms_send((uint8_t *)gp_banner[b], strlen(gp_banner[b]));
        comms_send((uint8_t *)"\n\r", 2);
    }

    sprintf((char *)tx_str, "\n\r1 - Display image slot info\n\r");
    comms_send(tx_str, strlen((char *)tx_str));
    sprintf((char *)tx_str, "2 - Download new Secure image (XModem)\n\r");
    comms_send(tx_str, strlen((char *)tx_str));
    sprintf((char *)tx_str, "3 - Download new Non-Secure image (XModem)\n\r");
    comms_send(tx_str, strlen((char *)tx_str));
    sprintf((char *)tx_str, "4 - Reboot\n\r");
    comms_send(tx_str, strlen((char *)tx_str));
    comms_send((uint8_t *)">", 1);

    rx_str[0] = 0;
    uint32_t len = 1;
    err = comms_read(rx_str, &len, portMAX_DELAY);
    if(err != FSP_SUCCESS)
    {
        __BKPT(1);
    }
    comms_send(rx_str, 1);
    comms_send((uint8_t *)"\n\r", 2);

    switch (rx_str[0])
    {
        case '1':
            display_image_slot_info();
        break;

        case '2':
            /* Download a new secure image in the secondary slot using the XModem protocol */
            /* Erase the secondary slot first */
            /* Check if it needs erasing first */
            sprintf((char *)tx_str, "Blank checking the S secondary slot...\r\n");
            comms_send(tx_str, strlen((char *)tx_str));

            bool blank = true;
            uint8_t * p_src;
            p_src = (uint8_t *) SECONDARY_S_IMAGE_START_ADDRESS;
            do {
                if (*p_src != 0xFF)
                {
                    blank = false;
                }
                p_src++;
            } while ((blank == true) && (p_src < (uint8_t *)SECONDARY_S_IMAGE_END_ADDRESS));

            if(1)
            {
                if (true == blank)
                {
                    sprintf((char *)tx_str, "S Secondary slot blank\r\n");
                    comms_send(tx_str, strlen((char *)tx_str));
                }
                else
                {
                    sprintf((char *)tx_str, "Erasing the S secondary slot...\r\n");
                    comms_send(tx_str, strlen((char *)tx_str));

                    tfm_err_status = tfm_platform_flash_erase((uint32_t) SECONDARY_S_IMAGE_START_ADDRESS, SECONDARY_IMAGE_NUM_BLOCKS, (int32_t *)&flash_test_result);

                    if(TFM_PLATFORM_ERR_SUCCESS == tfm_err_status)
                    {
                        sprintf((char *)tx_str, "Secondary S slot erased\r\n");
                        comms_send(tx_str, strlen((char *)tx_str));
                    }
                    else
                    {
                        sprintf((char *)tx_str, "ERROR: Erasing the S secondary slot\r\n");
                        comms_send(tx_str, strlen((char *)tx_str));
                        sprintf((char *)tx_str, "ERROR: tfm_err_status = %d\r\n", (int)tfm_err_status);
                        comms_send(tx_str, strlen((char *)tx_str));
                        sprintf((char *)tx_str, "ERROR: flash_test_result=%d\r\n", (int)flash_test_result);
                        comms_send(tx_str, strlen((char *)tx_str));
                        break;
                    }
                }

                /* XModem download and flash programming */
                sprintf((char *)tx_str, "Start Xmodem transfer...\r\n");
                comms_send(tx_str, strlen((char *)tx_str));
                unsigned char xm_err = XmodemDownloadAndProgramFlash(SECONDARY_S_IMAGE_START_ADDRESS);
                if(XM_OK == xm_err)
                {
                    sprintf((char *)tx_str, "Flash image download successful\r\n");
                    comms_send(tx_str, strlen((char *)tx_str));
                }
                else
                {
                    switch(xm_err)
                    {
                        case XM_ADDRESS_ERROR:
                            sprintf((char *)tx_str, "ERROR: Flash address invalid\r\n");
                        break;

                        case XM_TIMEOUT:
                            sprintf((char *)tx_str, "ERROR: Timeout during Xmodem download\r\n");
                        break;

                        case XM_PROG_FAIL:
                            sprintf((char *)tx_str, "ERROR: Flash programming error\r\n");
                        break;

                        default:
                            sprintf((char *)tx_str, "ERROR: unknown (%d)\r\n", xm_err);
                        break;
                    }

                    comms_send(tx_str, strlen((char *)tx_str));
                }
            }
            else
            {
                sprintf((char *)tx_str, "ERROR: Blank checking the NS secondary slot\r\n");
                comms_send(tx_str, strlen((char *)tx_str));
            }

        break;

        case '3':
            /* Download a new non-secure image in the secondary slot using the XModem protocol */
            /* Erase the secondary slot first */
            /* Check if it needs erasing first */
            sprintf((char *)tx_str, "Blank checking the NS secondary slot...\r\n");
            comms_send(tx_str, strlen((char *)tx_str));

            blank = true;
            p_src = (uint8_t *) SECONDARY_NS_IMAGE_START_ADDRESS;
            do {
                if (*p_src != 0xFF)
                {
                    blank = false;
                }
                p_src++;
            } while ((blank == true) && (p_src < (uint8_t *)SECONDARY_NS_IMAGE_END_ADDRESS));

            if(1)
            {
                if (true == blank)
                {
                    sprintf((char *)tx_str, "NS Secondary slot blank\r\n");
                    comms_send(tx_str, strlen((char *)tx_str));
                }
                else
                {
                    sprintf((char *)tx_str, "Erasing the NS secondary slot...\r\n");
                    comms_send(tx_str, strlen((char *)tx_str));
                    tfm_err_status = tfm_platform_flash_erase((uint32_t) SECONDARY_NS_IMAGE_START_ADDRESS, SECONDARY_IMAGE_NUM_BLOCKS, (int32_t *)&flash_test_result);
                    if(TFM_PLATFORM_ERR_SUCCESS == tfm_err_status)
                    {
                        sprintf((char *)tx_str, "NS Secondary slot erased\r\n");
                        comms_send(tx_str, strlen((char *)tx_str));
                    }
                    else
                    {
                        sprintf((char *)tx_str, "ERROR: Erasing the NS secondary slot\r\n");
                        comms_send(tx_str, strlen((char *)tx_str));
                        sprintf((char *)tx_str, "ERROR: tfm_err_status = %d\r\n", (int)tfm_err_status);
                        comms_send(tx_str, strlen((char *)tx_str));
                        sprintf((char *)tx_str, "ERROR: flash_test_result=%d\r\n", (int)flash_test_result);
                        comms_send(tx_str, strlen((char *)tx_str));
                        break;
                    }
                }

                /* XModem download and flash programming */
                sprintf((char *)tx_str, "Start Xmodem transfer...\r\n");
                comms_send(tx_str, strlen((char *)tx_str));
                unsigned char xm_err = XmodemDownloadAndProgramFlash(SECONDARY_NS_IMAGE_START_ADDRESS);
                if(XM_OK == xm_err)
                {
                    sprintf((char *)tx_str, "Flash image download successful\r\n");
                    comms_send(tx_str, strlen((char *)tx_str));
                }
                else
                {
                    switch(xm_err)
                    {
                        case XM_ADDRESS_ERROR:
                            sprintf((char *)tx_str, "ERROR: Flash address invalid\r\n");
                        break;

                        case XM_TIMEOUT:
                            sprintf((char *)tx_str, "ERROR: Timeout during Xmodem download\r\n");
                        break;

                        case XM_PROG_FAIL:
                            sprintf((char *)tx_str, "ERROR: Flash programming error\r\n");
                        break;

                        default:
                            sprintf((char *)tx_str, "ERROR: unknown (%d)\r\n", xm_err);
                        break;
                    }

                    comms_send(tx_str, strlen((char *)tx_str));
                }
            }
            else
            {
                sprintf((char *)tx_str, "ERROR: Blank checking the secondary slot\r\n");
                comms_send(tx_str, strlen((char *)tx_str));
            }

        break;

        case '4':
            sprintf((char *)tx_str, "Resetting the device...\r\n");
            comms_send(tx_str, strlen((char *)tx_str));
            tfm_platform_system_reset();
        break;

        default:
        break;
    }
}

void ThreadsAndInterrupts(enable_disable_t EnableDisable)
{
    static uint32_t control_reg_value;
    static uint32_t old_primask;

    if( DISABLE == EnableDisable )
    {
        /** Store the interrupt state */
        old_primask = __get_PRIMASK();

        /* Disable other threads whilst flash erasing */
//        tx_thread_suspend(& _ux_system_host->ux_system_host_hcd_thread );

        /* Disable the SysTick timer */
        control_reg_value = SysTick->CTRL;
        SysTick->CTRL = 0;
        NVIC_DisableIRQ( SysTick_IRQn ); /* Disable the SysTick timer IRQ */
        NVIC_ClearPendingIRQ( SysTick_IRQn ); /* Clear any pending SysTick timer IRQ */

        __disable_irq(); /* Disable interrupts */
    }
    else
    {
        NVIC_EnableIRQ( SysTick_IRQn ); /* Enable the SysTick timer IRQ */
        SysTick->CTRL = control_reg_value; /* Restart the SysTick timer */

        /** Restore the interrupt state */
        __set_PRIMASK( old_primask ); /* Enable interrupts */

        /* Resume the threads */
//        tx_thread_resume(& _ux_system_host->ux_system_host_hcd_thread );
    }
}
