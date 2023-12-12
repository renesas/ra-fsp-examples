/***********************************************************************************************************************
 * File Name    : pcdc_acm_thread_entry.c
 * Description  : Contains macros and functions used in pcdc_acm_thread_entry.c
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

#include "pcdc_acm_thread.h"
#include "ux_api.h"
#include "ux_device_class_cdc_acm.h"
#include "usbx_pcdc_acm_ep.h"
#include "common_utils.h"
/*******************************************************************************************************************//**
 * @addtogroup ubsx_pcdc_acm_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
    Macro definitions
 ******************************************************************************/
#define CDCACM_FLAG                         ((ULONG) 0x0001)
#define CDCACM_ACTIVATE_FLAG                ((ULONG) 0x0004)
#define CDCACM_DEACTIVATE_FLAG              ((ULONG) 0x0008)
/******************************************************************************
 Exported global variables and functions (to be accessed by other files)
 ******************************************************************************/
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_device_framework_hi_speed[];
extern uint8_t g_language_id_framework[];
extern uint8_t g_string_framework[];
extern uint32_t usb_peri_usbx_initialize(uint32_t dcd_io);

/******************************************************************************
 Global functions and variables
 ******************************************************************************/
UINT usbx_status_callback (ULONG status);

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static void ux_cdc_device0_instance_activate(void * cdc_instance);
static void ux_cdc_device0_instance_deactivate(void * cdc_instance);
static void usb_connection_status_check(void);
static void usbx_pcdc_operations(void);
/* Mempool size of 18k is required for USBX device class pre built libraries
 * and it is valid only if it with default USBX configurations. */
static uint32_t g_ux_pool_memory[MEMPOOL_SIZE / BYTE_SIZE];
static UX_SLAVE_CLASS_CDC_ACM_PARAMETER g_ux_device_class_cdc_acm0_parameter;
static UX_SLAVE_CLASS_CDC_ACM * g_cdc;
static ULONG g_actual_length;
static uint8_t g_buf[DATA_LEN];
static bool b_print_status = false;


/* PCDC ACM Thread entry function */
void pcdc_acm_thread_entry(void)
{
    /* To check ux api return status */
    UINT status = UX_SUCCESS;
    /* To check fsp api return status */
    fsp_err_t err = FSP_SUCCESS;
    ULONG actual_flags = 0x0000;
    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();

    /* print the banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* ux_system_initialization */
    status = ux_system_initialize(g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("_ux_system_initialize api failed..");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_system initialized successfully!");

    /* ux_device stack initialization */
    status =     ux_device_stack_initialize(g_device_framework_hi_speed,
                                            DEVICE_FRAME_LENGTH_HIGH_SPEED,
                                             g_device_framework_full_speed,
                                             DEVICE_FRAME_LENGTH_FULL_SPEED,
                                             g_string_framework,
                                             STRING_FRAMEWORK_LENGTH,
                                             g_language_id_framework,
                                             LANGUAGE_ID_FRAME_WORK_LENGTH,
                                             &usbx_status_callback);
    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_initialize api failed..");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_device stack initialized successfully!");

    /* The activate command is used when the host has sent a SET_CONFIGURATION command
     and this interface has to be mounted. Both Bulk endpoints have to be mounted
     and the cdc_acm thread needs to be activated.  */
    g_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_instance_activate = ux_cdc_device0_instance_activate;

    /* The deactivate command is used when the device has been extracted.
     The device endpoints have to be dismounted and the cdc_acm thread canceled.  */
    g_ux_device_class_cdc_acm0_parameter.ux_slave_class_cdc_acm_instance_deactivate = ux_cdc_device0_instance_deactivate;

    /* ux_device stack class registration */
    status = ux_device_stack_class_register (_ux_system_slave_class_cdc_acm_name, _ux_device_class_cdc_acm_entry, CONFIG_NUMB,
                                             INTERFACE_NUMB0, (void*) &g_ux_device_class_cdc_acm0_parameter);
    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register api failed..");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("ux_device stack class registered successfully!");
    /* Initialize the peripheral mode according to the USB speed selection */
    if (USB_SPEED_FS == g_basic0_cfg.usb_speed)
    {
        /* Peri mode initialization with Full Speed */
        status = usb_peri_usbx_initialize (R_USB_FS0_BASE);
    }
    else if (USB_SPEED_HS == g_basic0_cfg.usb_speed)
    {
        /* Peri mode initialization with HIGH Speed */
        status = usb_peri_usbx_initialize (R_USB_HS0_BASE);
    }
    else
    {
        /* do nothing */
    }
    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("usb_peri_usbx_initialize api failed..");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("usb_peri_usbx initialized successfully!");

    /* Open usb driver */
    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    /* Error Handle */
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_USB_OPEN api failed..");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("USB driver opened successfully!");

    PRINT_INFO_STR("To verify the usbx_pcdc operations, open the serial terminal and check the status");
    /* wait for enumeration event */
    status = tx_event_flags_get(&g_cdcacm_event_flags0, CDCACM_ACTIVATE_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);
    if((actual_flags & CDCACM_ACTIVATE_FLAG) && (TX_SUCCESS == status))
    {
        PRINT_INFO_STR("USB Enumeration completed!");
    }
    else if(!(actual_flags & CDCACM_ACTIVATE_FLAG))
    {
        PRINT_INFO_STR("USB instance deactivated");
    }

    /* usb pcdc operations will echo the user input on serial terminal*/
    while (true)
    {
        usbx_pcdc_operations ();
        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief     In this function, checks the USB device status and notifies the user by printing the status message
 * @param[IN] none
 * @retval    none
 **********************************************************************************************************************/
static void usb_connection_status_check(void)
{
    ULONG actual_flags = RESET_VALUE;
    /* wait for usb connection event */
    tx_event_flags_get (&g_cdcacm_event_flags0, CDCACM_FLAG, TX_OR, &actual_flags, TX_WAIT_FOREVER);
    if(actual_flags & CDCACM_FLAG)
    {

        if (true != b_print_status)
        {
            PRINT_INFO_STR("Status of USB Device is Attached");
            b_print_status = true;    //flag is updated
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        b_print_status = false; //clear the flag

        PRINT_INFO_STR("Status of USB Device is removed");
        while (!(actual_flags & CDCACM_FLAG))
        {
            ;/* wait until the event update */
        }
    }

}

/*******************************************************************************************************************//**
 * @brief     In this function, it performs the usb write/read operation and echo back the user input on serial terminal
 * @param[IN] none
 * @retval    none
 **********************************************************************************************************************/
static void usbx_pcdc_operations(void)
{
    UINT status = UX_SUCCESS;
    uint32_t data_size = RESET_VALUE;
    volatile UX_SLAVE_DEVICE *device;
    device = &_ux_system_slave->ux_system_slave_device;

    /* Verify the status of usb */
    usb_connection_status_check ();

    /* Wait until usb device is configured to slave */
    while (device->ux_slave_device_state != UX_DEVICE_CONFIGURED)
    {
        ;/* wait until the slave device state is configured. */
    }

    /* USB writes the display message on serial terminal */
    status = ux_device_class_cdc_acm_write (g_cdc, (UCHAR*) "\r\nEnter any Key to echo back the entered data \r\nUser Input :", WRITE_DATA_LEN,
            &g_actual_length);
    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_class_cdc_acm_write api failed..");
        ERROR_TRAP(status);
    }

    /* Clear the buffer */
    memset (g_buf, RESET_VALUE, sizeof(g_buf));

    /* USB Reads the input data from the user from serial terminal */
    status = ux_device_class_cdc_acm_read (g_cdc, g_buf, DATA_LEN, &g_actual_length);
    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_class_cdc_acm_read api failed..");
        ERROR_TRAP(status);
    }
    /* update the data length from the read input */
    data_size = g_actual_length;
    /* Write back the read data on to the serial terminal */
    status = ux_device_class_cdc_acm_write (g_cdc, g_buf, data_size, &g_actual_length);
    /* Error Handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_class_cdc_acm_write api failed..");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief     In this function, it activates the cdc instance.
 * @param[IN] cdc_instance    Pointer to the area store the instance pointer
 * @retval    none
 **********************************************************************************************************************/
static void ux_cdc_device0_instance_activate (void * cdc_instance)
{
    /* Save the CDC instance.  */
    g_cdc = (UX_SLAVE_CLASS_CDC_ACM *) cdc_instance;
    tx_event_flags_set(&g_cdcacm_event_flags0, CDCACM_ACTIVATE_FLAG, TX_OR);
}

/*******************************************************************************************************************//**
 * @brief     In this function, it deactivates the cdc instance.
 * @param[IN] cdc_instance    Pointer to area store the instance pointer
 * @retval    none
 **********************************************************************************************************************/
static void ux_cdc_device0_instance_deactivate (void * cdc_instance)
{
    FSP_PARAMETER_NOT_USED(cdc_instance);
    g_cdc = UX_NULL;
    tx_event_flags_set(&g_cdcacm_event_flags0, CDCACM_DEACTIVATE_FLAG, TX_OR);
}

/*******************************************************************************************************************//**
 * @brief     In this function, usb callback events will be captured into one variable.
 * @param[IN] status    Whenever any event occurred, status gets update
 * @retval    zero
 **********************************************************************************************************************/
UINT usbx_status_callback (ULONG status)
{
    switch (status)
    {
        case UX_DEVICE_ATTACHED:
        {
            tx_event_flags_set(&g_cdcacm_event_flags0, CDCACM_FLAG, TX_OR);
        }
        break;

        case UX_DEVICE_REMOVED:
        {
            tx_event_flags_set(&g_cdcacm_event_flags0, ~CDCACM_FLAG, TX_AND);
        }
        break;

        default:
        {
            /* do nothing */
        }
        break;
    }
    return 0;
}
