/***********************************************************************************************************************
 * File Name    : common_init.h
 * Description  : Common init functions.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#include "hal_data.h"

#define BLINK_FREQ_1HZ        (60000000)
#define BLINK_FREQ_5HZ        (12000000)
#define BLINK_FREQ_10HZ       (6000000)

#define CARRIAGE_RETURN       ('\r')
#define NUM_STRING_DESCRIPTOR (7U)

#define RYZ012_FWIMAGE_START_ADDRESS      0x00060000
#define RYZ012_FWIMAGE_END_ADDRESS        0x00080000
#define FLASH_BLOCK_SIZE                   (32 * 1024)
#define RYZ012_FWIMAGE_NUM_BLOCKS         ((RYZ012_FWIMAGE_END_ADDRESS - RYZ012_FWIMAGE_START_ADDRESS) / FLASH_BLOCK_SIZE)

extern bsp_leds_t g_bsp_leds;

extern bool GPT_Is_Watchdog_Expired(void);
extern void GPT_Start_Watchdog(void);
extern void GPT_Stop_Watchdog(void);
extern void GPT_Set_WatchdogExpired(void);

/**
 * Watchdog Timeout
 * timer ticks chosen for
 * OTA_Telink mobile app RYZ012 FW transfer
 */
#define BLE_APP_OTA_TRANSFER_FAST 45  // approximately 45 sec,  Speed = 0 Unlimited (Fastest)
#define BLE_APP_OTA_TRANSFER_SLOW 225 // approximately 3m 45sec,Speed = 1 Slowest

/* Macro definitions */
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

fsp_err_t common_init(void);

