/***********************************************************************************************************************
 * File Name    : wifi_ep.h
 * Description  : Contains macros, variables, data structures, and functions used in wifi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef WIFI_EP_H_
#define WIFI_EP_H_

#include <string.h>
#include "hal_data.h"
#include "ctype.h"

/* Macro for the reset value */
#define RESET_VALUE                         (0x00)

/* Macros for buffer size */
#define BUFF_LEN                            (32U)

/* Macros for network operations */
#define WIFI_MAX_TRY                        (5U)
#define MAX_WIFI_SCAN_RESULTS               (10U)

/* Macros for menu options */
#define SCAN_AP_SELECTION                   (1U)
#define ENTER_SSID_SELECTION                (2U)

/* Macro for array indexing */
#define IP_BUFF_LEN                         (4U)

/* Macro for the user LED */
#if defined (BOARD_RA6M4_EK) || defined (BOARD_RA4M3_EK)
#define USER_LED                            (BSP_IO_PORT_04_PIN_00)
#elif defined (BOARD_RA2L1_EK)
#define USER_LED                            (BSP_IO_PORT_05_PIN_05)
#elif defined (BOARD_RA4M2_EK) || defined (BOARD_RA8E2_EK)
#define USER_LED                            (BSP_IO_PORT_04_PIN_05)
#elif defined (BOARD_RA2E1_EK)
#define USER_LED                            (BSP_IO_PORT_09_PIN_13)
#elif defined (BOARD_RA6M5_EK) || defined (BOARD_RA0E1_FPB)
#define USER_LED                            (BSP_IO_PORT_00_PIN_08)
#elif defined (BOARD_RA6E1_FPB) || defined (BOARD_RA4E1_FPB) || defined (BOARD_RA8E1_FPB)
#define USER_LED                            (BSP_IO_PORT_04_PIN_08)
#elif defined (BOARD_RA2E2_EK)
#define USER_LED                            (BSP_IO_PORT_01_PIN_12)
#elif defined (BOARD_RA6T2_MCK)
#define USER_LED                            (BSP_IO_PORT_13_PIN_01)
#elif defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
#define USER_LED                            (BSP_IO_PORT_01_PIN_07)
#elif defined (BOARD_RA2A2_EK)
#define USER_LED                            (BSP_IO_PORT_03_PIN_07)
#elif defined (BOARD_RA8T1_MCK)
#define USER_LED                            (BSP_IO_PORT_10_PIN_12)
#elif defined (BOARD_RA4L1_EK)
#define USER_LED                            (BSP_IO_PORT_06_PIN_09)
#elif defined (BOARD_RA0E2_FPB)
#define USER_LED                            (BSP_IO_PORT_01_PIN_03)
#elif defined (BOARD_RA2L2_EK)
#define USER_LED                            (BSP_IO_PORT_02_PIN_04)
#elif defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK)
#define USER_LED                            (BSP_IO_PORT_06_PIN_00)
#elif defined (BOARD_RA4C1_EK)
#define USER_LED                            (BSP_IO_PORT_04_PIN_04)
#elif defined (BOARD_RA0L1_FPB)
#define USER_LED                            (BSP_IO_PORT_00_PIN_02)
#elif defined (BOARD_RA8T2_EK)
#define USER_LED                            (BSP_IO_PORT_07_PIN_14)
#elif defined (BOARD_RA4E2_FPB)
#define USER_LED                            (BSP_IO_PORT_02_PIN_07)
#else
#define USER_LED                            (BSP_IO_PORT_04_PIN_03)
#endif /* Specific board */

#if defined (BOARD_RA6T2_MCK) || defined (BOARD_RA8T1_MCK)
#define LED_ON                              (BSP_IO_LEVEL_LOW)
#define LED_OFF                             (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                              (BSP_IO_LEVEL_HIGH)
#define LED_OFF                             (BSP_IO_LEVEL_LOW)
#endif /* Specific board */

/* Structure for Wi-Fi parameters */
typedef struct
{
    char ssid[BUFF_LEN];
    char pwd[BUFF_LEN];
    WIFISecurity_t security;
    uint32_t ip_addr_device[4];
    fsp_err_t status;
} WiFiParameters_t;

/* Extern variables - Defined in wifi_ep.c */
extern WiFiParameters_t g_wifi;
extern WIFIScanResult_t g_scan_data[MAX_WIFI_SCAN_RESULTS];

/* Function declarations */
fsp_err_t wifi_open(void);
fsp_err_t wifi_config(void);
fsp_err_t wifi_scan(void);
fsp_err_t socket_connected(void);
void wifi_set_ap_ssid(uint32_t index);
void wifi_deinit(void);
void str_to_lowercase(uint8_t * p_str);
void set_led(bsp_io_level_t b_value);

#endif /* WIFI_EP_H_ */
