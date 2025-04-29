/***********************************************************************************************************************
 * File Name    : gpt_timer_initialize.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "gpt_timer_initialize.h"

/*******************************************************************************************************************//**
 * @brief      Initialize and start the GPT timer instance
 *
 * @param[in]  p_timer_ctrl  Timer instance control structure
 * @param[in]  p_timer_cfg   Timer instance config structure
 * @retval     FSP_SUCCESS or any other error value.
 **********************************************************************************************************************/
fsp_err_t gpt_timer_init(gpt_instance_ctrl_t * p_timer_ctrl, const timer_cfg_t * p_timer_cfg)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Open GPT timer instance */
    fsp_err = R_GPT_Open (p_timer_ctrl, p_timer_cfg);
    /* Handle error */
    if ( FSP_SUCCESS != fsp_err )
    {
        /* Print out in case of error */
        APP_ERR_PRINT ("\r\nGPT Timer open API failed\r\n");
        return fsp_err;
    }

    /* Start GPT Timer instance */
    fsp_err = R_GPT_Start (p_timer_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        /* Close timer if failed to start */
        if ( FSP_SUCCESS  != R_GPT_Close (p_timer_ctrl) )
        {
            /* Print out in case of error */
            APP_ERR_PRINT ("\r\nGPT Timer Close API failed\r\n");
        }

        APP_ERR_PRINT ("\r\nGPT Timer Start API failed\r\n");
        return fsp_err;
    }
    else
    {
        APP_PRINT ("\r\nGPT Timer Started successfully\r\n");
    }
    return FSP_SUCCESS;
}
/*******************************************************************************************************************//**
 * @} (end addtogroup freertos_ep)
 **********************************************************************************************************************/
