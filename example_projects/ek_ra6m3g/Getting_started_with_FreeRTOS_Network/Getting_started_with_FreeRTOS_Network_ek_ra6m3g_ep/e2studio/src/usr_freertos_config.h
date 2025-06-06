/***********************************************************************************************************************
 * File Name    : usr_freertos_config.h
 * Description  : User configuration file for FreeRTOS.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USR_FREERTOS_CONFIG_H_
#define USR_FREERTOS_CONFIG_H_

/*
 * The ipconfigUSE_HTTP macro enables the HTTP server functionality within the FreeRTOS+TCP demo:
 * https://github.com/FreeRTOS/FreeRTOS/tree/main/FreeRTOS-Plus/Demo/Common/Demo_IP_Protocols/HTTP_Server.
 * When set to 1, this macro configures the embedded system to handle HTTP requests and serve web content.
 * In the demo, the HTTP server processes GET requests and serves HTML pages, enabling users to interact
 * with the embedded system.
 */
#define ipconfigUSE_HTTP (1)


#endif /* USR_FREERTOS_CONFIG_H_ */
