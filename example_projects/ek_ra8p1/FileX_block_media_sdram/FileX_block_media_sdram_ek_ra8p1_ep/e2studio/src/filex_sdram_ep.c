/***********************************************************************************************************************
 * File Name    : filex_sdram_ep.c
 * Description  : Contains data structures and functions used in filex_sdram_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "filex_sdram_ep.h"
#include "filex_thread.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes FileX Block Media module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
UINT filex_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = RM_FILEX_BLOCK_MEDIA_Open(&g_rm_filex_block_media_ctrl, &g_rm_filex_block_media_cfg);

    return (UINT)err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened FileX Block Media module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void filex_deinit(void)
{
    if (g_rm_filex_block_media_ctrl.open)
    {
        RM_FILEX_BLOCK_MEDIA_Close(&g_rm_filex_block_media_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function converts a string month to an integer month.
 * @param[in] p_month                   Pointer to a string month.
 * @retval    value                     Integer value representing the month.
 **********************************************************************************************************************/
UINT get_month(const CHAR * p_month)
{
    static const CHAR * p_string[] = {"Nul", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
                                      "Nov", "Dec"};

    /* Check and convert month string to integer */
    for (UINT value = (UINT)JANUARY; value <= (UINT)DECEMBER; value++)
    {
        if (RESET_VALUE == strncmp(p_string[value], p_month, MONTH_STR_LEN))
        {
            return (UINT)value;
        }
    }

    /* Set the default month to JANUARY if month is not detected */
    return (UINT)JANUARY;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for FileX Block Media module.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void g_rm_filex_block_media_callback(rm_filex_block_media_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case RM_BLOCK_MEDIA_EVENT_MEDIA_INSERTED:
        case RM_BLOCK_MEDIA_EVENT_MEDIA_REMOVED:
        {
            /* Do nothing */
            break;
        }

        case RM_BLOCK_MEDIA_EVENT_WAIT_END:
        {
            /* FileX driver returns RM_BLOCK_MEDIA_EVENT_WAIT_END event when an operation is completed.
             * It can be read, write or erase operation. If there is an occurrence of error event,
             * the FileX callback will still return the same event and the respective API
             * will return the specific error code. */
            tx_event_flags_set(&g_media_event_flag, RM_BLOCK_MEDIA_EVENT_WAIT_END, TX_OR);
            break;
        }

        case RM_BLOCK_MEDIA_EVENT_POLL_STATUS:
        case RM_BLOCK_MEDIA_EVENT_MEDIA_SUSPEND:
        case RM_BLOCK_MEDIA_EVENT_MEDIA_RESUME:
        case RM_BLOCK_MEDIA_EVENT_WAIT:
        default:
        {
            /* Do nothing */
            break;
        }
    }
}
