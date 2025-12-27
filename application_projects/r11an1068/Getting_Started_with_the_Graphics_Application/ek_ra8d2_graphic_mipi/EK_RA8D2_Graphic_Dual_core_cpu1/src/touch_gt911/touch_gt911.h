/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/******************************************************************************************************************//**
 * @file          touch_gt911.h
 * @Version       1.0
 * @brief         Implements a driver for interfacing to the GOODIX GT911 Touch Controller.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 06.06.2024 1.00     First Release
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
#include "semphr.h"
#endif

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef TOUCH_GT911_H_
#define TOUCH_GT911_H_

#define USE_EVENT_NOTIFICATIONS (0)
#define USE_SEMAPHORES  (1)

#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8E2_EK)
#define RES_PIN             (IOPORT_PORT_10_PIN_01)
#define SCL_PIN             (IOPORT_PORT_05_PIN_12)
#define SDA_PIN             (IOPORT_PORT_05_PIN_11)
#define IRQ_PIN             (IOPORT_PORT_05_PIN_10)
#define INT_IRQ_CHANNEL     (3U)
#elif defined(BOARD_RA8P1_EK) || defined(BOARD_RA8D2_EK)
#define RES_PIN             (IOPORT_PORT_06_PIN_06)
#define SCL_PIN             (IOPORT_PORT_05_PIN_12)
#define SDA_PIN             (IOPORT_PORT_05_PIN_11)
#define IRQ_PIN             (IOPORT_PORT_01_PIN_11)
#define INT_IRQ_CHANNEL     (19)
#else
#error "Specific RES_PIN, SCL_PIN , SDA_PIN, IRQ_PIN, INT_IRQ_CHANNEL for touch control "
#endif

#if !defined (USE_EVENT_NOTIFICATIONS) && !defined(USE_SEMAPHORES) && (BSP_CFG_RTOS > 0)
#error "For RTOS-based environments, set either USE_EVENT_NOTIFICATIONS or USE_SEMAPHORES to 1"
#endif

#if (USE_EVENT_NOTIFICATIONS == 0) && (USE_SEMAPHORES==0) && (BSP_CFG_RTOS > 0)
#error "USE_EVENT_NOTIFICATIONS or USE_SEMAPHORES are set to 0 in a RTOS-based environment. Set either USE_EVENT_NOTIFICATIONS or USE_SEMAPHORES to 1."
#endif

#if (USE_EVENT_NOTIFICATIONS > 0) && (USE_SEMAPHORES > 0) && (BSP_CFG_RTOS > 0)
#error "USE_EVENT_NOTIFICATIONS or USE_SEMAPHORES are set to 1 in a RTOS-based environment. Set either USE_EVENT_NOTIFICATIONS or USE_SEMAPHORES to 1."
#endif

#if (BSP_CFG_RTOS == 2) && (USE_EVENT_NOTIFICATIONS > 0 ) && (configTASK_NOTIFICATION_ARRAY_ENTRIES < 2)
#error "Set configTASK_NOTIFICATION_ARRAY_ENTRIES to value >= 2"
#endif

#if (BSP_CFG_RTOS == 0)
#define EVENT_FLAG_T uint32_t
#define QUEUE_T uint32_t

#elif (BSP_CFG_RTOS == 1)
#define EVENT_FLAG_T TX_EVENT_FLAGS_GROUP *
#define SEMAPHORE_T TX_SEMAPHORE *
#elif (BSP_CFG_RTOS == 2)
#define SEMAPHORE_T SemaphoreHandle_t
#define EVENT_FLAG_T TaskHandle_t
#endif

#ifndef MAX_TOUCH_POINTS
#define MAX_TOUCH_POINTS    (5U)
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

typedef enum
{
    TOUCH_GT911_COORD_NO_TRANSFORM = 0,
    TOUCH_GT911_COORD_ROTATE_X  = (1<<0),
    TOUCH_GT911_COORD_ROTATE_Y  = (1<<1),
    TOUCH_GT911_COORD_EXCHANGE  = (1<<2),
}touch_gt911_coord_transform_t;

typedef enum
{
    TOUCH_GT911_I2C_EVENT_CHANNEL = 0,
    TOUCH_GT911_IRQ_EVENT_CHANNEL = 1,
}touch_notification_channel_t;

typedef struct st_sync_objects
{
#if USE_EVENT_NOTIFICATIONS == 1
    EVENT_FLAG_T p_event_flag;
#elif USE_SEMAPHORES == 1
    SEMAPHORE_T p_semaphore_i2c;
    SEMAPHORE_T p_semaphore_irq;
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

    uint32_t x_max;
    uint32_t y_max;

    touch_gt911_coord_transform_t transform_flags;
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
    uint32_t x;
    uint32_t y;
    uint32_t size;
}coord_t;

typedef struct
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

/**
 * @brief Reset the GT911 touch controller.
 *
 * This function performs a reset of the GT911 touch controller by configuring the
 * IRQ (Interrupt Request) and RESET pins according to the specified timing. It utilizes
 * I/O port manipulation and wait periods to achieve the required reset sequence.
 *
 * @param[in] p_ctrl Pointer to the GT911 touch controller control block.
 *
 * @return ::FSP_SUCCESS on successful reset, ::FSP_ERR_* on error.
 *
 * @note This function assumes that it is used in an environment where an RTOS is configured.
 *       It utilizes I/O port manipulation to control the IRQ and RESET pins, and it includes
 *       wait periods based on the configured RTOS to achieve the required reset timing.
 *
 * @warning The behavior of this function depends on the proper configuration of the I/O port,
 *          synchronization objects, and the underlying RTOS. Make sure to configure the RTOS
 *          properly to match the intended behavior of the function.
 */
fsp_err_t R_TOUCH_GT911_Reset(touch_gt911_ctrl_t * const);

/**
 * @brief Get the firmware version of the GT911 touch controller.
 *
 * This function reads the firmware version from the GT911 touch controller via I2C communication.
 * The firmware version information is stored in the @p p_fw_version parameter.
 *
 * @param[in]  p_ctrl       Pointer to the GT911 touch controller control block.
 * @param[out] p_fw_version Pointer to the variable to store the read firmware version.
 *
 * @return ::FSP_SUCCESS on successful firmware version retrieval, ::FSP_ERR_* on error.
 *
 * @note This function assumes that it is used in an environment where an RTOS is configured.
 *       It uses I2C communication to read the GT911 firmware version. The synchronization
 *       events TOUCH_GT911_I2C_TX_COMPLETE, TOUCH_GT911_I2C_RX_COMPLETE, and TOUCH_GT911_I2C_ERROR
 *       are utilized for synchronization during I2C operations.
 *
 * @warning The behavior of this function depends on the proper configuration of the I2C module,
 *          synchronization objects, and the underlying RTOS. Make sure to configure the RTOS
 *          properly to match the intended behavior of the function.
 */
fsp_err_t R_TOUCH_GT911_VersionGet(touch_gt911_ctrl_t * const, uint16_t * p_version);

/**
 * @brief Get the status of the GT911 touch controller.
 *
 * This function reads the status register of the GT911 touch controller via I2C communication.
 * The status information is stored in the @p p_status parameter. Optionally, the status register
 * can be cleared after reading by setting the @p clear_after_read parameter to true.
 *
 * @param[in]  p_ctrl             Pointer to the GT911 touch controller control block.
 * @param[out] p_status           Pointer to the variable to store the read status.
 * @param[in]  clear_after_read   Flag indicating whether to clear the status register after reading.
 *
 * @return fsp_err_t::FSP_SUCCESS on successful status read, fsp_err_t::FSP_ERR_* on error.
 *
 * @note This function assumes that it is used in an environment where an RTOS is configured.
 *       It uses I2C communication to read the GT911 status register. The synchronization
 *       events TOUCH_GT911_I2C_TX_COMPLETE, TOUCH_GT911_I2C_RX_COMPLETE, and TOUCH_GT911_I2C_ERROR
 *       are utilized for synchronization during I2C operations.
 *
 * @warning The behavior of this function depends on the proper configuration of the I2C module,
 *          synchronization objects, and the underlying RTOS. Make sure to configure the RTOS
 *          properly to match the intended behavior of the function.
 */

fsp_err_t R_TOUCH_GT911_StatusGet(touch_gt911_ctrl_t * const, uint8_t * p_status, bool clear_after_read);

/**
 * @brief Get touch points from the GT911 touch controller.
 *
 * This function reads touch points from the GT911 touch controller via I2C communication.
 * The touch point information is stored in the @p p_data parameter.
 *
 * @param[in]  p_ctrl Pointer to the GT911 touch controller control block.
 * @param[out] p_data Pointer to the structure to store the touch point data.
 *
 * @return ::FSP_SUCCESS on successful touch point retrieval, ::FSP_ERR_* on error.
 *
 * @note This function assumes that it is used in an environment where an RTOS is configured.
 *       It uses I2C communication to read the GT911 touch points. The synchronization
 *       events TOUCH_GT911_I2C_TX_COMPLETE, TOUCH_GT911_I2C_RX_COMPLETE, and TOUCH_GT911_I2C_ERROR
 *       are utilized for synchronization during I2C operations.
 *
 * @warning The behavior of this function depends on the proper configuration of the I2C module,
 *          synchronization objects, and the underlying RTOS. Make sure to configure the RTOS
 *          properly to match the intended behavior of the function.
 */
fsp_err_t R_TOUCH_GT911_PointsGet(touch_gt911_ctrl_t * const, touch_gt911_data_t * p_data);

#endif /* TOUCH_GT911_H_ */
