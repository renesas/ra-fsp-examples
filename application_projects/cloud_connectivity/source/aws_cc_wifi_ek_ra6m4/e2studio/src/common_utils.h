/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "r_ioport.h"
#include "usr_config.h"

/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"

#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

#define UTF8          (1)
#define JSON          (2)

#define LOG_ERROR    (1)
#define LOG_WARN     (2)
#define LOG_INFO     (3)
#define LOG_DEBUG    (4)

#ifndef USR_LOG_LVL
#define LOG_LVL      (LOG_ERROR)       /* error conditions   */
#else
#undef  LOG_LVL
#define LOG_LVL      (USR_LOG_LVL)     /* error conditions   */
#endif

#define RTT_TERMINAL  (1)
#define ITM_TERMINAL  (2)

#ifndef USR_LOG_TERMINAL
#define LOG_TERMINAL      (RTT_TERMINAL)       /* error conditions   */
#else
#define LOG_TERMINAL      (USR_LOG_TERMINAL)     /* error conditions   */
#endif

#define RESET_VALUE             (0x00)
#define PRE_INIT_VALUE          (0xFF)

#define KIT_NAME                "EK-RA6M3"

#define AP_VERSION              ("1.1")
#define MODULE_NAME             "MQTT Client Module using AWS SDK"
#define BANNER_INFO             "\r\n********************************************************************************"\
                                "\r\n*   Renesas FSP Application Project for "MODULE_NAME"       *"\
                                "\r\n*   Application Project Version %s                                            *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                                "\r\n********************************************************************************"\
                                "\r\nRefer to Application Note for more details on Application Project and              " \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME"                    "\
                                "\r\n********************************************************************************\r\n"

#define AP_INFO                 "\r\nThis Application project demonstrates the functionalities of MQTT Client"\
                                "\r\nusing AWS IOT SDK with WiFi_OnChip_Silex Module running on Renesas RA MCU's" \
							    "\r\n********************************************************************************\r\n"\
                                "\r\n\r\n"

#define SEGGER_INDEX            (0)

#define APP_PRINT(fn_, ...)         if(LOG_TERMINAL == RTT_TERMINAL)\
	                                    SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__);\
                                    else \
                                 	    printf((fn_), ##__VA_ARGS__);

#define APP_ERR_PRINT(fn_, ...)     if(LOG_LVL >= LOG_ERROR){\
                                        if(LOG_TERMINAL == RTT_TERMINAL){\
                                            SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), ", __FUNCTION__);\
      									    SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
                                     	}\
                                        else {\
								 	        printf("[ERR] In Function: %s(), ",__FUNCTION__); \
									        printf(fn_, ##__VA_ARGS__);\
                                        }\
								     }

#define APP_WARN_PRINT(fn_, ...) if(LOG_LVL >= LOG_WARN){\
                                     if(LOG_TERMINAL == RTT_TERMINAL){\
                                         SEGGER_RTT_printf (SEGGER_INDEX, "[WARN] In Function: %s(), ", __FUNCTION__);\
                                         SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
                                     	}\
                                     else {\
								 	     printf("[WARN] In Function: %s(), ",__FUNCTION__); \
									     printf(fn_, ##__VA_ARGS__);\
                                     }\
								 }

#define APP_INFO_PRINT(fn_, ...) if(LOG_LVL >= LOG_INFO){\
                                     if(LOG_TERMINAL == RTT_TERMINAL){\
                                         SEGGER_RTT_printf (SEGGER_INDEX, "[INFO] In Function: %s(), ", __FUNCTION__);\
                                         SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
                                     	}\
                                     else {\
								 	     printf("[INFO] In Function: %s(), ",__FUNCTION__); \
									     printf((fn_), ##__VA_ARGS__);\
                                     }\
                                 }

#define APP_DBG_PRINT(fn_, ...)  if(LOG_LVL >= LOG_DEBUG){\
                                     if(LOG_TERMINAL == RTT_TERMINAL){\
                                         SEGGER_RTT_printf (SEGGER_INDEX, "[DBG] In Function: %s(), ", __FUNCTION__);\
                                         SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
                                     	}\
                                     else {\
								 	     printf("[DBG] In Function: %s(), ",__FUNCTION__); \
									     printf((fn_), ##__VA_ARGS__);\
                                     }\
                                 }

#define APP_ERR_TRAP(err)        if(err){\
                                     if(LOG_LVL >= RTT_TERMINAL){\
                                         SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", (unsigned int)err);\
                                         __asm("BKPT #0\n");\
                                     }\
                                     else {\
                                         printf("\r\nReturned Error Code: 0x%x	\r\n", (unsigned int)err);\
                                         __asm("BKPT #0\n");\
                                     }\
                                 }

#define APP_READ(read_data)     SEGGER_RTT_Read (SEGGER_INDEX, read_data, sizeof(read_data));

#define APP_CHECK_DATA          SEGGER_RTT_HasKey()

#endif /* COMMON_UTILS_H_ */
