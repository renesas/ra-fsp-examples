/***********************************************************************************************************************
 * File Name    : ipc_cpu0.c
 * Description  : This source file implements functions declared in `common.h` to handle
 *                print requests locally on CPU0, process or format the data if necessary,
 *                and forward the final message to another core (e.g., CPU1) for further handling..
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "apn_cpu0.h"

/***********************************************************************************************************************
 * Extern variables
 **********************************************************************************************************************/
extern share_mem_t share_memory;

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
/* semaphore guard for share memory output buffer */
const bsp_ipc_semaphore_handle_t g_ipc_sem_smem =
{ .semaphore_num = 0 };
/* semaphore guard for share memory input buffer */
const bsp_ipc_semaphore_handle_t g_ipc_sem_input =
{ .semaphore_num = 1 };

/*******************************************************************************************************************//**
 * @brief     This function is to check if the output shared memory is free.
 *
 * @param[in] NONE
 * @return    See @ref RENESAS_ERROR_CODES or functions called by this function for other possible
 *            return codes. This function calls:
 **********************************************************************************************************************/
fsp_err_t smem_output_busy(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_BSP_IpcSemaphoreTake (&g_ipc_sem_smem);
    while(err)
    {
        if(FSP_ERR_IN_USE == err)
        {
            err = R_BSP_IpcSemaphoreTake (&g_ipc_sem_smem);
        }
        else if (FSP_ERR_IP_CHANNEL_NOT_PRESENT == err)
        {
            APP_ERR_PRINT("\r\nThe semaphore not present\r\n");
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This Function writes the message to the shared memory buffer
 *            for print out to the terminal by the other core
 *
 * @param[in] format: pointer to message buffer
 * @return    See @ref RENESAS_ERROR_CODES or functions called by this function for other possible
 *            return codes. This function calls:
 **********************************************************************************************************************/
 fsp_err_t ipc_log_send(char *format, ...)
 {
     /* Initialize variables */
     fsp_err_t  err   = FSP_SUCCESS;
     size_t     len   = RESET_VALUE;
     int        ret   = RESET_VALUE;
     unsigned char g_tx_buffer[BUFFER_SIZE_UP] = {RESET_VALUE};
     va_list aptr;

     va_start(aptr, format);
     ret = vsprintf((char *)g_tx_buffer, format, aptr);
     va_end(aptr);

     /* Check VSPRINTF function return code */
     if (RESET_VALUE > ret)
     {
         return (fsp_err_t)FSP_ERR_INVALID_ARGUMENT;
     }

     /* Calculate length of message */
     len = strlen((char *)g_tx_buffer);
     if (RESET_VALUE == len)
     {
         return (fsp_err_t)FSP_ERR_INVALID_SIZE;
     }

     while (smem_output_busy() != 0);

     /* Write message to share memory and trigger second core to print out terminal */
     memcpy(share_memory.buf_out, g_tx_buffer, BUFFER_SIZE_UP);
     share_memory.length = len;
     err = R_IPC_EventGenerate(&g_ipc1_ctrl, IPC_GENERATE_EVENT_IRQ0);
     if (FSP_SUCCESS != err)
     {
         APP_ERR_TRAP(err);
     }
     return err;
 }
