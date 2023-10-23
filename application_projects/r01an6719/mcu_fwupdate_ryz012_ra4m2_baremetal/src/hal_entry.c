/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Entry function.
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

#include <stdio.h>
#include <string.h>
#include "hal_entry.h"
#include "common_init.h"
#include "common_utils.h"
#include "xmodem.h"
#include "usb_comms.h"


/* Function declaration */
void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* Global variables */
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t *g_apl_string_table[];

const usb_descriptor_t usb_descriptor =
{
 g_apl_device,                   /* Pointer to the device descriptor */
 g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
 g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
 g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
 g_apl_string_table,             /* Pointer to the string descriptor table */
 NUM_STRING_DESCRIPTOR
};

usb_status_t            usb_event;
usb_setup_t             usb_setup;

uint8_t g_usb_module_number = 0x00;
usb_class_t g_usb_class_type    = 0x00;
static bool  b_usb_attach = false;

/* function prototypes */
static fsp_err_t check_for_write_complete(void);
static void xmodem_console(void);
void app_main(void);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize GPT module */
    err = common_init();
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_PRINT("\r\n ** Initialize GPT module : failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Start XMODEM Menu on TeraTerm */
    APP_PRINT("\r\n ** Start RTT Viewer for Logging ** \r\n");
    APP_PRINT("\r\n ** Start Tera Term for XMODEM File Transfer of RYZ012 FW Image to MCU ** \r\n");
    APP_PRINT("\r\n ** Tera Term : New connection->Serial->COMx : USB Serial Device (COMx) ** \r\n");
    APP_PRINT("\r\n ** Hit Enter Key (CR) in Tera Term Window  ** \r\n");
    xmodem_console();

    /* Start the MCU FW update of RYZ012 : Use RTT Viewer for Status Logging */
    APP_PRINT("\r\n ** Start MCU FW Update of RYZ012 ** \r\n");
    app_main();
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


static void xmodem_console(void)
{
    static usb_pcdc_linecoding_t g_line_coding;
    fsp_err_t err = FSP_SUCCESS;
    uint8_t xm_err = XM_TIMEOUT;
    uint8_t g_buf[READ_BUF_SIZE] = {0};
    uint8_t tx_str[80];
    bool run_menu = true;

    /* Open the flash driver */
    err =   R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_PRINT("\r\n ** Open Flash Driver : failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Open USB instance */
    err = R_USB_Open (&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_PRINT("\r\n ** Open USB Instance : failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Get USB class type */
    err = R_USB_ClassTypeGet (&g_basic0_ctrl, &g_usb_class_type);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_PRINT("\r\n ** Get USB class type : failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Get module number */
    err = R_USB_ModuleNumberGet(&g_basic0_ctrl, &g_usb_module_number);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_PRINT("\r\n ** Get USB Module Number : failed ** \r\n");
        APP_ERR_TRAP(err);
    }


    while ( true == run_menu )
    {
        /* Obtain USB related events */
        err = R_USB_EventGet (&g_basic0_ctrl, &usb_event);
        if (FSP_SUCCESS != err)
        {

            APP_ERR_TRAP(err);
        }

        /* USB event received by R_USB_EventGet */
        switch (usb_event)
        {
            case USB_STATUS_CONFIGURED:
            {
                err = R_USB_Read (&g_basic0_ctrl, g_buf, READ_BUF_SIZE, (uint8_t)g_usb_class_type);
                if (FSP_SUCCESS != err)
                {

                    APP_ERR_TRAP(err);
                }
                break;
            }

            case USB_STATUS_READ_COMPLETE:
            {
                if(b_usb_attach)
                {
                    // get Menu choice
                    err = R_USB_Read (&g_basic0_ctrl, g_buf, READ_BUF_SIZE, (uint8_t)g_usb_class_type);
                }
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_TRAP(err);
                }

                /* Switch case evaluation of menu input */
                switch (g_buf[0])
                {
                    case '1':
                    {
                        /* XModem download and flash programming */
                        /* Download a new image in the RYZ012 firmware slot using the XModem protocol */
                        /* Erase the image slot first */
                        /* Check if it needs erasing first */
                        sprintf((char *)tx_str, "\r\nBlank checking the RYZ012 firmware image slot...\r\n");
                        comms_send(tx_str, strlen((char *)tx_str));

                        bool blank = true;
                        uint8_t * p_src;
                        p_src = (uint8_t *) RYZ012_FWIMAGE_START_ADDRESS;
                        do
                        {
                            if (*p_src != 0xFF)
                            {
                                blank = false;
                            }
                            p_src++;
                        } while ((blank == true) && (p_src < (uint8_t *)RYZ012_FWIMAGE_END_ADDRESS));



                        if (true == blank)
                        {
                            sprintf((char *)tx_str, "\r\nRYZ012 firmware image slot blank\r\n");
                            comms_send(tx_str, strlen((char *)tx_str));
                        }
                        else
                        {
                            sprintf((char *)tx_str, "\r\nErasing the RYZ012 firmware image slot...\r\n");
                            comms_send(tx_str, strlen((char *)tx_str));
                            ThreadsAndInterrupts(DISABLE);
                            err = R_FLASH_HP_Erase(&g_flash0_ctrl, (uint32_t) RYZ012_FWIMAGE_START_ADDRESS, RYZ012_FWIMAGE_NUM_BLOCKS);
                            ThreadsAndInterrupts(RE_ENABLE);

                            if(FSP_SUCCESS == err)
                            {
                                sprintf((char *)tx_str, "\r\nRYZ012 firmware image slot erased\r\n");
                                comms_send(tx_str, strlen((char *)tx_str));
                                blank = true;
                            }
                            else
                            {
                                sprintf((char *)tx_str, "\r\nERROR: Erasing the RYZ012 firmware image slot err = 0x%x\r\n", err);
                                comms_send(tx_str, strlen((char *)tx_str));
                                blank = false;
                            }
                        }

                        if (true == blank)
                        {
                            /* Image slot is erased : Proceed with file transfer */
                            sprintf((char *)tx_str, "\r\nStart Xmodem transfer...\r\n");
                            comms_send(tx_str, strlen((char *)tx_str));
                            sprintf((char *)tx_str, "\r\nXmodem send : File->Transfer->XMODEM->Send ...\r\n");
                            comms_send(tx_str, strlen((char *)tx_str));
                            sprintf((char *)tx_str, "\r\nChoose RYZ012 Binary Image File ...\r\n");
                            comms_send(tx_str, strlen((char *)tx_str));
                            sprintf((char *)tx_str, "\r\nSystem will automatically reset after successful download...\r\n");
                            comms_send(tx_str, strlen((char *)tx_str));

                            xm_err = XmodemDownloadAndProgramFlash(RYZ012_FWIMAGE_START_ADDRESS);
                            if(XM_OK != xm_err)
                            {
                                switch(xm_err)
                                {
                                    case XM_ADDRESS_ERROR:
                                        sprintf((char *)tx_str, "ERROR: Flash address invalid\r\n");
                                    break;

                                    case XM_TIMEOUT:
                                        sprintf((char *)tx_str, "ERROR: Timeout waiting on Xmodem download\r\n");
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
                        break;
                    }

                    case CARRIAGE_RETURN:
                    {
                        /* Print Menu OPTIONS to xmodem console */
                        sprintf((char *)tx_str, "\r\nPress 1 RYZ012 File Transfer to MCU, 2 Write Image to RYZ012 \r\n");
                        comms_send(tx_str, strlen((char *)tx_str));
                        if (FSP_SUCCESS != err)
                        {
                            APP_ERR_TRAP(err);
                        }
                        break;
                    }

                    case '2':
                    {
                        /* Exit and Write Image (in MCU Code Flash) to RYZ012 */
                        run_menu = false;
                        sprintf((char *)tx_str, "\r\nWriting Image to RYZ012\r\n");
                        comms_send(tx_str, strlen((char *)tx_str));
                        sprintf((char *)tx_str, "\r\nWaiting....Do not Remove Power or Reset MCU - RYZ012 FW updating...\r\n");
                        comms_send(tx_str, strlen((char *)tx_str));
                        if (FSP_SUCCESS != err)
                        {
                            APP_ERR_TRAP(err);
                        }
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
                break;
            }

            case USB_STATUS_REQUEST : /* Receive Class Request */
            {
                R_USB_SetupGet(&g_basic0_ctrl, &usb_setup);

                /* Check for the specific CDC class request IDs */
                if (USB_PCDC_SET_LINE_CODING == (usb_setup.request_type & USB_BREQUEST))
                {
                    err =  R_USB_PeriControlDataGet (&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH );
                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_TRAP(err);
                    }
                }
                else if (USB_PCDC_GET_LINE_CODING == (usb_setup.request_type & USB_BREQUEST))
                {
                    err =  R_USB_PeriControlDataSet (&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH );
                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_TRAP(err);
                    }
                }
                else if (USB_PCDC_SET_CONTROL_LINE_STATE == (usb_setup.request_type & USB_BREQUEST))
                {
                    err = R_USB_PeriControlStatusSet (&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
                    /* Handle error */
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_TRAP(err);
                    }
                }
                else
                {
                    /* none */
                }

                break;
            }

            case USB_STATUS_DETACH:
            case USB_STATUS_SUSPEND:
            {
                b_usb_attach = false;
                memset (g_buf, 0, sizeof(g_buf));
                break;
            }
            case USB_STATUS_RESUME:
            {
                b_usb_attach = true;
                break;
            }
            default:
            {
                break;
            }
        }
    }

    /* Close USB instance */
    err = R_USB_Close (&g_basic0_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_PRINT("\r\n ** Close USB instance : failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Close the flash driver */
    err =   R_FLASH_HP_Close(&g_flash0_ctrl);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
        APP_ERR_PRINT("\r\n ** Close Flash Driver : failed ** \r\n");
        APP_ERR_TRAP(err);
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
    }
}


/*****************************************************************************************************************
 *  @brief      Check for write completion
 *  @param[in]  None
 *  @retval     FSP_SUCCESS     Upon success
 *  @retval     any other error code apart from FSP_SUCCESS
 ****************************************************************************************************************/
static fsp_err_t check_for_write_complete(void)
{
    usb_status_t usb_write_event = USB_STATUS_NONE;
    int32_t timeout_count = UINT16_MAX;
    fsp_err_t err = FSP_SUCCESS;

    do
    {
        err = R_USB_EventGet (&g_basic0_ctrl, &usb_write_event);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        --timeout_count;
        if (0 > timeout_count)
        {
            timeout_count = 0;
            err = (fsp_err_t)USB_STATUS_NONE;
            break;
        }
    }while(USB_STATUS_WRITE_COMPLETE != usb_write_event);

    return err;
}


/*******************************************************************************************************************//**
 * @brief This function writes the USB data
 * @param[IN]   p_src               destination data buffer
 * @param[IN]   len                     length of the data
 * @retval      FSP_SUCCESS             Upon successful operations.
 * @retval      Any Other Error code    Upon unsuccessful operations.
 **********************************************************************************************************************/
fsp_err_t comms_send(uint8_t * p_src, uint32_t len)
{
    fsp_err_t err;

    err = R_USB_Write(&g_basic0_ctrl, p_src, len, (uint8_t)g_usb_class_type);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for the USB write to complete */
    err = check_for_write_complete();
    if (FSP_SUCCESS != err)
    {
        /* Did not get the event returning error */
        return FSP_ERR_USB_FAILED;
    }
    return err;
}


/*******************************************************************************************************************//**
 * @brief This function reads the USB data with timeout control
 * @param[IN]   p_dest                  destination data buffer
 * @param[IN]   len                     length of the data
 * @param[IN]   timeout_milliseconds    timeout in milliseconds
 * @retval      FSP_SUCCESS             Upon successful operations.
 * @retval      Any Other Error code    Upon unsuccessful operations.
 **********************************************************************************************************************/
fsp_err_t comms_read(uint8_t * p_dest, uint32_t * len, uint32_t timeout_milliseconds)
{
    fsp_err_t err = FSP_SUCCESS;
    usb_status_t usb_read_event = USB_STATUS_NONE;
    uint32_t timeout_count;

    timeout_count = timeout_milliseconds;

    err = R_USB_Read(&g_basic0_ctrl, p_dest, *len, g_usb_class_type);

    while ( timeout_count && (usb_read_event != USB_STATUS_READ_COMPLETE) )
    {
        /* Obtain USB related events */
        err = R_USB_EventGet (&g_basic0_ctrl, &usb_read_event);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_count -= 1;
        if (0 >= timeout_count)
        {
            timeout_count = 0;
            err = FSP_ERR_TIMEOUT;
            break;
        }

        /* USB event received by R_USB_EventGet */
        switch (usb_read_event)
        {
            case USB_STATUS_READ_COMPLETE:
            {
                ;
                break;
            }

            default:
            {
                break;
            }
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup hal_entry)
 **********************************************************************************************************************/

