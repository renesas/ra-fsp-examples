/***********************************************************************************************************************
 * File Name    : touch_ft5x06.h
 * Description  : Contains declarations of data and functions used in touch_ft5x06.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "touch_thread.h"

#define FT5X06_NUM_POINTS    5

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
    touch_coord_t point[FT5X06_NUM_POINTS];
} touch_data_t;

void ft5x06_init(i2c_master_instance_t const * p_i2c_instance, SemaphoreHandle_t * i2c_semaphore, bsp_io_port_pin_t reset_pin);
void ft5x06_payload_get(touch_data_t * touch_data);
