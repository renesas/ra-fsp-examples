/***********************************************************************************************************************
 * File Name    : i3c_master_hdr.c
 * Description  : Contains data structures and functions used in i3c_master_hdr.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "i3c_master_hdr.h"

#ifdef I3C_HDR_DDR_SUPPORT

/* Private function declarations */
static fsp_err_t i3c_write_hdr(void);
static fsp_err_t i3c_read_hdr(void);

/***********************************************************************************************************************
 * Function Name: i3c_write_hdr.
 * Description  : This function performs I3C write operation in HDR-DDR mode.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Written successfully.
 *                Any other error codes     Failed to write.
 **********************************************************************************************************************/
static fsp_err_t i3c_write_hdr(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t status = RESET_VALUE;

    /* Send the command code in HDR-DDR */
    command_descriptor.command_code = I3C_HDR_COMMAND_CODE;

    /* Set a buffer for storing the data write by the command */
    command_descriptor.p_buffer = g_write_data;

    /* The length for write in HDR-DDR. Must be set to even value. */
    command_descriptor.length = MAX_WRITE_DATA_LEN;

    /* Terminate the transfer with a Repeated Start condition */
    command_descriptor.restart = false;

    /* Identify direction of the transfer is write */
    command_descriptor.rnw = false;

    /* Perform write operation in HDR-DDR mode */
    err = R_I3C_CommandSend(&g_i3c0_ctrl, &command_descriptor);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for command complete */
    status = i3c_app_event_notify(I3C_EVENT_FLAG_COMMAND_COMPLETE, WAIT_TIME);
    if (RESET_VALUE == status)
    {
        return FSP_ERR_TIMEOUT;
    }

    return err;
}
/***********************************************************************************************************************
* End of function i3c_write_hdr
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_read_hdr.
 * Description  : This function performs I3C read operation in HDR-DDR mode.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Read successfully.
 *                Any other error codes     Failed to read.
 **********************************************************************************************************************/
static fsp_err_t i3c_read_hdr(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t status = RESET_VALUE;

    /* Send the command code in HDR-DDR */
    command_descriptor.command_code = I3C_HDR_COMMAND_CODE;

    /* Set a buffer for storing the data read by the command */
    command_descriptor.p_buffer = g_p_next;

    /* The length for read in HDR-DDR. Must be set to even value. */
    command_descriptor.length = MAX_READ_DATA_LEN;

    /* Terminate the transfer with a Repeated Start condition */
    command_descriptor.restart = false;

    /* Identify direction of the transfer is read */
    command_descriptor.rnw = true;

    /* Perform read operation in HDR-DDR mode */
    err = R_I3C_CommandSend(&g_i3c0_ctrl, &command_descriptor);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for command complete */
    status = i3c_app_event_notify(I3C_EVENT_FLAG_COMMAND_COMPLETE, WAIT_TIME);
    if (RESET_VALUE == status)
    {
        return FSP_ERR_TIMEOUT;
    }

    /* Update next read buffer */
    set_next_read_buffer();

    return err;
}
/***********************************************************************************************************************
* End of function i3c_read_hdr
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: master_write_read_hdr_verify.
 * Description  : This function performs I3C operation in HDR-DDR mode and verifies data match.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Verified successfully.
 *                Any other error codes     Failed to verify.
 **********************************************************************************************************************/
fsp_err_t master_write_read_hdr_verify(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Update write buffer */
    for (uint32_t cnt = RESET_VALUE; cnt < sizeof(g_write_data); cnt++)
    {
        g_write_data[cnt] = (uint8_t) (cnt + g_write_read_routine_count) & UINT8_MAX;
    }
    g_write_read_routine_count++;

    /* Switch to HDR-DDR mode */
    err = R_I3C_DeviceSelect(&g_i3c0_ctrl, 0, I3C_BITRATE_MODE_I3C_HDR_DDR_STDBR);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Perform write operation */
    err = i3c_write_hdr();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for Slave device prepare buffer */
    R_BSP_SoftwareDelay(DELAY_FOR_SLAVE_PREPARE, BSP_DELAY_UNITS_MICROSECONDS);

    /* Start a read operation */
    err = i3c_read_hdr();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Compare read data with written data on Slave */
    if (RESET_VALUE != memcmp(g_write_data, g_p_last, sizeof(g_write_data)))
    {
        return FSP_ERR_INVALID_DATA;
    }

    return err;
}
/***********************************************************************************************************************
* End of function master_write_read_hdr_verify
***********************************************************************************************************************/

#endif /* I3C_HDR_DDR_SUPPORT */
