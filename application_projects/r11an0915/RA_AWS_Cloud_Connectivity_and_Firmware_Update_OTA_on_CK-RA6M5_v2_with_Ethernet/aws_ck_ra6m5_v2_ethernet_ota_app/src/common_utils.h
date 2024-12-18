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
 * Copyright (C) 2023-2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* generic headers */
#include <sensors_thread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <usr_config.h>
#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "r_ioport.h"
#include "console_thread.h"
#include "r_typedefs.h"
#include "common_init.h"
#include "console_menu/console.h"

/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"
int _write(int iFile, char *cptr, int iLen);
int __printf (const char *format, ...);
fsp_err_t uart_initialize(void);
fsp_err_t uart_print_user_msg(uint8_t *p_msg);
void delayMicroseconds(uint32_t us);
void delay(uint32_t ms);
void Delay(unsigned int xms);

extern const char_t * const gp_cursor_store;
extern const char_t * const gp_cursor_restore;
extern const char_t * const gp_cursor_temp;
extern const char_t * const gp_cursor_frequency;
extern const char_t * const gp_cursor_intensity;

extern const char_t * const gp_green_fg;
extern const char_t * const gp_white_fg;

extern const char_t * const gp_clear_screen;
extern const char_t * const gp_cursor_home;
extern char __buff[];

#define MENU_RETURN_INFO  "\r\n\r\n> Press space bar to return to MENU\r\n"
#define MENU_FLASH_SELECT  "\r\n\r\n> Enter (a - i) to select options (or press space bar to return to main MENU)\r\n"
#define MENU_MAIN_SELECT  "\r\n\r\n> Enter (1-5) to select options\r\n"
#define DISABLE_CODE_SIGNING_PKEY_WARNING  "\r\n[GREEN]OTA is disabled, do not need to input this credential[WHITE]\r\n"
#define CARRIAGE_ASCII            (13u)     /* Carriage return */
#define ZERO_ASCII                (48u)     /* ASCII value of zero */
#define NINE_ASCII                (57u)     /* ASCII value for nine */
#define DATA_LENGTH               (20u)      /* Expected Input Data length */
#define UART_ERROR_EVENTS         (UART_EVENT_BREAK_DETECT | UART_EVENT_ERR_OVERFLOW | UART_EVENT_ERR_FRAMING | \
		UART_EVENT_ERR_PARITY)    /* UART Error event bits mapped in registers */

#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

#define UTF8          (1)
#define JSON          (2)

#ifdef LOG_ERROR
#undef LOG_ERROR
#define LOG_ERROR     (1)
#endif

#ifdef LOG_WARN
#undef LOG_WARN
#define LOG_WARN      (2)
#endif

#ifdef LOG_INFO
#undef LOG_INFO
#define LOG_INFO      (3)
#endif

#ifdef LOG_DEBUG
#undef LOG_DEBUG
#define LOG_DEBUG     (4)
#endif

#define MACRO_ZERO                ((uint8_t) 0)
#define MACRO_ONE                 ((uint8_t) 1)
#define MACRO_TWO                 ((uint8_t) 2)
#define MACRO_TEN                 ((uint8_t) 10)

#ifndef USR_LOG_LVL
#define LOG_LVL      (LOG_ERROR)       /* error conditions   */
#else
#undef  LOG_LVL
#define LOG_LVL      (USR_LOG_LVL)     /* error conditions   */
#endif

#define RTT_TERMINAL  (1)
#define ITM_TERMINAL  (2)
#define UART_TERMINAL (3)

#ifndef USR_LOG_TERMINAL
#define LOG_TERMINAL      (RTT_TERMINAL)       /* error conditions   */
#else
#define LOG_TERMINAL      (USR_LOG_TERMINAL)     /* error conditions   */
#endif

#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)
#define SIZE_128     (128u)

#define LVL_ERR      (1u)       /* error conditions   */

#define RESET_VALUE             (0x00)
#define PRE_INIT_VALUE          (0xFF)
#define BUFFER_LINE_LENGTH       (1024)
#define BUFFER_LENGTH_SHORT      (192)

#define KIT_NAME                "CK-RA6M5v2"

#define AP_VERSION              ("1.0")
#define MODULE_NAME             "AWS IoT Over-the-air Update Library"
#define BANNER_INFO             "\r\n********************************************************************************"\
                                "\r\n*   Renesas FSP Application Project for "MODULE_NAME"    *"\
                                "\r\n*   Application Project Version %s                                            *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                                "\r\n********************************************************************************"\
                                "\r\nRefer to Application Note for more details on Application Project and              " \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME"                    "\
                                "\r\n********************************************************************************\r\n"


#define AP_INFO                 "\r\nThis Application project demonstrates the functionalities of MQTT Client"\
                                "\r\nusing AWS IOT SDK with Ethernet Interface Module running on Renesas RA MCU's" \
							    "\r\n********************************************************************************\r\n"\
                                "\r\n\r\n"

#define SEGGER_INDEX            (0)

#define ETH_PREINIT             "\r\n \r\n--------------------------------------------------------------------------------"\
                                "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Pre IP Init       "\
                                "\r\n--------------------------------------------------------------------------------\r\n\r\n"

#define ETH_POSTINIT            "\r\n \r\n--------------------------------------------------------------------------------"\
                                "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Post IP Init       "\
                                "\r\n--------------------------------------------------------------------------------\r\n\r\n"

#define APP_PRINT(fn_, ...)         ({if(LOG_TERMINAL == RTT_TERMINAL){\
                                       SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__);\
                                    }\
                                    else {\
                                        __printf(fn_, ##__VA_ARGS__);\
                                    	printf_colour(__buff);\
                                    }})

#define APP_ERR_PRINT(fn_, ...)     ({if(LOG_LVL >= LOG_ERROR){\
                                        if(LOG_TERMINAL == RTT_TERMINAL){\
                                            SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), ", __FUNCTION__);\
      									    SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
                                     	}\
                                        else {\
                                            __printf("[ERR] In Function: %s(), ",__FUNCTION__);\
                                            printf_colour(__buff);\
                                            __printf(fn_, ##__VA_ARGS__);\
                                            printf_colour(__buff);\
                                        }\
								     }})

#define APP_WARN_PRINT(fn_, ...) ({if(LOG_LVL >= LOG_WARN){\
                                     if(LOG_TERMINAL == RTT_TERMINAL){\
                                         SEGGER_RTT_printf (SEGGER_INDEX, "[WARN] In Function: %s(), ", __FUNCTION__);\
                                         SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
                                     	}\
                                     else {\
                                         __printf("[WARN] In Function: %s(), ",__FUNCTION__); \
                                         printf_colour(__buff);\
                                         __printf(fn_, ##__VA_ARGS__);\
                                         printf_colour(__buff);\
                                     }\
								 }})

#define APP_INFO_PRINT(fn_, ...) ({if(LOG_LVL >= LOG_INFO){\
                                     if(LOG_TERMINAL == RTT_TERMINAL){\
                                         SEGGER_RTT_printf (SEGGER_INDEX, "[INFO] In Function: %s(), ", __FUNCTION__);\
                                         SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
                                     	}\
                                     else {\
                                         __printf("[INFO] In Function: %s(), ",__FUNCTION__);\
                                         printf_colour(__buff);\
                                         __printf(fn_, ##__VA_ARGS__);\
                                         printf_colour(__buff);\
                                     }\
                                 }})

#define APP_DBG_PRINT(fn_, ...)  ({if(LOG_LVL >= LOG_DEBUG){\
                                     if(LOG_TERMINAL == RTT_TERMINAL){\
                                         SEGGER_RTT_printf (SEGGER_INDEX, "[DBG] In Function: %s(), ", __FUNCTION__);\
                                         SEGGER_RTT_printf (SEGGER_INDEX, (fn_), ##__VA_ARGS__);\
                                     	}\
                                     else {\
                                         __printf("[DBG] In Function: %s(), ",__FUNCTION__); \
                                         printf_colour(__buff);\
                                         __printf(fn_, ##__VA_ARGS__);\
                                         printf_colour(__buff);\
                                     }\
                                 }})

#define APP_ERR_TRAP(err)        ({if(err){\
                                     if(LOG_LVL >= RTT_TERMINAL){\
                                         SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", (unsigned int)err);\
                                         __BKPT(0);\
                                     }\
                                     else {\
                                         __printf("\r\nReturned Error Code: 0x%x	\r\n", (unsigned int)err);\
                                         printf_colour(__buff);\
                                         __BKPT(0);\
                                     }\
                                 }})

#define APP_READ(read_data)     (SEGGER_RTT_Read (SEGGER_INDEX, read_data, sizeof(read_data)))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#endif /* COMMON_UTILS_H_ */
