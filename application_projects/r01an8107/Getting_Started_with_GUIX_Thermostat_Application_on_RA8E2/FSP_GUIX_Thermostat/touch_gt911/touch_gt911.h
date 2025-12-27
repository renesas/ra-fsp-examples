/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/******************************************************************************************************************//**
 * @file          touch_gt911.h
 * @Version       1.0
 * @brief         Implements a driver for interfacing to the GOODIX GT911 Touch Controller.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 12.05.2024 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include "bsp_api.h"
#include "r_ioport.h"
#include "r_external_irq_api.h"
#include "r_i2c_master_api.h"

#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
#include "tx_api.h"
#elif (BSP_CFG_RTOS == 2)
#include "FreeRTOS.h"
#endif

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef TOUCH_GT911_H_
#define TOUCH_GT911_H_

#define USE_EVENT_FLAGS (0)
#define USE_SEMAPHORES  (1)

#if (BSP_CFG_RTOS == 0)
#define EVENT_FLAG_T uint32_t
#define QUEUE_T uint32_t

#elif (BSP_CFG_RTOS == 1)
#define EVENT_FLAG_T TX_EVENT_FLAGS_GROUP
#define QUEUE_T TX_QUEUE
#define SEMAPHORE_T TX_SEMAPHORE
#elif (BSP_CFG_RTOS == 2)
SEMPHORE_T SemaphoreHandle_t
QUEUE_T
#endif

#ifndef MAX_TOUCH_POINTS
#define MAX_TOUCH_POINTS    (5U)
#endif

#if defined(BOARD_RA8E2_EK)
#ifndef INT_IRQ_CHANNEL
#define INT_IRQ_CHANNEL (3U)
#endif
#endif

#define GT911_BUFFER_READY  (0x80U)
#define GT911_MASK_TOUCH_COUNT (0x0FU)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

typedef enum
{
    TOUCH_GT911_I2C_ERROR       = (1<<0),
    TOUCH_GT911_I2C_TX_COMPLETE = (1<<1),
    TOUCH_GT911_I2C_RX_COMPLETE = (1<<2),
    TOUCH_GT911_INT             = (1<<3),
}touch_gt911_event_flag_t;


typedef struct st_sync_objects
{
#if USE_EVENT_FLAGS == 1
    EVENT_FLAG_T * p_event_flag;
#elif USE_SEMAPHORES == 1
    SEMAPHORE_T * p_semaphore_i2c;
    SEMAPHORE_T * p_semaphore_irq;
    /* Queue Needed */
    QUEUE_T * p_touch_queue;
#endif
}sync_objects_t;

typedef struct
{
    /* Pins Needed  */
    ioport_port_pin_t reset_pin;
    ioport_port_pin_t irq_pin;
    ioport_port_pin_t i2c_scl_pin;
    ioport_port_pin_t i2c_sda_pin;

    /* I2C Interface needed */
    i2c_master_instance_t const * p_i2c_master;

    /* IRQ Interface needed */
    external_irq_instance_t const * p_external_irq;

    ioport_instance_t const * p_ioport;

    sync_objects_t * sync;


}touch_gt911_cfg_t;

typedef struct
{
    /* Pins Needed  */
    ioport_port_pin_t reset_pin;
    ioport_port_pin_t irq_pin;
    ioport_port_pin_t i2c_scl_pin;
    ioport_port_pin_t i2c_sda_pin;

    /* I2C Interface needed */
    i2c_master_instance_t const * p_i2c_master;

    /* IRQ Interface needed */
    external_irq_instance_t const * p_external_irq;

    ioport_instance_t const * p_ioport;

    sync_objects_t * sync;

}touch_gt911_ctrl_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t size;
}coord_t;

typedef struct p_data
{
    uint32_t touch_count;
    coord_t point[MAX_TOUCH_POINTS];
}touch_gt911_data_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

/* Function Name: module_function */
/******************************************************************************************************************//**
 * @brief <Replace this with a one paragraph short Description>
 *        detailed function commenting is at the definition. (C file)
 *********************************************************************************************************************/
fsp_err_t R_TOUCH_GT911_Validate (touch_gt911_cfg_t const * const p_cfg, uint32_t * const err_mask);
fsp_err_t R_TOUCH_GT911_Open     (touch_gt911_ctrl_t * const p_ctrl, touch_gt911_cfg_t const * const p_cfg);
fsp_err_t R_TOUCH_GT911_Close    (touch_gt911_ctrl_t * const);
fsp_err_t R_TOUCH_GT911_Reset(touch_gt911_ctrl_t * const);
fsp_err_t R_TOUCH_GT911_VersionGet(touch_gt911_ctrl_t * const, uint16_t * p_version);
fsp_err_t R_TOUCH_GT911_StatusGet(touch_gt911_ctrl_t * const, uint8_t * p_status, bool clear_after_read);
fsp_err_t R_TOUCH_GT911_PointsGet(touch_gt911_ctrl_t * const, touch_gt911_data_t * p_data);

#endif /* TOUCH_GT911_H_ */
