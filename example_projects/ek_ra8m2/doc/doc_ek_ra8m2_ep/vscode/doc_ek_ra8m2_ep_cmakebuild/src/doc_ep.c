/***********************************************************************************************************************
 * File Name    : doc_ep.c
 * Description  : Contains data structures and functions used in doc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "doc_ep.h"

/* Flag to check occurrence of DOC event */
volatile bool g_doc_event_flag = false;

/*******************************************************************************************************************//**
 * @brief     This function initializes DOC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t doc_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open the DOC module */
    err = R_DOC_Open(&g_doc_ctrl, &g_doc_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to the DOC data input register.
 * @param[in] data                      Data to write to the DOC data input register.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
fsp_err_t doc_write(uint32_t data)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Write data to the DOC data input register */
    err = R_DOC_Write(&g_doc_ctrl, data);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened DOC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void doc_deinit(void)
{
    /* Close opened DOC module */
    if (g_doc_ctrl.open)
    {
        R_DOC_Close(&g_doc_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for DOC module that sets the event flag.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void doc_callback(doc_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Set the event flag */
    g_doc_event_flag = true;
}
