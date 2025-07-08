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
#include "apn_cpu1.h"
#include "hal_data.h"
/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/******************************************************************************
 * Global Variables
 *****************************************************************************/
share_mem_t *share_mem = (share_mem_t*) BSP_PARTITION_SHARED_MEM_START;

const bsp_ipc_semaphore_handle_t g_ipc_sem_smem =
{ .semaphore_num = 0 };
const bsp_ipc_semaphore_handle_t g_ipc_sem_input =
{ .semaphore_num = 1 };

uint8_t g_cpu0_rq_print = RESET_FLAG;
uint8_t temp_val_get = RESET_FLAG;
uint32_t temp_val = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief     IPC1 Callback
 *
 * @param[in] ipc_callback_args_t * The arguments information if interrupt occurs
 * @return    NONE
 **********************************************************************************************************************/
void ipc1_callback(ipc_callback_args_t *p_args)
{
    switch(p_args->event)
    {
        case IPC_EVENT_IRQ0:
        {
            g_cpu0_rq_print = SET_FLAG;
            break;
        }
        case IPC_EVENT_IRQ1:
        {
            R_BSP_IpcSemaphoreGive (&g_ipc_sem_input);
            break;
        }
        case IPC_EVENT_MESSAGE_RECEIVED:
        {
            temp_val = p_args->message;
            temp_val_get = SET_FLAG;
            break;
        }
        default:
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function checks if shared memory for user input is available.
 *
 * @param[in] None.
 * @return    See @ref RENESAS_ERROR_CODES or functions called by this function for other possible
 *            return codes.
 **********************************************************************************************************************/
fsp_err_t smem_input_busy(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_BSP_IpcSemaphoreTake (&g_ipc_sem_input);
    while (err)
    {
        if (FSP_ERR_IN_USE == err)
        {
            err = R_BSP_IpcSemaphoreTake (&g_ipc_sem_input);
        }
        else if (FSP_ERR_IP_CHANNEL_NOT_PRESENT == err)
        {
            APP_ERR_PRINT("\r\nIPC Do not present\r\n");
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function checks if shared memory for output message is available.
 *
 * @param[in] None.
 * @return    See @ref RENESAS_ERROR_CODES or functions called by this function for other possible
 *            return codes.
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
 * @brief      Initializes IPC communication between two cores or IPC instances.
 *
 * @param[in]  ipc0   Pointer to the first IPC instance configuration.
 * @param[in]  ipc1   Pointer to the second IPC instance configuration.
 *
 * @retval     fsp_err_t   FSP_SUCCESS if initialization is successful;
 *                         error code otherwise.
 **********************************************************************************************************************/
fsp_err_t ipc_init(const ipc_instance_t *ipc0, const ipc_instance_t *ipc1)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize IPC0 */
    err = R_IPC_Open(ipc0->p_ctrl, ipc0->p_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n Failure Open IPC0 ");
        return err;
    }
    /* Initialize IPC1 */
    err = R_IPC_Open(ipc1->p_ctrl, ipc1->p_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n Failure Open IPC1 ");
        return err;
    }
    return err;
}
