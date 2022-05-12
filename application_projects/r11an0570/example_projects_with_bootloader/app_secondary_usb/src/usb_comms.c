/***********************************************************************************************************************
 * File Name    : usb_comms.c
 * Description  : Contains downloader communication related functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Copyright [2020-2022] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

#include "comms.h"
#include "header.h"

#ifdef COMMS_USB

/* pvParameters contains TaskHandle_t */

#define DATA_LEN                  1024
#define MAX_PACKET_SIZE           64
#define LINE_CODING_LENGTH        7
#define CONTROL_LINE_STATE_LENGTH 1

uint8_t g_buf[DATA_LEN];

typedef enum data_bits
{
    DATA_BITS_5 = 5,
    DATA_BITS_6,
    DATA_BITS_7,
    DATA_BITS_8,
    DATA_BITS_16 = 16,
}data_bits_t;

typedef enum parity_type
{
    PARITY_NONE = 0,
    PARITY_ODD,
    PARITY_EVEN,
    PARITY_MARK,
    PARITY_SPACE,
}parity_type_t;

typedef enum stop_bits
{
    STOP_BITS_1 = 0,
    STOP_BITS_1_5,
    STOP_BITS_2,
}stop_bits_t;


/* 115200 8n1 by default */
usb_pcdc_linecoding_t g_line_coding = {
    .dw_dte_rate    = 115200,
    .b_char_format = STOP_BITS_1,
    .b_parity_type = PARITY_NONE,
    .b_data_bits   = DATA_BITS_8,
};

volatile usb_pcdc_ctrllinestate_t g_control_line_state = {
    .bdtr = 0,
    .brts = 0,
};

extern usb_instance_ctrl_t g_basic0_ctrl;
extern const usb_cfg_t g_basic0_cfg;

void usb_cdc_rtos_callback(usb_event_info_t * event, usb_hdl_t handle, usb_onoff_t onoff)
{
    FSP_PARAMETER_NOT_USED(handle);
    FSP_PARAMETER_NOT_USED(onoff);

    BaseType_t xHigherPriorityTaskWoken;

    usb_setup_t             setup;

    /* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    switch (event->event)
    {
        case USB_STATUS_CONFIGURED :
        break;
        case USB_STATUS_WRITE_COMPLETE :
            if (pdTRUE == xSemaphoreGiveFromISR(g_usb_write_complete_binary_semaphore, &xHigherPriorityTaskWoken))
            {
                __NOP();
            }
        break;
        case USB_STATUS_READ_COMPLETE :
            if (pdTRUE == xQueueSendFromISR(g_usb_read_queue, &event->data_size, &xHigherPriorityTaskWoken ) )
            {
                __NOP();
            }

        break;
        case USB_STATUS_REQUEST : /* Receive Class Request */

            R_USB_SetupGet(event, &setup);
            if (USB_PCDC_SET_LINE_CODING == (setup.request_type & USB_BREQUEST))
            {

                R_USB_PeriControlDataGet(&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
            }
            else if (USB_PCDC_GET_LINE_CODING == (setup.request_type & USB_BREQUEST))
            {

                R_USB_PeriControlDataSet(&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
            }
            else if (USB_PCDC_SET_CONTROL_LINE_STATE == (event->setup.request_type & USB_BREQUEST))
            {
                fsp_err_t err = R_USB_PeriControlDataGet(&g_basic0_ctrl,(uint8_t *) &g_control_line_state, sizeof(g_control_line_state));
                if (FSP_SUCCESS == err)
                {
                    g_control_line_state.bdtr = (unsigned char)((event->setup.request_value >> 0) & 0x01);
                    g_control_line_state.brts = (unsigned char)((event->setup.request_value >> 1) & 0x01);
                }

            }
            else
            {
                /* none */
            }
        break;
        case USB_STATUS_REQUEST_COMPLETE :
            __NOP();
        break;
        case USB_STATUS_SUSPEND :
        case USB_STATUS_DETACH :
        case USB_STATUS_DEFAULT :
            __NOP();
        break;
        default :
            __NOP();
        break;
    }

    /* We can switch context if necessary. */
    /* Actual macro used here is port specific. */
    portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
}

fsp_err_t comms_open(void)
{
    fsp_err_t err;

    err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    if (FSP_SUCCESS != err)
    {
        return (err);
    }

    /* Wait for the application to open the COM port */
    while (0 == g_control_line_state.bdtr)
    {
        vTaskDelay(1);
    }

    return FSP_SUCCESS;
}

void comms_send(uint8_t * p_src, uint32_t len)
{
    fsp_err_t       err;

    err = R_USB_Write(&g_basic0_ctrl, p_src, len,  USB_CLASS_PCDC);
    if (FSP_SUCCESS != err)
    {
        return;
    }

    /* Wait for the USB Read to complete */
    if( xSemaphoreTake( g_usb_write_complete_binary_semaphore, portMAX_DELAY ) == pdTRUE )
    {
        __NOP();
    }
}

/*******************************************************************************************************************//**
 * @brief This function read the USB data with timeout control
 * @param[IN]   p_dest                  destination data buffer
 * @param[IN]   len                     length of the data
 * @param[IN]   timeout_milliseconds    timeout in milliseconds
 * @retval      FSP_SUCCESS             Upon successful FLash_HP data flash operations.
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP data flash operations.
 **********************************************************************************************************************/
fsp_err_t comms_read(uint8_t * p_dest, uint32_t * len, uint32_t timeout_milliseconds)
{
    fsp_err_t       err             = FSP_SUCCESS;
    BaseType_t      fr_err;
    TickType_t      timeout;


    err = R_USB_Read(&g_basic0_ctrl, p_dest, *len,  USB_CLASS_PCDC);
    if (FSP_SUCCESS != err)
    {
        return FSP_ERR_USB_FAILED;
    }

    if(timeout_milliseconds == portMAX_DELAY)
    {
        timeout = portMAX_DELAY;
    }
    else
    {
        timeout = timeout_milliseconds / portTICK_PERIOD_MS;
    }

    /* Wait for the USB Read to complete */
    *len = 0;
    fr_err = xQueueReceive(g_usb_read_queue, len, timeout);
    if (pdTRUE == fr_err)
    {
        return FSP_SUCCESS;
    }
    else
    {
        /* If there was a timeout, we need to terminate the USB transfer */

        err = R_USB_Stop(&g_basic0_ctrl, USB_TRANSFER_READ, USB_CLASS_PCDC);
        *len = 0;

        return FSP_ERR_TIMEOUT;
    }
}

#endif /* #ifdef COMMS_USB */
