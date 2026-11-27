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

/* Callback argument memory required by g_doc_callback_set_guard() */
static doc_callback_args_t s_doc_callback_args;

/* Private function */
static void doc_callback(doc_callback_args_t * p_args);

/*******************************************************************************************************************//**
 * @brief     This function initializes the DOC module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t doc_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open the DOC module */
    err = g_doc_open_guard(NULL, NULL);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Register the callback */
    err = g_doc_callback_set_guard(NULL, doc_callback, NULL, &s_doc_callback_args);

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
    err = g_doc_write_guard(NULL, data);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened DOC module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void doc_deinit(void)
{
    /* Close the opened DOC module */
    g_doc_close_guard(NULL);
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for the DOC module.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
static void doc_callback(doc_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Set the event flag */
    g_doc_event_flag = true;
}
