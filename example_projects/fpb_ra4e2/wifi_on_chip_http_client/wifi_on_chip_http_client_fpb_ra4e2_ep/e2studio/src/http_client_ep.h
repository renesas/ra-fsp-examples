/***********************************************************************************************************************
 * File Name    : http_client_ep.h
 * Description  : Contains macros and functions used in http_client_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef HTTP_CLIENT_EP_H_
#define HTTP_CLIENT_EP_H_

#include "hal_data.h"
#include "http_client_thread.h"

/* Macros for HTTP request */
#define HTTP_GET_SELECTION                  (1U)
#define HTTP_PUT_SELECTION                  (2U)
#define HTTP_POST_SELECTION                 (3U)
#if defined(BOARD_RA0L1_FPB)
#define HTTP_ONCHIP_DA16XXX_BUFFER_SIZE     (1024U)
#else
#define HTTP_ONCHIP_DA16XXX_BUFFER_SIZE     (2048U)
#endif /* BOARD_RA0L1_FPB */
#define HTTP_GET_ENDPOINT                   "http://httpbin.org/get"
#define HTTP_PUT_ENDPOINT                   "http://httpbin.org/put"
#define HTTP_POST_ENDPOINT                  "http://httpbin.org/post"
#define HTTP_PUT_REQUEST_BODY_SAMPLE        "HTTP-Client PUT method sample test!"
#define HTTP_POST_REQUEST_BODY_SAMPLE       "HTTP-Client POST method sample test!"

/* Macro for the reset value */
#define RESET_VALUE                         (0x00)

/* Macro for the user input buffer */
#define BUFFER_SIZE                         (32U)

/* Macro for the null char */
#define NULL_CHAR                           ('\0')

/* Function declarations */
fsp_err_t http_client_init(http_onchip_da16xxx_instance_ctrl_t * p_ctrl);
fsp_err_t http_client_send(http_onchip_da16xxx_instance_ctrl_t * p_ctrl, http_onchip_da16xxx_request_t * p_request,
                           http_onchip_da16xxx_buffer_t * p_buffer);
void http_client_deinit(http_onchip_da16xxx_instance_ctrl_t * p_ctrl);

#endif /* HTTP_CLIENT_EP_H_ */
