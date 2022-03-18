/***********************************************************************************************************************
 * File Name    : https_agent.h
 * Description  : Contains macros, data structures and functions used in the https_agent.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/


#ifndef HTTPS_AGENT_H_
#define HTTPS_AGENT_H_

/* The config header is always included first. */
#include "iot_config.h"

/* Amazon FreeRTOS includes. */
#include "iot_https_client.h"
#include "iot_https_utils.h"

/* Platform layer includes. */
#include "platform/iot_clock.h"
#include "platform/iot_threads.h"
#include "private/iot_error.h"


/******************************************************************************
 * Macro definitions
 ******************************************************************************/

/* TLS port for HTTPS. */
#define IOT_DEMO_HTTPS_PORT    ( ( uint16_t ) 443U )

/* Buffer size for HTTP connection context. This is the minimum size from HTTP library, we cannot
 * use it directly because it's only available at runtime. */
#define IOT_DEMO_HTTPS_CONN_BUFFER_SIZE       			( ( int ) 512 )

/* Buffer size for HTTP request context and header.*/
#define IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE          	( ( int ) 2048 )

/* Buffer size for HTTP response context and header.*/
#define IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE         	( ( int ) 1024 )

/* Buffer size for HTTP response body.*/
#define IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE         	( ( int ) 1024 )

/* Default timeout for HTTP synchronous request. */
#define IOT_DEMO_HTTPS_SYNC_TIMEOUT_MS                  ( ( uint32_t ) 6000 )

#define IOT_DEMO_HTTPS_SYNC_NO_DELAY               ( ( uint32_t ) 0 )

/* Number of times to retry the HTTPS connection. A connection is only attempted again if
 * IOT_HTTPS_CONNECTION_ERROR is returned from IotHttpsClient_Connect(). This indicates an error in the network
 * layer.
 */
#define IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY				( ( uint32_t ) 5 )

#define IOT_DEMO_HTTPS_CONNECTION_RETRY_WAIT_MS			( ( uint32_t ) 3000 )

#define ID_LEN                                          ( (size_t) 26 )
#define ID_START_INDEX                                  (8)
#define INDEX_ZERO                                      (0)
#define URL_SIZE                                        (128)

/******************************************************************************
 * Exported functions (to be accessed by other files)
 ******************************************************************************/

/* Function prototypes */
IotHttpsReturnCode_t initialize_https_client (char *pURL);
IotHttpsReturnCode_t connect_aws_https_client(const IotNetworkInterface_t * pNetworkInterface);
IotHttpsReturnCode_t Process_GETRequest(void);
IotHttpsReturnCode_t Process_PUTRequest(float mcu_die_temp);
IotHttpsReturnCode_t Process_POSTRequest(float mcu_die_temp);
IotHttpsReturnCode_t add_header(IotHttpsRequestHandle_t reqHandle);

#endif /* HTTPS_AGENT_H_ */
