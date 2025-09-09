/***********************************************************************************************************************
 * File Name    : usb_typec.c
 * Description  : Contains data structures and functions for usb typeC application.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include <stdio.h>
#include "usb_typec.h"
#include <stdarg.h>

static fsp_err_t adc_init(void);
static fsp_err_t adc_read(adc_channel_t adc_channel, uint16_t * adc_data);
static fsp_err_t adc_wait_for_scan_complete(void);
static void usb_print(const char *fmt, ...);
static void error_handler(fsp_err_t error_code);

int g_VbusMonitor_VsenseData;
int g_VbusMonitor_IsenseData;
volatile bool g_typec_callback_flg = true;
volatile bool g_tx_flag  = false;
volatile bool g_rx_flag  = false;
volatile bool g_err_flag = false;
volatile bool g_operation_completed_flag = false;
volatile bool g_usb_unattached_flag = true;
usb_typec_callback_args_t g_typec_args;
extern bsp_leds_t g_bsp_leds;                    /* LED Structure used to blink on board LED */

/* CC (Configuration Channel) Plug Status */
static const char * const g_cc_plug_status[CC_PLUG_STATUS_COUNT] =
{
    "CC1",
    "CC2"
};

/* CC Connection Status Descriptions */
static const char * const g_cc_connection_status[CC_CONNECTION_STATUS_COUNT] =
{
    "Disabled",
    "Unattached",
    "Unattached Wait",
    "Attached",
    "Power Default SNK",
    "Power 1.5A SNK",
    "Power 3.0A SNK"
};

/***********************************************************************************************************************
 * @brief Initializes and runs the USB Type-C and VBUS monitoring task.
 *
 **********************************************************************************************************************/
void usb_typec_entry(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;
    uint16_t adc_voltage_raw = 0;
    uint16_t adc_current_raw = 0;

    /* Initialize USB Type-C driver */
    fsp_err = R_USB_TYPEC_Open(&g_usbcc0_ctrl, &g_usbcc0_cfg);
    error_handler(fsp_err);

    /* Initialize USB CDC communication interface */
    fsp_err = RM_COMMS_USB_PCDC_Open(&g_comms_usb_pcdc0_ctrl, &g_comms_usb_pcdc0_cfg);
    error_handler(fsp_err);

    /* EK-RA2L2 ExRd1_En(P109) = L */
    R_IOPORT_PinCfg(&g_ioport_ctrl,
                    BSP_IO_PORT_01_PIN_09,
                    (uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT |
                    (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW);
    /* EK-RA2L2 ExRd2_En(P110) = L */
    R_IOPORT_PinCfg(&g_ioport_ctrl,
                    BSP_IO_PORT_01_PIN_10,
                    (uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT |
                    (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW);

    /* Initialize ADC module */
    fsp_err = adc_init();
    error_handler(fsp_err);

    while (true)
    {

        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

        /* Read voltage on AN007 (VBUS/2) */
        fsp_err = adc_read(ADC_CHANNEL_7, &adc_voltage_raw);
        error_handler(fsp_err);

        /* Read current sense output on AN008 */
        fsp_err = adc_read(ADC_CHANNEL_8, &adc_current_raw);
        error_handler(fsp_err);

        /* Convert ADC value to VBUS in millivolts (VBUS is divided by 2 externally) */
        g_VbusMonitor_VsenseData = (int)((adc_voltage_raw * VREF_mV * 2.0f) / ADC_MAX_COUNTS);

        /* Convert ADC value to shunt voltage, then to current in mA */
        float v_shunt_mV = (adc_current_raw * VREF_mV) / ADC_MAX_COUNTS;
        g_VbusMonitor_IsenseData = (int) ((v_shunt_mV / ISL28006_GAIN) / SHUNT_OHMS);

        /* Print monitoring result via USB CDC */
        usb_print ("\033[2J\033[H"
                   "----------------------------------------\r\n"
                   "          USB Type-C Information        \r\n"
                   "----------------------------------------\r\n"
                   " CC Plug Status        : %s\r\n"
                   " CC Connection Status  : %s\r\n"
                   " VBUS Voltage          : %u mV\r\n"
                   " VBUS Current          : %u mA\r\n"
                   "----------------------------------------\r\n",
                   g_cc_plug_status[g_typec_args.plug], g_cc_connection_status[g_typec_args.connection_status],
                   g_VbusMonitor_VsenseData, g_VbusMonitor_IsenseData);
    }

}

/***********************************************************************************************************************
 * @brief   Initializes the ADC module and configures the scan settings.
 * @retval  FSP_SUCCESS         ADC initialized successfully.
 * @retval  Any other error     Failed to initialize or configure ADC.
 **********************************************************************************************************************/
static fsp_err_t adc_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open the ADC module */
    fsp_err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Configure the ADC scan channels */
    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    return fsp_err;
}

/***********************************************************************************************************************
 * @brief   Reads a value from the specified ADC channel.
 * @param[in]  adc_channel  The ADC channel to read from.
 * @param[out] adc_data     Pointer to store the read ADC value.
 * @retval     FSP_SUCCESS  Upon successful read.
 * @retval     Others       If any ADC operation fails.
 **********************************************************************************************************************/
static fsp_err_t adc_read(adc_channel_t adc_channel, uint16_t * adc_data)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Start ADC scan */
    fsp_err = R_ADC_ScanStart(&g_adc_ctrl);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Wait for scan to complete */
    fsp_err = adc_wait_for_scan_complete();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Read ADC value from the specified channel */
    fsp_err = R_ADC_Read(&g_adc_ctrl, adc_channel, adc_data);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    return fsp_err;
}

/***********************************************************************************************************************
 * @brief Waits for ADC scan to complete by polling status.
 *
 * @retval FSP_SUCCESS         Scan completed successfully.
 * @retval FSP_ERR_TIMEOUT     Timeout waiting for scan.
 * @retval Others              Error from R_ADC_StatusGet().
 **********************************************************************************************************************/
static fsp_err_t adc_wait_for_scan_complete(void)
{
    adc_status_t status = { .state = ADC_STATE_SCAN_IN_PROGRESS };
    fsp_err_t fsp_err = FSP_SUCCESS;
    uint32_t retry = 0;

    while (ADC_STATE_SCAN_IN_PROGRESS == status.state)
    {
        fsp_err = R_ADC_StatusGet(&g_adc_ctrl, &status);
        if (fsp_err != FSP_SUCCESS)
        {
            return fsp_err;
        }

        if (++retry > ADC_STATUS_RETRY_MAX)
        {
            return FSP_ERR_TIMEOUT;
        }
    }

    return fsp_err;
}

/***********************************************************************************************************************
 * @brief Prints a formatted message over USB CDC.
 *
 * @param[in] fmt The format string (e.g., "Value: %d").
 * @param[in] ... Variable number of arguments for the format string.
 * @retval    None.
 **********************************************************************************************************************/
static void usb_print(const char *fmt, ...)
{
    /* Use a static buffer to avoid stack overflow with large messages */
    static char msg_buffer[1024];
    va_list args;
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* If USB is not attached, do nothing */
    if (g_usb_unattached_flag)
    {
        return;
    }

    /* Format the message using the variable arguments */
    va_start(args, fmt);
    int len = vsnprintf(msg_buffer, sizeof(msg_buffer), fmt, args);
    va_end(args);

    /* Check if the message was successfully formatted and fits in the buffer */
    if (len <= 0)
    {
        return;
    }

    /* Start USB CDC transmission */
    g_tx_flag  = false;
    fsp_err = RM_COMMS_USB_PCDC_Write(&g_comms_usb_pcdc0_ctrl, (uint8_t *)msg_buffer, (uint32_t)len);
    if (FSP_SUCCESS != fsp_err)
    {
        /* Current skip to allow for attached and unattached USB testing. */
    }

    /* Wait for the transmission to complete, or exit early if the device is detached */
    while (!g_tx_flag)
    {
        if (g_usb_unattached_flag)
        {
            break;
        }
    }
}

/***********************************************************************************************************************
 * @brief Callback function for USB Type-C event notifications.
 *
 * @param[in] p_typec_args     Pointer to the callback event data.
 **********************************************************************************************************************/
void usb_typec_callback(usb_typec_callback_args_t * p_typec_args)
{
    if (p_typec_args == NULL)
    {
        return;
    }

    /* Store callback arguments */
    g_typec_args = *p_typec_args;

    /* Update the connection flags based on the connection status. */
    switch (g_typec_args.connection_status)
    {
        case USB_TYPEC_CONNECTION_STATUS_DISABLED:
        case USB_TYPEC_CONNECTION_STATUS_UNATTACHED:
        {
            /* USB Type-C Unattached */
            g_usb_unattached_flag = true;
            break;
        }
        case USB_TYPEC_CONNECTION_STATUS_ATTACHED:
        case USB_TYPEC_CONNECTION_STATUS_ATTACHED_POWER_DEFAULT:
        case USB_TYPEC_CONNECTION_STATUS_ATTACHED_POWER_1_5:
        case USB_TYPEC_CONNECTION_STATUS_ATTACHED_POWER_3_0:
        {
            /* USB Type-C Attached */
            g_usb_unattached_flag = false;
            break;
        }
        default:
        {
            break;
        }
    }
}

/***********************************************************************************************************************
 * @brief  Callback function for USB PCDC communication events.
 *         Handles transmission, reception, and error events.
 *
 * @param[in] p_args    Pointer to the callback argument structure containing the event type.
 **********************************************************************************************************************/
void rm_comms_usb_pcdc_callback (rm_comms_callback_args_t * p_args)
{
    if (NULL == p_args)
    {
        return;
    }

    switch (p_args->event)
    {
        case RM_COMMS_EVENT_OPERATION_COMPLETE:
            g_operation_completed_flag = true;
            break;

        case RM_COMMS_EVENT_TX_OPERATION_COMPLETE:
            g_tx_flag = true;
            break;

        case RM_COMMS_EVENT_RX_OPERATION_COMPLETE:
            g_rx_flag = true;
            break;

        case RM_COMMS_EVENT_ERROR:
        default:
            g_err_flag = true;
            break;
    }
}

/***********************************************************************************************************************
 * @brief Handles application-specific errors by lighting an LED and trapping the system.
 *
 * @param[in] error_code The FSP error code to be evaluated.
 * @retval    None.
 **********************************************************************************************************************/
void error_handler(fsp_err_t error_code)
{
    if (FSP_SUCCESS != error_code)
    {
        /* Turn ON the error LED */
        R_IOPORT_PinWrite (&g_ioport_ctrl, g_bsp_leds.p_leds[ERROR_LED_INDEX], BSP_IO_LEVEL_HIGH);

        /* Close opened module*/
        if(0U != g_usbcc0_ctrl.open)
        {
            R_USB_TYPEC_Close(&g_usbcc0_ctrl);
        }

        if(0U != g_comms_usb_pcdc0_ctrl.open)
        {
            RM_COMMS_USB_PCDC_Close(&g_comms_usb_pcdc0_ctrl);
        }

        if(0U != g_adc_ctrl.opened)
        {
            R_ADC_Close(&g_adc_ctrl);
        }

        /* Trap to stop execution */
        ERROR_TRAP;
    }
}
