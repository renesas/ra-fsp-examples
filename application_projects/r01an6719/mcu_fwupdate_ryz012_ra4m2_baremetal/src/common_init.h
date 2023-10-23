/***********************************************************************************************************************
 * File Name    : common_init.h
 * Description  : Common init functions.
 **********************************************************************************************************************/
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

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

/* Macro definitions */
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

fsp_err_t common_init(void);

