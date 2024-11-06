/**********************************************************************************************************************
 * File Name    : i3c_master_hdr.c
 * Description  : Contains macros data structures and functions used for demonstrate HDR-DDR mode.
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/
#include "i3c_master_hdr.h"

#ifdef I3C_HDR_DDR_SUPPORT
/**********************************************************************************************************************
 * External global variable
 *********************************************************************************************************************/
extern i3c_command_descriptor_t command_descriptor;
extern uint8_t g_write_data[MAX_WRITE_DATA_LEN] BSP_ALIGN_VARIABLE(WORD_ALIGN);
extern uint8_t * p_next;
extern uint32_t      g_write_read_routine_count;
extern uint8_t       * p_last;
extern uint32_t      g_data_transfer_size;

/* Private function declarations.*/
static fsp_err_t i3c_write_hdr(void);
static fsp_err_t i3c_read_hdr(void);

/**********************************************************************************************************************
 *  Function Name: i3c_write_hdr
 *  Description  : This function performs I3C write operation in HDR-DDR mode.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t i3c_write_hdr(void)
{
    fsp_err_t       err = FSP_SUCCESS;
    uint32_t        status = RESET_VALUE;

    /* Send the command code in HDR-DDR */
    command_descriptor.command_code = I3C_HDR_COMMAND_CODE;
    /* Set a buffer for storing the data write by the command. */
    command_descriptor.p_buffer = g_write_data;
    /* The length for write in HDR-DDR. Must be set to even value. */
    command_descriptor.length = MAX_WRITE_DATA_LEN;
    /* Terminate the transfer with a Repeated Start condition. */
    command_descriptor.restart = false;
    /* Identifies direction of the transfer is write */
    command_descriptor.rnw = false;

    /* Perform write operation in HDR-DDR mode.*/
    err = R_I3C_CommandSend(&g_i3c0_ctrl, &command_descriptor);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR : R_I3C_CommandSend API FAILED.\r\n");
        return err;
    }

    /* Wait for command complete.*/
    status = i3c_app_event_notify(I3C_EVENT_FLAG_COMMAND_COMPLETE, WAIT_TIME);
    if (RESET_VALUE == status)
    {
        APP_ERR_PRINT ("\r\nERROR : Requested event not received with in specified timeout.\r\n");
        return FSP_ERR_TIMEOUT;
    }

    APP_PRINT ("\r\nINFO: Write in HDR-DDR successfully.\r\n");

    return err;
}
/**********************************************************************************************************************
* End of function i3c_write_hdr
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: i3c_read_hdr
 *  Description  : This function performs I3C read operation in HDR-DDR mode.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
static fsp_err_t i3c_read_hdr(void)
{
    fsp_err_t       err = FSP_SUCCESS;
    uint32_t        status = RESET_VALUE;

    /* Send the command code in HDR-DDR */
    command_descriptor.command_code = I3C_HDR_COMMAND_CODE;
    /* Set a buffer for storing the data read by the command. */
    command_descriptor.p_buffer = p_next;
    /* The length for read in HDR-DDR. Must be set to even value,. */
    command_descriptor.length = MAX_READ_DATA_LEN;
    /* Terminate the transfer with a Repeated Start condition. */
    command_descriptor.restart = false;
    /* Identifies direction of the transfer is read */
    command_descriptor.rnw = true;

    /* Perform read operation in HDR-DDR mode.*/
    err = R_I3C_CommandSend(&g_i3c0_ctrl, &command_descriptor);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nERROR : R_I3C_CommandSend API FAILED.\r\n");
        return err;
    }

    /* Wait for command complete.*/
    status = i3c_app_event_notify(I3C_EVENT_FLAG_COMMAND_COMPLETE, WAIT_TIME);
    if (RESET_VALUE == status)
    {
        APP_ERR_PRINT ("\r\nERROR : Requested event not received with in specified timeout.\r\n");
        return FSP_ERR_TIMEOUT;
    }

    APP_PRINT ("\r\nINFO: Read in HDR-DDR successfully.\r\n");

    /* Update next read buffer */
    set_next_read_buffer();

    return err;
}
/**********************************************************************************************************************
* End of function i3c_read_hdr
**********************************************************************************************************************/

/**********************************************************************************************************************
 *  Function Name: master_write_read_hdr_verify
 *  Description  : This function performs I3C operation in HDR-DDR mode.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 *********************************************************************************************************************/
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
       APP_ERR_PRINT("\r\nERROR : R_I3C_DeviceSelect API FAILED.\r\n");
       return err;
   }

   /* Perform write operation.*/
   err = i3c_write_hdr();
   if (FSP_SUCCESS != err)
   {
       APP_ERR_PRINT("\r\nERROR : i3c_write_ddr() FAILED.\r\n");
       return err;
   }

   /* Wait for slave device prepare buffer. */
   R_BSP_SoftwareDelay(DELAY_FOR_SLAVE_PREPARE, BSP_DELAY_UNITS_MICROSECONDS);

   /* Start a read operation. */
   err = i3c_read_hdr();
   if (FSP_SUCCESS != err)
   {
       APP_ERR_PRINT("\r\nERROR : i3c_read_ddr() FAILED.\r\n");
       return err;
   }

   /* Compare read data with written data on slave.*/
   if (RESET_VALUE == memcmp(g_write_data, p_last, sizeof(g_write_data)))
   {
       APP_PRINT("\r\nINFO: [HDR-DDR] Data written to I3C slave is read back and matching - SUCCESS\r\n");
       APP_PRINT("INFO: [HDR-DDR] Data Transfer size 0x%x\r\n",g_data_transfer_size);
   }
   else
   {
       APP_ERR_PRINT("\r\nERROR : Data mismatch - FAILURE\r\n");
       return FSP_ERR_INVALID_DATA;
   }

   return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function master_write_read_hdr_verify
***********************************************************************************************************************/

#endif /* I3C_HDR_DDR_SUPPORT */
