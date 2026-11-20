/***********************************************************************************************************************
 * File Name    : usb_pcdc_comms.c
 * Description  : Contains data structures and functions used in usb_pcdc_comms.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "pqc_ep.h"
#include "usb_pcdc_comms.h"

/* Private variables */
static volatile bool s_tx_flag = false;
static volatile bool s_rx_flag = false;
static volatile bool s_usb_opened = false;

/* Private function */
static fsp_err_t comms_write(uint8_t * p_src, uint32_t len);

/***********************************************************************************************************************
 * Function Name: comms_init.
 * Description  : This function initializes USB PCDC module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t comms_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_COMMS_USB_PCDC_Open(&g_comms_usb_pcdc0_ctrl, &g_comms_usb_pcdc0_cfg);
    if (FSP_SUCCESS == err)
    {
        s_usb_opened = true;
    }

    return err;
}
/***********************************************************************************************************************
 * End of function comms_init.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: comms_read.
 * Description  : This function reads data via USB PCDC.
 * Arguments    : p_dest                   Pointer to destination buffer.
 *                len                      Length of transfer data.
 * Return Value : FSP_SUCCESS              Read successfully.
 *                Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t comms_read(uint8_t * p_dest, uint32_t len)
{
    fsp_err_t err = FSP_SUCCESS;
    s_rx_flag = false;

    err = RM_COMMS_USB_PCDC_Read(&g_comms_usb_pcdc0_ctrl, p_dest, len);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for the RX complete event */
    while (false == s_rx_flag)
    {
        /* Do nothing */
    }

    return err;
}
/***********************************************************************************************************************
 * End of function comms_read.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: comms_recv_ack.
 * Description  : This function checks the response value from USB host.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Checked successfully.
 *                Any other error codes     Failed to check.
 **********************************************************************************************************************/
fsp_err_t comms_recv_ack(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t temp_buf[2] = {RESET_VALUE};

    err = comms_read(temp_buf, RESP_CMD_SIZE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (CMD_ACK != temp_buf[0])
    {
        return FSP_ERR_ASSERTION;
    }

    return err;
}
/***********************************************************************************************************************
 * End of function comms_recv_ack.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: comms_send_data.
 * Description  : This function sends data to USB host.
 * Arguments    : p_data                    Pointer to transfer data.
 *                len                       Length of transfer data.
 * Return Value : FSP_SUCCESS               Sent successfully.
 *                Any other error codes     Failed to send.
 **********************************************************************************************************************/
fsp_err_t comms_send_data(const uint8_t * p_data, size_t len)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t temp_buf[DATA_LEN_SIZE] = {RESET_VALUE};

    temp_buf[0] = (uint8_t)(len >> 8);
    temp_buf[1] = (uint8_t)(len & 0xFF);

    /* Send data length first */
    err = comms_write(temp_buf, DATA_LEN_SIZE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = comms_recv_ack();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Then send the transfer data */
    err = comms_write((uint8_t *)p_data, len);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = comms_recv_ack();

    return err;
}
/***********************************************************************************************************************
 * End of function comms_send_data.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: comms_recv_data.
 * Description  : This function receives data from USB host.
 * Arguments    : p_out_buf                 Pointer to stored buffer.
 *                len                       Length of received data.
 * Return Value : FSP_SUCCESS               Received successfully.
 *                Any other error codes     Failed to receive.
 **********************************************************************************************************************/
fsp_err_t comms_recv_data(uint8_t * p_out_buf, size_t len)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t temp_buf[RESP_CMD_SIZE] = {RESET_VALUE};

    err = comms_read(p_out_buf, len);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Send ACK */
    temp_buf[0] = CMD_ACK;
    err = comms_write(temp_buf, RESP_CMD_SIZE);

    return err;
}
/***********************************************************************************************************************
 * End of function comms_recv_data.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: comms_deinit.
 * Description  : This function de-initializes the opened USB PCDC module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void comms_deinit(void)
{
    if (s_usb_opened)
    {
        RM_COMMS_USB_PCDC_Close(&g_comms_usb_pcdc0_ctrl);
        s_usb_opened = false;
    }
}
/***********************************************************************************************************************
 * End of function comms_deinit.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: comms_write.
 * Description  : This function writes data via USB PCDC.
 * Arguments    : p_src                     Pointer to source buffer.
 *                len                       Length of transfer data.
 * Return Value : FSP_SUCCESS               Wrote successfully.
 *                Any other error codes     Failed to write.
 **********************************************************************************************************************/
static fsp_err_t comms_write(uint8_t * p_src, uint32_t len)
{
    fsp_err_t err = FSP_SUCCESS;
    s_tx_flag = false;

    err = RM_COMMS_USB_PCDC_Write(&g_comms_usb_pcdc0_ctrl, p_src, len);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for the transmission to complete */
    while (false == s_tx_flag)
    {
        /* Do nothing */
    }

    return err;
}
/***********************************************************************************************************************
 * End of function comms_write.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: rm_comms_usb_pcdc_callback.
 * Description  : This function is a callback for USB PCDC module.
 * Arguments    : p_args                    Pointer to the callback argument.
 * Return Value : None.
 **********************************************************************************************************************/
void rm_comms_usb_pcdc_callback(rm_comms_callback_args_t * p_args)
{
    if (NULL == p_args)
    {
        return;
    }

    switch (p_args->event)
    {
        case RM_COMMS_EVENT_TX_OPERATION_COMPLETE:
        {
            s_tx_flag = true;
            break;
        }

        case RM_COMMS_EVENT_RX_OPERATION_COMPLETE:
        {
            s_rx_flag = true;
            break;
        }

        default:
        {
            break;
        }
    }
}
/***********************************************************************************************************************
 * End of function rm_comms_usb_pcdc_callback.
 **********************************************************************************************************************/
