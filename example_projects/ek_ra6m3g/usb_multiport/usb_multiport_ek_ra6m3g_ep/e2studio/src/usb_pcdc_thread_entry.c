/***********************************************************************************************************************
 * File Name    : usb_pcdc_thread_entry.c
 * Description  : Contains entry function of USB PCDC.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
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

/* Global variable */
uint8_t g_usb_module_number  = RESET_VALUE;         /* variable for module number */
usb_class_t g_usb_class_type = RESET_VALUE;         /* variable for class type */
uint8_t g_buf[READ_BUF_SIZE] = {RESET_VALUE};       /* buffer to write data to terminal */
uint8_t g_readbuf[READ_BUF_SIZE]  = {RESET_VALUE};  /* buffer to read data from terminal */
uint8_t g_writeBuf[READ_BUF_SIZE] = {RESET_VALUE};  /* data to send from queue */
uint8_t g_tempBuf[READ_BUF_SIZE]  = {RESET_VALUE};  /* temp buffer */
volatile bool b_overflow = false;                   /* flag for overflow */
uint16_t g_bytestoWrite = RESET_VALUE;              /* variable to count bytes to write on file */

/* Static variable */
static volatile usb_event_info_t * p_usb_pcdc_event = NULL;
static usb_pcdc_linecoding_t g_line_coding;

/* Private function */
static void handle_error(fsp_err_t err, char * err_str);

/* extern variable */
extern bool b_writetoUSB;
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t *g_apl_string_table[];

/* USB descriptor */
const usb_descriptor_t g_usb_descriptor =
{
 g_apl_device,                   /* Pointer to the device descriptor */
 g_apl_configuration,            /* Pointer to the configuration descriptor for Full-speed */
 g_apl_hs_configuration,         /* Pointer to the configuration descriptor for Hi-speed */
 g_apl_qualifier_descriptor,     /* Pointer to the qualifier descriptor */
 g_apl_string_table,             /* Pointer to the string descriptor table */
 NUM_STRING_DESCRIPTOR
};

/*******************************************************************************************************************//**
 * @brief     Entry function of usb pcdc thread.
 * @param[IN] pointer to  pvParameters
 * @retval    None
 **********************************************************************************************************************/
void usb_pcdc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_err_t err = FSP_SUCCESS;
    BaseType_t err_queue = pdFALSE;

    /* Open USB instance */
    err = R_USB_Open (&g_basic1_ctrl, &g_basic1_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_Open failed.\r\n");
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nUSB PCDC Opened successfully.\n\r");

    while (true)
    {
        /* process events of usb pcdc */
        err = process_usb_pcdc_events();
        handle_error(err, "\r\nprocess_usb_pcdc_events failed.\r\n");

        /* Receive message from queue */
        err_queue = xQueueReceive (g_queue_data_from_hmsc, g_buf, (TickType_t) (RESET_VALUE));
        if (pdTRUE == err_queue)
        {
            APP_PRINT("\r\n   Data from the file is written on to console (Tera term via PCDC)\r\n")
            /* Write data to tera term */
            err = R_USB_Write (&g_basic1_ctrl, g_buf, WRITE_ITEM_SIZE, USB_CLASS_PCDC);
            /* Handle error */
            handle_error(err, "\r\nR_USB_Write failed.\r\n");
        }
        vTaskDelay (1);
    }
}

/*******************************************************************************************************************//**
 * @brief     Function processes usb pcdc events.
 * @param[IN] None
 * @retval    Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
fsp_err_t process_usb_pcdc_events(void)
{
    fsp_err_t err = FSP_SUCCESS;
    usb_event_info_t    event_info = *p_usb_pcdc_event;
    /* USB event received */
    switch (p_usb_pcdc_event->event)
    {
        case USB_STATUS_CONFIGURED:
        {
            err = R_USB_Read (&g_basic1_ctrl, g_readbuf, READ_BUF_SIZE, USB_CLASS_PCDC);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Read failed.\r\n");
            }
            break;
        }
        case USB_STATUS_WRITE_COMPLETE:
        {
            break;
        }

        case USB_STATUS_READ_COMPLETE:
        {
            /* Read data from tera term */
            err = R_USB_Read (&g_basic1_ctrl, g_readbuf, READ_BUF_SIZE, USB_CLASS_PCDC);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Read failed.\r\n");
                return err;
            }
            else
            {
                /* Check for flag status */
                if (true == b_writetoUSB)
                {
                    fillDataBufferandWritetoFile();
                }
            }
            /* Loop back received data to host */
            err = R_USB_Write (&g_basic1_ctrl, g_readbuf, p_usb_pcdc_event->data_size, USB_CLASS_PCDC);
            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Write failed.\r\n");
            }
            break;
        }

        case USB_STATUS_REQUEST: /* Receive Class Request */
        {
            /* Check for the specific CDC class request IDs */
            if (USB_PCDC_SET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
            {
                err =  R_USB_PeriControlDataGet (&g_basic1_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH );
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlDataGet failed.\r\n");
                }
            }
            else if (USB_PCDC_GET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
            {
                err =  R_USB_PeriControlDataSet (&g_basic1_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH );
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet failed.\r\n");
                }
            }
            else if (USB_PCDC_SET_CONTROL_LINE_STATE == (event_info.setup.request_type & USB_BREQUEST))
            {
                err = R_USB_PeriControlStatusSet (&g_basic1_ctrl, USB_SETUP_STATUS_ACK);
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet failed.\r\n");
                }
            }
            else
            {
                /* none */
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
 * @brief     Fill data buffer and Write to file.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
void fillDataBufferandWritetoFile(void)
{
    static uint16_t WriteBufIndex;

    if ( RESET_VALUE < p_usb_pcdc_event->data_size)
    {
        /* Check for buffer size index */
        if ( READ_BUF_SIZE == WriteBufIndex)
        {
            /* Reset the index */
            WriteBufIndex = RESET_VALUE;
            /* Set the flag */
            b_overflow = true;
        }
        g_writeBuf[WriteBufIndex++] = g_readbuf[RESET_VALUE];
        g_bytestoWrite++;
        /* Check for Carriage return */
        if (CARRIAGE_RETURN == g_readbuf[RESET_VALUE])
        {
            /* Check for overflow flag */
            if(true == b_overflow)
            {
                g_bytestoWrite = WRITE_ITEM_SIZE;
                /* Circular buffer is used. Logic is to arrange array in
                 * old to latest before writing to file */
                uint16_t tempBufIndex = RESET_VALUE;
                /* Copy data from WriteBufIndex to end of buf into temp buf*/
                for(uint16_t cnt = WriteBufIndex; cnt < READ_BUF_SIZE ; cnt++)
                {
                    g_tempBuf[tempBufIndex++] = g_writeBuf[cnt];
                }
                /* Copy data from 0 to WriteBufIndex into temp buf */
                for(uint16_t cnt = RESET_VALUE; cnt < WriteBufIndex; cnt++)
                {
                    g_tempBuf[tempBufIndex++] = g_writeBuf[cnt];
                }
                /* Copy data back to buffer */
                memcpy(g_writeBuf, g_tempBuf, READ_BUF_SIZE);
            }
            /* Send data to hmsc thread by queue */
            if (pdTRUE == (xQueueSend(g_queue_data_from_pcdc, g_writeBuf, (TickType_t)(RESET_VALUE))))
            {
                /* Reset index and flags */
                WriteBufIndex = RESET_VALUE;
                b_overflow = false;
                b_writetoUSB = false;
            }
        }
    }
}


/*******************************************************************************************************************//**
 * @brief       usb pcdc callback
 * @param[IN]   usb_event_info_t    usb event
 * @param[IN]   usb_hdl_t           task
 * @param[IN]   usb_onoff_t         state
 * @retval      None
 **********************************************************************************************************************/
void usb_pcdc_callback(usb_event_info_t * p_usb_event , usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED (task);
    FSP_PARAMETER_NOT_USED (state);
    /* capture the usb event info */
    p_usb_pcdc_event = p_usb_event;

}

/*******************************************************************************************************************//**
 *  @brief       Closes the USB module , Print and traps error.
 *  @param[IN]   status    error status
 *  @param[IN]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        if (FSP_SUCCESS != R_USB_Close(&g_basic1_ctrl))
        {
            APP_ERR_PRINT ("\r\n** R_USB_Close API Failed ** \r\n ");
        }
        APP_PRINT(err_str);
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_multiport_ep)
 **********************************************************************************************************************/
