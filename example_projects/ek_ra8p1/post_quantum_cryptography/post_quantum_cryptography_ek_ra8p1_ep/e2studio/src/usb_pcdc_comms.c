/***********************************************************************************************************************
 * File Name    : usb_pcdc_comms.c
 * Description  : Contains data structures and common functions use to transfer data via USB PCDC Communication.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usb_pcdc_comms.h"

/* Private variables declarations */
static volatile bool g_tx_flag = false;
static volatile bool g_rx_flag = false;

/* Private function declaration */
static fsp_err_t comms_write(uint8_t * p_src, uint32_t len);

/***********************************************************************************************************************
 *  Function Name: rm_comms_usb_pcdc_callback
 *  Description  : This function is used to handle USB PCDC communication events.
 *  Arguments    : p_args    Pointer to the callback argument structure containing the event type.
 *  Return Value : None
 **********************************************************************************************************************/
void rm_comms_usb_pcdc_callback (rm_comms_callback_args_t * p_args)
{
    if (NULL == p_args)
    {
        return;
    }

    switch (p_args->event)
    {
        case RM_COMMS_EVENT_TX_OPERATION_COMPLETE:
            g_tx_flag = true;
            break;

        case RM_COMMS_EVENT_RX_OPERATION_COMPLETE:
            g_rx_flag = true;
            break;

        default:
            break;
    }
}
/***********************************************************************************************************************
* End of function rm_comms_usb_pcdc_callback.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: comms_write
 *  Description  : This function is used to write data via USB PCDC.
 *  Arguments    : p_src          Pointer to source buffer.
 *                 len            length of transfer data.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t comms_write(uint8_t * p_src, uint32_t len)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_COMMS_USB_PCDC_Write(&g_comms_usb_pcdc0_ctrl, p_src, len);
    if (FSP_SUCCESS == err)
    {
        g_tx_flag       = false;

        /* Wait for the transmission to complete */
        while (false == g_tx_flag)
        {
            __NOP();
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function comms_write.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: comms_read
 *  Description  : This function is used to read data via USB PCDC.
 *  Arguments    : p_dest         Pointer to destination buffer.
 *                 len            length of transfer data.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t comms_read(uint8_t * p_dest, uint32_t len)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_COMMS_USB_PCDC_Read(&g_comms_usb_pcdc0_ctrl, p_dest, len);
    if (FSP_SUCCESS == err)
    {
        if (FSP_SUCCESS == err)
        {
            g_rx_flag   = false;

            /* Wait for the rx complete event */
            while (false == g_rx_flag)
            {
                __NOP();
            }
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function comms_read.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: comms_recv_ack
 *  Description  : This function is used to check the response value from USB Host.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS          Receive ACK.
 *                 FSP_ERR_ASSERTION    Receive NAK.
 *                 Any other error code apart from FSP_SUCCESS when the read fails.
 **********************************************************************************************************************/
fsp_err_t comms_recv_ack(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t temp_buf[2] = {RESET_VALUE};

    err = comms_read(temp_buf, RESP_CMD_SIZE);
    if (FSP_SUCCESS == err && CMD_ACK != temp_buf[0])
    {
        err = FSP_ERR_ASSERTION;
    }

    return err;
}
/***********************************************************************************************************************
* End of function comms_recv_ack.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: comms_send_data
 *  Description  : This function is used to send data to USB Host
 *  Arguments    : p_data         Pointer to transfer data.
 *                 len            length of transfer data.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t comms_send_data(const uint8_t *p_data, size_t len)
{
    fsp_err_t err = FSP_SUCCESS;

    uint8_t temp_buf[DATA_LEN_SIZE] = {RESET_VALUE};

    temp_buf[0] = (uint8_t)(len >> 8);
    temp_buf[1] = (uint8_t)(len & 0xFF);

    /* Send data length first */
    err = comms_write(temp_buf, DATA_LEN_SIZE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "comms_send (data length) failed\r\n");

    err = comms_recv_ack();
    APP_ERR_RET(FSP_SUCCESS != err, err, "comms_recv_ack failed\r\n");

    /* Then send the transfer data */
    err = comms_write((uint8_t *)p_data, len);
    APP_ERR_RET(FSP_SUCCESS != err, err, "comms_send (data) failed\r\n");

    err = comms_recv_ack();
    APP_ERR_RET(FSP_SUCCESS != err, err, "comms_recv_ack failed\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function comms_send_data.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: comms_recv_data
 *  Description  : This function is used to receive data from USB Host.
 *  Arguments    : p_out_buf      Pointer to stored buffer.
 *                 len            length of received data.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t comms_recv_data (uint8_t *p_out_buf, size_t len)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t temp_buf[RESP_CMD_SIZE] = {RESET_VALUE};

    err = comms_read(p_out_buf, len);
    APP_ERR_RET(FSP_SUCCESS != err, err, "comms_read failed\r\n");

    /* Send ACK or NAK */
    temp_buf[0] = (FSP_SUCCESS == err) ? CMD_ACK : CMD_NAK;
    err = comms_write(temp_buf, RESP_CMD_SIZE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "comms_send failed\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function comms_recv_data.
***********************************************************************************************************************/
