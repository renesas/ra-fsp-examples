/***********************************************************************************************************************
 * File Name    : usb_pcdc_thread_entry.c
 * Description  : Contains entry function of USB PCDC.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usb_pcdc_thread.h"
#include "common_utils.h"
#include "usb_multiport.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_multiport_ep
 * @{
 **********************************************************************************************************************/

/* Global variables */
uint8_t g_usb_module_number  = RESET_VALUE;         /* Variable for module number */
usb_class_t g_usb_class_type = RESET_VALUE;         /* Variable for class type */
uint8_t g_buf[READ_BUF_SIZE] = {RESET_VALUE};       /* Buffer to write data to terminal */
uint8_t g_readbuf[READ_BUF_SIZE]  = {RESET_VALUE};  /* Buffer to read data from terminal */
uint8_t g_writebuf[READ_BUF_SIZE] = {RESET_VALUE};  /* Data to send from queue */
uint8_t g_tempbuf[READ_BUF_SIZE]  = {RESET_VALUE};  /* Temp buffer */
volatile bool g_overflow = false;                   /* Flag for overflow */
uint16_t g_bytes_to_write = RESET_VALUE;            /* Variable to count bytes to write on file */

/* Static variables */
static volatile usb_event_info_t * p_usb_pcdc_event = NULL;
static usb_pcdc_linecoding_t g_line_coding;

/* Private function */
static void handle_error(fsp_err_t err, char * err_str);

/* Extern variables */
extern bool g_write_to_usb;
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t *gp_apl_string_table[];

/* USB descriptor */
const usb_descriptor_t g_usb_descriptor =
{
 g_apl_device,                   /* Pointer to the device descriptor */
 g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
 g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
 g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
 gp_apl_string_table,            /* Pointer to the string descriptor table */
 NUM_STRING_DESCRIPTOR
};

/*******************************************************************************************************************//**
 * @brief     Entry function of USB PCDC thread.
 * @param[in] pointer to  pvParameters.
 * @retval    None.
 **********************************************************************************************************************/
void usb_pcdc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    fsp_err_t err = FSP_SUCCESS;
    BaseType_t err_queue = pdFALSE;

    /* Open USB instance */
    err = R_USB_Open(&g_basic1_ctrl, &g_basic1_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_Open API failed.\r\n");
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nUSB PCDC Opened successfully.\r\n");

    while (true)
    {
        /* Check if USB event is received */
        err_queue = xQueueReceive(g_event_queue, &p_usb_pcdc_event, (TickType_t) (RESET_VALUE));
        if (pdTRUE == err_queue)
        {
            /* Process events of USB PCDC */
            err = process_usb_pcdc_events();
            handle_error(err, "\r\nprocess_usb_pcdc_events failed.\r\n");
        }

        /* Receive message from queue */
        err_queue = xQueueReceive(g_queue_data_from_hmsc, g_buf, (TickType_t) (RESET_VALUE));
        if (pdTRUE == err_queue)
        {
            APP_PRINT("\r\nData from the file is written on to console (Tera Term via PCDC)\r\n");
            /* Write data to Tera Term */
            err = R_USB_Write(&g_basic1_ctrl, g_buf, WRITE_ITEM_SIZE, USB_CLASS_PCDC);
            /* Handle error */
            handle_error(err, "\r\nR_USB_Write API failed.\r\n");
        }
        vTaskDelay(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function processes USB PCDC events.
 * @param[in] None.
 * @retval    Any Other Error code apart from FSP_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t process_usb_pcdc_events(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* USB event received */
    switch (p_usb_pcdc_event->event)
    {
        case USB_STATUS_CONFIGURED:
        {
            err = R_USB_Read(&g_basic1_ctrl, g_readbuf, READ_BUF_SIZE, USB_CLASS_PCDC);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Read API failed.\r\n");
            }
            break;
        }
        case USB_STATUS_WRITE_COMPLETE:
        {
            break;
        }

        case USB_STATUS_READ_COMPLETE:
        {
            /* Read data from Tera Term */
            err = R_USB_Read(&g_basic1_ctrl, g_readbuf, READ_BUF_SIZE, USB_CLASS_PCDC);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Read API failed.\r\n");
                return err;
            }
            else
            {
                /* Check for flag status */
                if (true == g_write_to_usb)
                {
                    fillDataBufferandWritetoFile();
                }
            }
            /* Loop back received data to host */
            err = R_USB_Write(&g_basic1_ctrl, g_readbuf, p_usb_pcdc_event->data_size, USB_CLASS_PCDC);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Write API failed.\r\n");
            }
            break;
        }

        case USB_STATUS_REQUEST: /* Receive Class Request */
        {
            /* Check for the specific CDC class request IDs */
            if (USB_PCDC_SET_LINE_CODING == (p_usb_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                err = R_USB_PeriControlDataGet(&g_basic1_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlDataGet API failed.\r\n");
                }
            }
            else if (USB_PCDC_GET_LINE_CODING == (p_usb_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                err = R_USB_PeriControlDataSet(&g_basic1_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet API failed.\r\n");
                }
            }
            else if (USB_PCDC_SET_CONTROL_LINE_STATE == (p_usb_pcdc_event->setup.request_type & USB_BREQUEST))
            {
                err = R_USB_PeriControlStatusSet(&g_basic1_ctrl, USB_SETUP_STATUS_ACK);
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet API failed.\r\n");
                }
            }
            else
            {
                /* None */
            }
            break;
        }

        case USB_STATUS_DETACH:
        {
            APP_PRINT("USB is detached\r\n");
            break;
        }
        case USB_STATUS_SUSPEND:
        case USB_STATUS_RESUME:
        {
            break;
        }
        default:
        {
            break;
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     Fill data buffer and write to file.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void fillDataBufferandWritetoFile(void)
{
    static uint16_t WriteBufIndex;

    if (RESET_VALUE < p_usb_pcdc_event->data_size)
    {
        /* Check for buffer size index */
        if (READ_BUF_SIZE == WriteBufIndex)
        {
            /* Reset the index */
            WriteBufIndex = RESET_VALUE;
            /* Set the flag */
            g_overflow = true;
        }
        g_writebuf[WriteBufIndex++] = g_readbuf[RESET_VALUE];
        g_bytes_to_write++;
        /* Check for Carriage return */
        if (CARRIAGE_RETURN == g_readbuf[RESET_VALUE])
        {
            /* Check for overflow flag */
            if (true == g_overflow)
            {
                g_bytes_to_write = WRITE_ITEM_SIZE;
                /* Circular buffer is used. Logic is to arrange array in old to latest before writing to file */
                uint16_t tempBufIndex = RESET_VALUE;
                /* Copy data from WriteBufIndex to end of buffer into temperature buffer */
                for (uint16_t cnt = WriteBufIndex; cnt < READ_BUF_SIZE; cnt++)
                {
                    g_tempbuf[tempBufIndex++] = g_writebuf[cnt];
                }
                /* Copy data from 0 to WriteBufIndex into temp buffer */
                for (uint16_t cnt = RESET_VALUE; cnt < WriteBufIndex; cnt++)
                {
                    g_tempbuf[tempBufIndex++] = g_writebuf[cnt];
                }
                /* Copy data back to buffer */
                memcpy(g_writebuf, g_tempbuf, READ_BUF_SIZE);
            }
            /* Send data to HMSC thread by queue */
            if (pdTRUE == (xQueueSend(g_queue_data_from_pcdc, g_writebuf, (TickType_t)(RESET_VALUE))))
            {
                /* Reset index and flags */
                WriteBufIndex = RESET_VALUE;
                g_overflow = false;
                g_write_to_usb = false;
            }
        }
    }
}


/*******************************************************************************************************************//**
 * @brief       USB PCDC callback.
 * @param[in]   p_usb_event     USB event.
 * @param[in]   task            Task.
 * @param[in]   state           State.
 * @retval      None.
 **********************************************************************************************************************/
void usb_pcdc_callback(usb_event_info_t * p_usb_event , usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED(task);
    FSP_PARAMETER_NOT_USED(state);
    /* Capture the USB event info */
    xQueueSendFromISR(g_event_queue, (const void *)&p_usb_event, NULL);
}

/*******************************************************************************************************************//**
 *  @brief       This function closes the USB module, prints and traps error.
 *  @param[in]   err       Error status.
 *  @param[in]   err_str   Error string.
 *  @retval      None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        if (FSP_SUCCESS != R_USB_Close(&g_basic1_ctrl))
        {
            APP_ERR_PRINT("\r\n** R_USB_Close API failed **\r\n");
        }
        APP_PRINT(err_str);
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_multiport_ep)
 **********************************************************************************************************************/
