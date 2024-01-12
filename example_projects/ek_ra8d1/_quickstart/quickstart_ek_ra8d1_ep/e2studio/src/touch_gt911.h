/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : touch_gt911.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include "hal_data.h"

#define GT911_NUM_POINTS             5

#define GT911_REG_PRODUCT_ID         0x8140
#define GT911_REG_READ_COORD_ADDR    0x814E
#define GT911_REG_POINT1_X_ADDR      0x814F
#define GT911_REG_COMMAND            0x8040
#define GT911_REG_CONFIG_VERSION     0x8047
#define GT911_REG_CONFIG_CHECKSUM    0x80FF
#define GT911_REG_CONFIG_FRESH       0x8100
#define GT911_REG_FW_VER_HIGH        0x8145

typedef enum
{
    TOUCH_EVENT_NONE,
    TOUCH_EVENT_DOWN,
    TOUCH_EVENT_HOLD,
    TOUCH_EVENT_MOVE,
    TOUCH_EVENT_UP
} touch_event_t;

typedef struct st_touch_coord
{
    uint16_t      x;
    uint16_t      y;
    touch_event_t event;
} touch_coord_t;

typedef struct st_touch_data
{
    uint8_t       num_points;
    touch_coord_t point[GT911_NUM_POINTS];
} touch_data_t;

void GT911_init(i2c_master_instance_t const * p_i2c_instance, void * i2c_semaphore, bsp_io_port_pin_t reset_pin);
void GT911_payload_get(touch_data_t * touch_data);
