/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
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

/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"

#define RTT_TERMINAL  (1)
#define ITM_TERMINAL  (2)

#define LOG_ERROR    (1)
#define LOG_WARN     (2)
#define LOG_INFO     (3)
#define LOG_DEBUG    (4)


#define USR_LOG_TERMINAL     (RTT_TERMINAL)  /* User Options are:  RTT_TERMINAL  */

#ifndef USR_LOG_LVL
#define LOG_LVL      (LOG_ERROR)       /* error conditions   */
#else
#undef  LOG_LVL
#define LOG_LVL      (USR_LOG_LVL)     /* error conditions   */
#endif


#ifndef USR_LOG_TERMINAL
#define LOG_TERMINAL      (RTT_TERMINAL)       /* error conditions   */
#else
#define LOG_TERMINAL      (USR_LOG_TERMINAL)     /* error conditions   */
#endif

#define RESET_VALUE             (0x00)

#define KIT_NAME                "FPB-RA6E1"

#define AP_VERSION              ("1.0")
#define MODULE_NAME             "LPM  Module"
#define BANNER_INFO             "\r\n********************************************************************************"\
                                "\r\n*   Renesas FSP Application Project for "MODULE_NAME"                            *"\
                                "\r\n*   Application Project Version %s                                            *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                                "\r\n********************************************************************************"\
                                "\r\nPlease refer to Application Note for more details on Application Project and       " \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME"                    "\
                                "\r\n********************************************************************************\r\n"

#define AP_INFO                 "\r\nThis Application project demonstrates the changing clock source dynamically and "\
	                            "\r\nfunctionalities of RA LPM Modes using r_lpm HA Module drivers on Renesas RA MCU's"\
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
                                         __BKPT(0);\
                                     }\
                                     else {\
                                         printf("\r\nReturned Error Code: 0x%x	\r\n", (unsigned int)err);\
                                         __BKPT(0);\
                                     }\
                                 }

#define APP_READ(read_data)     SEGGER_RTT_Read (SEGGER_INDEX, read_data, sizeof(read_data));

#define APP_CHECK_DATA          SEGGER_RTT_HasKey()

#endif /* COMMON_UTILS_H_ */
