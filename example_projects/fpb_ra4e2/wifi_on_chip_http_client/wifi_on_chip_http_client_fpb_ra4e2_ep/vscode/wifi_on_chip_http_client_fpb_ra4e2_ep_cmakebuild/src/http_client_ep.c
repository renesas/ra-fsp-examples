/***********************************************************************************************************************
 * File Name    : http_client_ep.c
 * Description  : Contains data structures and functions used in http_client_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "http_client_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes HTTP Client service.
 * @param[in] p_ctrl                    Pointer to the HTTP Client control block.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t http_client_init(http_onchip_da16xxx_instance_ctrl_t * p_ctrl)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open the HTTP Client service */
    err = RM_HTTP_DA16XXX_Open(p_ctrl, &g_http_onchip_da16xxx_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sends the HTTP request with the configured buffers.
 * @param[in] p_ctrl                    Pointer to the HTTP Client control block.
 * @param[in] p_request                 Pointer to the HTTP request structure.
 * @param[in] p_buffer                  Pointer to the HTTP buffer structure.
 * @retval    FSP_SUCCESS               Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
fsp_err_t http_client_send(http_onchip_da16xxx_instance_ctrl_t * p_ctrl, http_onchip_da16xxx_request_t * p_request,
                            http_onchip_da16xxx_buffer_t * p_buffer)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Send the HTTP request */
    err = RM_HTTP_DA16XXX_Send(p_ctrl, p_request, p_buffer);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened HTTP Client service.
 * @param[in] p_ctrl                    Pointer to the HTTP Client control block.
 * @retval    None.
 **********************************************************************************************************************/
void http_client_deinit(http_onchip_da16xxx_instance_ctrl_t * p_ctrl)
{
    /* Close the opened HTTP Client service */
    if (p_ctrl->open)
    {
        RM_HTTP_DA16XXX_Close(p_ctrl);
    }
}
