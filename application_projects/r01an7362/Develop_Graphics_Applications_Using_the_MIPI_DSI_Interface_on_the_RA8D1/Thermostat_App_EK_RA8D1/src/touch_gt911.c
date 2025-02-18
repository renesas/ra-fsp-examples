/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : touch_gt911.c
 * Version      : 1.0
 * Description  : Implements a driver for interfacing to the GOODIX GT911 Touch Controller.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 06.06.2024 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "touch_gt911.h"
/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/


/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

typedef enum e_coordinate_register_list
{
    GT911_PRODUCT_ID_FIRST_BYTE = 0x8140,
    GT911_PRODUCT_ID_SECOND_BYTE = 0x8141,
    GT911_PRODUCT_ID_THIRD_BYTE = 0x8142,
    GT911_PRODUCT_ID_FOURTH_BYTE = 0x8143,
    GT911_FIRMWARE_VERSION_LOW_BYTE = 0x8144,
    GT911_FIRMWARE_VERSION_HIGH_BYTE = 0x8145,
    GT911_X_RESOLUTION_LOW_BYTE = 0x8146,
    GT911_X_RESOLUTION_HIGH_BYTE = 0x8147,
    GT911_Y_RESOLUTION_LOW_BYTE = 0x8148,
    GT911_Y_RESOLUTION_HIGH_BYTE = 0x8149,
    GT911_VENDOR_ID = 0x814A,
    GT911_STATUS = 0x814E,
    GT911_COORDINATE_START = 0x814F,
} coordinate_register_list_t;


typedef struct st_chip_info
{
    uint32_t product_id;
    uint16_t firmware_version;
    uint16_t resolution_x;
    uint16_t resolution_y;
    uint8_t vendor_id;
}chip_info_t;

typedef __PACKED_STRUCT st_gt911_point_payload
{
    uint8_t track_id;
    uint16_t x;
    uint16_t y;
    uint16_t point_size;
    uint8_t reserved;
} gt911_point_payload_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
#if !defined(define_touch_i2c_callback)
#define define_touch_i2c_callback
void touch_i2c_callback (i2c_master_callback_args_t * p_args);
#endif

#if !defined(define_touch_int_callback)
#define define_touch_int_callback
void touch_int_callback (external_irq_callback_args_t * p_args);
#endif

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**
 * @brief Sends an event from a callback to synchronize with other parts of the system.
 *
 * This function is responsible for sending synchronization events based on the configured
 * synchronization mechanism (event flags or semaphores). The type of event to be sent is
 * specified by the @p event parameter.
 *
 * @param[in] p_sync Pointer to the synchronization objects structure.
 * @param[in] event  The touch event to be sent, specified as a touch_gt911_event_flag_t.
 *
 * @note This function is designed to work in an environment where either event notifications or
 *       semaphores are used for synchronization. The specific mechanism is determined by the
 *       configuration flags (USE_EVENT_NOTIFICATIONS and USE_SEMAPHORES).
 *
 * @warning The behavior of this function depends on the underlying RTOS configuration (BSP_CFG_RTOS).
 *          Make sure to configure the RTOS properly to match the intended behavior of the function.
 *
 * @warning This function is designed for internal use, and the parameters should be set correctly
 *          based on the specific requirements of the application.
 *
 * @note The function utilizes conditional compilation to handle different RTOS configurations and
 *       synchronization mechanisms. This includes handling event flags and semaphores based on the
 *       configured flags (USE_EVENT_NOTIFICATIONS and USE_SEMAPHORES).
 *
 * @return None.
 */
static void send_event_from_callback(sync_objects_t * p_sync, touch_gt911_event_flag_t event)
{
#if (USE_EVENT_NOTIFICATIONS == 1)

#if (BSP_CFG_RTOS == 1)
    UINT err = tx_event_flags_set(p_sync->p_event_flag, event, TX_OR);
    FSP_PARAMETER_NOT_USED(err);
#elif (BSP_CFG_RTOS == 2)
    UBaseType_t channel = (( TOUCH_GT911_I2C_ERROR | TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_RX_COMPLETE ) & event) ? TOUCH_GT911_I2C_EVENT_CHANNEL : TOUCH_GT911_IRQ_EVENT_CHANNEL;
    TaskHandle_t pHandle = p_sync->p_event_flag;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult = xTaskNotifyIndexedFromISR(pHandle,
                                                   channel,
                                                   event,
                                                   eSetBits,
                                                   &xHigherPriorityTaskWoken);
    if(pdPASS == xResult)
    {
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
#endif /* BSP_CFG_RTOS */

#elif (USE_SEMAPHORES == 1)
    SEMAPHORE_T p_sem = (( TOUCH_GT911_I2C_ERROR | TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_RX_COMPLETE ) & event) ? p_sync->p_semaphore_i2c : p_sync->p_semaphore_irq;
#if (BSP_CFG_RTOS == 1)
    UINT tx_err = tx_semaphore_put(p_sem);
    FSP_PARAMETER_NOT_USED(tx_err);
#elif (BSP_CFG_RTOS == 2)
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t xResult = xSemaphoreGiveFromISR( p_sem, &xHigherPriorityTaskWoken );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    FSP_PARAMETER_NOT_USED(xResult);
#endif /* BSP_CFG_RTOS */

#endif /* (USE_SEMAPHORES == 1) || (USE_EVENT_NOTIFICATIONS == 1) */
    return;
}

/**
 * @brief Waits for a specific event to occur, blocking if necessary until the event is signaled.
 *
 * This function is designed to wait for a synchronization event based on the configured synchronization
 * mechanism (event flags or semaphores). The specific event to wait for is determined by the @p event parameter.
 * The function blocks until the event is signaled or a timeout occurs.
 *
 * @param[in] p_sync Pointer to the synchronization objects structure.
 * @param[in] event  The touch event to wait for, specified as a touch_gt911_event_flag_t.
 *
 * @return fsp_err_t::FSP_SUCCESS if the event is successfully received, fsp_err_t::FSP_ERR_TIMEOUT if a timeout occurs, and
 *         fsp_err_t::FSP_ERR_ABORTED if the operation is aborted or encounters an error.
 *
 * @note This function is designed to work in an environment where either event notifications or semaphores
 *       are used for synchronization. The specific mechanism is determined by the configuration flags
 *       (USE_EVENT_NOTIFICATIONS and USE_SEMAPHORES).
 *
 * @warning The behavior of this function depends on the underlying RTOS configuration (BSP_CFG_RTOS).
 *          Make sure to configure the RTOS properly to match the intended behavior of the function.
 *
 * @warning This function is designed for internal use, and the parameters should be set correctly
 *          based on the specific requirements of the application.
 *
 * @note The function utilizes conditional compilation to handle different RTOS configurations and
 *       synchronization mechanisms. This includes handling event flags and semaphores based on the
 *       configured flags (USE_EVENT_NOTIFICATIONS and USE_SEMAPHORES).
 */
static fsp_err_t wait_for_event(sync_objects_t * p_sync, touch_gt911_event_flag_t event)
{
#if (USE_EVENT_NOTIFICATIONS == 1)

#if BSP_CFG_RTOS == 1
    ULONG actual_flags = 0;
    UINT tx_err = tx_event_flags_get(p_sync->p_event_flag,
                                event,
                                TX_OR_CLEAR,
                                &actual_flags,
                                TX_WAIT_FOREVER);

    if((TX_SUCCESS != tx_err) || (actual_flags & (((TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_RX_COMPLETE ) & event) ? TOUCH_GT911_I2C_ERROR : 0)))
    {
        return FSP_ERR_ABORTED;
    }
#elif BSP_CFG_RTOS == 2
    uint32_t ulNotifiedValue = 0;
    UBaseType_t channel = (( TOUCH_GT911_I2C_ERROR | TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_RX_COMPLETE ) & event) ? TOUCH_GT911_I2C_EVENT_CHANNEL : TOUCH_GT911_IRQ_EVENT_CHANNEL;
    BaseType_t xResult = xTaskNotifyWaitIndexed(channel, /* Wait for notification on I2C channel */
                                                event, /* Don't clear any notification bits on entry. */
                                                event, /* Reset the notification value to 0 on exit. */
                                                        &ulNotifiedValue, /* Notified value pass out in ulNotifiedValue. */
                                                        portMAX_DELAY); /* Block indefinitely. */

    if((pdFALSE == xResult)  || (ulNotifiedValue & (((TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_RX_COMPLETE ) & event) ? TOUCH_GT911_I2C_ERROR : 0)))
    {
        return FSP_ERR_TIMEOUT;
    }
    FSP_PARAMETER_NOT_USED(p_sync);
#endif

#endif /* (USE_EVENT_NOTIFICATIONS == 1) */

#if (USE_SEMAPHORES == 1)

    SEMAPHORE_T p_sem = (( TOUCH_GT911_I2C_ERROR | TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_RX_COMPLETE ) & event) ? p_sync->p_semaphore_i2c : p_sync->p_semaphore_irq;
#if BSP_CFG_RTOS == 1
    UINT tx_err = tx_semaphore_get(p_sem, TX_WAIT_FOREVER);
    if(TX_SUCCESS != tx_err)
    {
        return FSP_ERR_ABORTED;
    }
#elif BSP_CFG_RTOS == 2
    BaseType_t xResult = xSemaphoreTake(p_sem, portMAX_DELAY);
    if(pdFALSE == xResult)
    {
        return FSP_ERR_ABORTED;
    }
#endif

#endif /* (USE_SEMAPHORES == 1) */

    return FSP_SUCCESS;
}

/**
 * @brief Callback function for GT911 I2C events.
 *
 * This function is a callback handler for events that occur during touch I2C communication.
 * It is designed to be used with an I2C master module. The function determines the type of
 * event that occurred and sends an appropriate synchronization event based on the event type.
 *
 * @param[in] p_args Pointer to the structure containing callback information.
 *
 * @note This function assumes that it is used in an environment where an RTOS is configured (BSP_CFG_RTOS > 0).
 *       The specific event types (I2C_MASTER_EVENT_TX_COMPLETE, I2C_MASTER_EVENT_RX_COMPLETE, and
 *       I2C_MASTER_EVENT_ABORTED) trigger the sending of corresponding events using the
 *       send_event_from_callback function.
 *
 * @warning This function is designed for internal use, and its behavior depends on the underlying RTOS configuration.
 *          Make sure to configure the RTOS properly to match the intended behavior of the function.
 *
 * @param[in] p_args Pointer to the structure containing callback information.
 * @return None.
 */
void touch_i2c_callback (i2c_master_callback_args_t * p_args)
{
    sync_objects_t * p_sync = (sync_objects_t *) p_args->p_context;

    const touch_gt911_event_flag_t event_map[] =
    {
     [I2C_MASTER_EVENT_ABORTED] = TOUCH_GT911_I2C_ERROR,
     [I2C_MASTER_EVENT_TX_COMPLETE] = TOUCH_GT911_I2C_TX_COMPLETE,
     [I2C_MASTER_EVENT_RX_COMPLETE] = TOUCH_GT911_I2C_RX_COMPLETE,
    };

    if(p_args->event > sizeof(event_map)/sizeof(event_map[0]) || p_args->event < I2C_MASTER_EVENT_ABORTED)
    {
        return;
    }

    send_event_from_callback(p_sync, event_map[p_args->event]);

    return;
}

/**
 * @brief Callback function for touch interrupt events.
 *
 * This function is a callback handler for external interrupt events related to touch functionality.
 * It is designed to be used with an external interrupt module. The function checks the interrupt channel,
 * and if it corresponds to the touch interrupt channel (INT_IRQ_CHANNEL), it sends a synchronization event
 * indicating a touch interrupt using the send_event_from_callback function.
 *
 * @param[in] p_args Pointer to the structure containing callback information.
 *
 * @note This function assumes that it is used in an environment where an RTOS is configured.
 *       The specific event type (TOUCH_GT911_INT) triggers the sending of the corresponding event.
 *
 * @warning This function is designed for internal use, and its behavior depends on the proper configuration
 *          of the external interrupt module and the underlying RTOS.
 *
 * @param[in] p_args Pointer to the structure containing callback information.
 * @return None.
 */
void touch_int_callback (external_irq_callback_args_t * p_args)
{
    sync_objects_t * p_sync = (sync_objects_t *) p_args->p_context;

    if (INT_IRQ_CHANNEL == p_args->channel)
    {
        send_event_from_callback(p_sync, TOUCH_GT911_INT);
    }
    return;
}

/**********************************************************************************************************************
 * Function Name: R_TOUCH_GT911_Validate
 * Description  : Validates if a configuration can be used with the current MCU.
 * Arguments    : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t R_TOUCH_GT911_Validate (touch_gt911_cfg_t const * const p_cfg, uint32_t * const err)
{
    uint32_t error_mask = 1;

#if defined(BOARD_RA8D1_EK)
    error_mask *= (error_mask > 1 || p_cfg->reset_pin == IOPORT_PORT_10_PIN_01) ? 1: 2;
    error_mask *= (error_mask > 1 || p_cfg->i2c_scl_pin == IOPORT_PORT_05_PIN_12) ? 1: 3;
    error_mask *= (error_mask > 1 || p_cfg->i2c_sda_pin == IOPORT_PORT_05_PIN_11) ? 1: 5;
    error_mask *= (error_mask > 1 || p_cfg->irq_pin == IOPORT_PORT_05_PIN_10) ? 1: 7;

    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master != NULL          ) ? 1: 11;
    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_cfg != NULL   ) ? 1: 13;
    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_api != NULL   ) ? 1: 17;
    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_ctrl != NULL  ) ? 1: 19;

    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_cfg->p_extend != NULL                 ) ? 1: 23;
    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_cfg->p_context == NULL                ) ? 1: 29;
    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_cfg->channel == 1                     ) ? 1: 31;
    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_cfg->rate == I2C_MASTER_RATE_STANDARD ) ? 1: 37;

    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_cfg->slave == 0x14                            ) ? 1: 1;
    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_cfg->addr_mode == I2C_MASTER_ADDR_MODE_7BIT   ) ? 1: 41;
    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_cfg->p_callback == touch_i2c_callback         ) ? 1: 43;
    error_mask *= (error_mask > 1 || p_cfg->p_i2c_master->p_cfg->ipl > 0                                  ) ? 1: 47;

    error_mask *= (error_mask > 1 || p_cfg->p_external_irq != NULL        ) ? 1: 53;
    error_mask *= (error_mask > 1 || p_cfg->p_external_irq->p_cfg != NULL ) ? 1: 59;
    error_mask *= (error_mask > 1 || p_cfg->p_external_irq->p_api != NULL ) ? 1: 61;
    error_mask *= (error_mask > 1 || p_cfg->p_external_irq->p_ctrl != NULL) ? 1: 67;

    error_mask *= (error_mask > 1 || p_cfg->p_external_irq->p_cfg->channel == 3                       ) ? 1: 71;
    error_mask *= (error_mask > 1 || p_cfg->p_external_irq->p_cfg->p_context == NULL                  ) ? 1: 73;
    error_mask *= (error_mask > 1 || p_cfg->p_external_irq->p_cfg->p_callback == touch_int_callback   ) ? 1: 79;
    error_mask *= (error_mask > 1 || p_cfg->p_external_irq->p_cfg->trigger == EXTERNAL_IRQ_TRIG_RISING) ? 1: 83;

    error_mask *= (error_mask > 1 || p_cfg->p_external_irq->p_cfg->ipl > 0        ) ? 1: 89;
    error_mask *= (error_mask > 1 || p_cfg->p_ioport != NULL                      ) ? 1: 97;
    error_mask *= (error_mask > 1 || p_cfg->sync != NULL                          ) ? 1: 101;
#if (BSP_CFG_RTOS == 0)

#else

#if (USE_EVENT_NOTIFICATIONS == 1)
    error_mask *= (error_mask > 1 || p_cfg->sync->p_event_flag != NULL) ? 1: 103;

#if (BSP_CFG_RTOS == 1)
    /* Use the information get API in ThreadX to read the status of the event flag */
    {
        CHAR * p_name = NULL;
        TX_THREAD * p_first_suspended;
        TX_EVENT_FLAGS_GROUP * p_next_group;
        ULONG suspended_count;
        ULONG current_flags;
        UINT tx_err = tx_event_flags_info_get(p_cfg->sync->p_event_flag,
                                              &p_name, &current_flags,
                                              &p_first_suspended,
                                              &suspended_count,
                                              &p_next_group);
        error_mask *= (error_mask > 1 || p_name != NULL               ) ? 1: 109;
        error_mask *= (error_mask > 1 || current_flags == 0           ) ? 1: 113;
        error_mask *= (error_mask > 1 || p_first_suspended == NULL    ) ? 1: 127;
        error_mask *= (error_mask > 1 || suspended_count == 0         ) ? 1: 131;
        error_mask *= (error_mask > 1 || tx_err == TX_SUCCESS         ) ? 1: 137;
    }
#endif

#elif (USE_SEMAPHORES == 1)
    error_mask *= (error_mask > 1 || p_cfg->sync->p_semaphore_i2c != NULL) ? 1: 103;
    error_mask *= (error_mask > 1 || p_cfg->sync->p_semaphore_irq != NULL) ? 1: 107;

#if (BSP_CFG_RTOS == 1)
    /* Use the information get API in ThreadX to read the status of the semaphores */
    {
        CHAR * p_name = NULL;
        TX_THREAD * p_first_suspended;
        TX_SEMAPHORE * p_next_semaphore;
        ULONG suspended_count;
        ULONG current_value;

        UINT tx_err = TX_SUCCESS;
        tx_err = tx_semaphore_info_get(p_cfg->sync->p_semaphore_i2c,
                                &p_name, &current_value,
                                &p_first_suspended,
                                &suspended_count,
                                &p_next_semaphore);
        error_mask *= ( error_mask > 1 || p_name != NULL               ) ? 1: 109;
        error_mask *= ( error_mask > 1 || current_value == 0           ) ? 1: 113;
        error_mask *= ( error_mask > 1 || p_first_suspended == NULL    ) ? 1: 127;
        error_mask *= ( error_mask > 1 || suspended_count == 0         ) ? 1: 131;
        error_mask *= ( error_mask > 1 || tx_err == TX_SUCCESS         ) ? 1: 137;

        tx_err = tx_semaphore_info_get(p_cfg->sync->p_semaphore_irq,
                                &p_name, &current_value,
                                &p_first_suspended,
                                &suspended_count,
                                &p_next_semaphore);
        error_mask *= ( error_mask > 1 || p_name != NULL               ) ? 1: 139;
        error_mask *= ( error_mask > 1 || current_value == 0           ) ? 1: 149;
        error_mask *= ( error_mask > 1 || p_first_suspended == NULL    ) ? 1: 151;
        error_mask *= ( error_mask > 1 || suspended_count == 0         ) ? 1: 157;
        error_mask *= ( error_mask > 1 || tx_err == TX_SUCCESS         ) ? 1: 163;
    }
#endif /* (BSP_CFG_RTOS == 1) */

#if (BSP_CFG_RTOS == 2)
    /* Set semaphores back down to zero */
    {
        error_mask *= ( error_mask > 1 || uxSemaphoreGetCount(p_cfg->sync->p_semaphore_i2c) == 0 ) ? 1: 109;
        error_mask *= ( error_mask > 1 || uxSemaphoreGetCount(p_cfg->sync->p_semaphore_irq) == 0 ) ? 1: 113;
    }
#endif /* (BSP_CFG_RTOS == 2) */

#endif /* (USE_SEMAPHORES == 1) || (USE_EVENT_NOTIFICATIONS == 1) */

#endif /* (BSP_CFG_RTOS == 0) */


    * err = (error_mask == 1) ? 0 : error_mask;

    if (error_mask > 1)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }
#endif

    return FSP_SUCCESS;
}




/**********************************************************************************************************************
 * End of function R_EXAMPLE_ApiFunction
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: private_function
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *********************************************************************************************************************/
i2c_master_cfg_t i2c_master;
external_irq_cfg_t irq_setting;

fsp_err_t R_TOUCH_GT911_Open     (touch_gt911_ctrl_t * const p_ctrl, touch_gt911_cfg_t const * const p_cfg)
{

    fsp_err_t err = FSP_SUCCESS;

    {
        uint32_t err_validate = 0;
        err = R_TOUCH_GT911_Validate(p_cfg, &err_validate);
        if (err != FSP_SUCCESS)
            return err;
    }

    memcpy(&i2c_master,  p_cfg->p_i2c_master->p_cfg, sizeof(i2c_master_cfg_t));
    memcpy(&irq_setting,  p_cfg->p_external_irq->p_cfg, sizeof(external_irq_cfg_t));

#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)

#if USE_EVENT_NOTIFICATIONS == 1
    i2c_master.p_context = p_cfg->sync;
    irq_setting.p_context = p_cfg->sync;
    p_ctrl->sync = p_cfg->sync;
    p_ctrl->sync->p_event_flag = p_cfg->sync->p_event_flag;
#elif USE_SEMAPHORES == 1
    irq_setting.p_context = i2c_master.p_context = p_ctrl->sync = p_cfg->sync;
#endif

#elif (BSP_CFG_RTOS == 2)
    irq_setting.p_context = i2c_master.p_context = p_ctrl->sync = p_cfg->sync;
#endif

    ioport_instance_t const * const p_ioport = p_cfg->p_ioport;

    err = p_ioport->p_api->pinCfg(p_ioport->p_ctrl,
                                  (bsp_io_port_pin_t) p_cfg->i2c_scl_pin,
                                  ((uint32_t) IOPORT_CFG_DRIVE_MID_IIC |
                                          (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
                                          (uint32_t) IOPORT_PERIPHERAL_IIC));
    if (err != FSP_SUCCESS)
        return err;

    err = p_ioport->p_api->pinCfg(p_ioport->p_ctrl,
                                  (bsp_io_port_pin_t) p_cfg->i2c_sda_pin,
                                  ((uint32_t) IOPORT_CFG_DRIVE_MID_IIC |
                                          (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
                                          (uint32_t) IOPORT_PERIPHERAL_IIC));
    if (err != FSP_SUCCESS)
        return err;

    p_ctrl->p_ioport = p_ioport;

    p_ctrl->i2c_scl_pin = p_cfg->i2c_scl_pin;
    p_ctrl->i2c_sda_pin = p_cfg->i2c_sda_pin;
    p_ctrl->reset_pin = p_cfg->reset_pin;
    p_ctrl->irq_pin = p_cfg->irq_pin;

    external_irq_instance_t const * const p_irq = p_cfg->p_external_irq;
    i2c_master_instance_t const * const p_i2c = p_cfg->p_i2c_master;
    err = p_irq->p_api->open(p_irq->p_ctrl, &irq_setting);

    if (err != FSP_SUCCESS)
        return err;


    err = p_irq->p_api->enable(p_irq->p_ctrl);
    if (err != FSP_SUCCESS)
        return err;

    p_ctrl->p_external_irq = p_irq;

    err = p_i2c->p_api->open(p_i2c->p_ctrl, &i2c_master);
    if (err != FSP_SUCCESS)
        return err;

    p_ctrl->p_i2c_master = p_i2c;

    memcpy(&p_ctrl->transform_flags, &p_cfg->transform_flags, sizeof(touch_gt911_coord_transform_t));

    p_ctrl->x_max = p_cfg->x_max;
    p_ctrl->y_max = p_cfg->y_max;

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
 * End of function private_function
 *********************************************************************************************************************/


fsp_err_t R_TOUCH_GT911_Reset(touch_gt911_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;

    ioport_instance_t const * const p_ioport = p_ctrl->p_ioport;
    /* Power-ON timing for GT911 */
    {
        /* Enter reset keeping both INT and RESET pin low*/
        {
            err = p_ioport->p_api->pinCfg(p_ioport->p_ctrl,
                                          (bsp_io_port_pin_t) p_ctrl->irq_pin,
                                          ((uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT |
                                                  (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW));
            if (err != FSP_SUCCESS)
                return err;

            err = p_ioport->p_api->pinCfg(p_ioport->p_ctrl,
                                          (bsp_io_port_pin_t) p_ctrl->reset_pin,
                                          ((uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT |
                                                  (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW));
            if (err != FSP_SUCCESS)
                return err;
        }

        /* Wait for 10 ms */
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        UINT tx_err = TX_SUCCESS;
        tx_err = tx_thread_sleep(1);
        if(TX_SUCCESS != tx_err)
            return FSP_ERR_ABORTED;
#elif (BSP_CFG_RTOS == 2)
        {
            TickType_t xDelay = 15 / portTICK_PERIOD_MS;
            TickType_t xLastTick = xTaskGetTickCount();
            vTaskDelayUntil(&xLastTick, xDelay);
        }
#endif

        /* First set INT pin high */
        err = R_IOPORT_PinWrite(p_ioport->p_ctrl, (bsp_io_port_pin_t) p_ctrl->irq_pin, BSP_IO_LEVEL_HIGH);
        if (err != FSP_SUCCESS)
            return err;

        /* Wait for 100 us */
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        tx_err = tx_thread_sleep(1);
        if(TX_SUCCESS != tx_err)
            return FSP_ERR_ABORTED;
#elif (BSP_CFG_RTOS == 2)
        {
            TickType_t xDelay = 1 / portTICK_PERIOD_MS;
            TickType_t xLastTick = xTaskGetTickCount();
            vTaskDelayUntil(&xLastTick, xDelay);
        }
#endif

        /* Then RESET can go high */
        err = R_IOPORT_PinWrite(p_ioport->p_ctrl, (bsp_io_port_pin_t) p_ctrl->reset_pin, BSP_IO_LEVEL_HIGH);
        if (err != FSP_SUCCESS)
            return err;

        /* Wait for 5 ms*/
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        tx_err = tx_thread_sleep(1);
        if(TX_SUCCESS != tx_err)
            return FSP_ERR_ABORTED;
#elif (BSP_CFG_RTOS == 2)
        {
            TickType_t xDelay = 6 / portTICK_PERIOD_MS;
            TickType_t xLastTick = xTaskGetTickCount();
            vTaskDelayUntil(&xLastTick, xDelay);
        }
#endif

        /* Then INT pin can go low */
        R_IOPORT_PinWrite(p_ioport->p_ctrl, (bsp_io_port_pin_t) p_ctrl->irq_pin, BSP_IO_LEVEL_LOW);
        if (err != FSP_SUCCESS)
            return err;

        /* Wait for 50 ms */
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        tx_err = tx_thread_sleep(5);
        if(TX_SUCCESS != tx_err)
            return FSP_ERR_ABORTED;
#elif (BSP_CFG_RTOS == 2)
        {
            TickType_t xDelay = 52 / portTICK_PERIOD_MS;
            TickType_t xLastTick = xTaskGetTickCount();
            vTaskDelayUntil(&xLastTick, xDelay);
        }
#endif

        /* Set IRQ pin as input */
        err = p_ioport->p_api->pinCfg(p_ioport->p_ctrl,
                                      (bsp_io_port_pin_t) p_ctrl->irq_pin,
                                      ((uint32_t) IOPORT_CFG_IRQ_ENABLE |
                                              (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT));
        if (err != FSP_SUCCESS)
            return err;
    }

    return FSP_SUCCESS;
}

fsp_err_t R_TOUCH_GT911_Close    (touch_gt911_ctrl_t * const p_ctrl)
{
    fsp_err_t fsp_err = FSP_SUCCESS;
    i2c_master_instance_t const * const p_i2c = p_ctrl->p_i2c_master;
    external_irq_instance_t const * const p_irq = p_ctrl->p_external_irq;

    fsp_err = p_i2c->p_api->close(p_i2c->p_ctrl);

    if (fsp_err != FSP_SUCCESS)
        return fsp_err;

    fsp_err = p_irq->p_api->disable(p_irq->p_ctrl);
    if (fsp_err != FSP_SUCCESS)
        return fsp_err;

    fsp_err = p_irq->p_api->close(p_irq->p_ctrl);

    if (fsp_err != FSP_SUCCESS)
        return fsp_err;

#if (BSP_CFG_RTOS == 0)

#else

#if (USE_EVENT_NOTIFICATIONS == 1)


#if (BSP_CFG_RTOS == 1)
    /* Clear all pending event flags */
    {
        ULONG actual_flags = 0;
        ULONG event = (TOUCH_GT911_I2C_ERROR | TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_RX_COMPLETE | TOUCH_GT911_INT);
        UINT tx_err = tx_event_flags_get(p_ctrl->sync->p_event_flag,
                                         event,
                                         TX_OR_CLEAR,
                                         &actual_flags,
                                         TX_NO_WAIT);
        FSP_PARAMETER_NOT_USED(tx_err);
        FSP_PARAMETER_NOT_USED(actual_flags);
    }
#endif /* (BSP_CFG_RTOS == 1) */

#if (BSP_CFG_RTOS == 2)
    /* Clear all pending notifications */
    {
        uint32_t value = 0;
        value = ulTaskNotifyValueClearIndexed( p_ctrl->sync->p_event_flag,
                                               TOUCH_GT911_I2C_EVENT_CHANNEL,
                                               (uint32_t) (TOUCH_GT911_I2C_ERROR | TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_RX_COMPLETE) );

        value = ulTaskNotifyValueClearIndexed( p_ctrl->sync->p_event_flag,
                                               TOUCH_GT911_IRQ_EVENT_CHANNEL,
                                               (uint32_t) (TOUCH_GT911_INT) );
        FSP_PARAMETER_NOT_USED(value);
    }
#endif /* (BSP_CFG_RTOS == 2) */

#elif (USE_SEMAPHORES == 1)


#if (BSP_CFG_RTOS == 1)
    /* Set the semaphore values back down to zero */
    {
        CHAR * p_name = NULL;
        TX_THREAD * p_first_suspended;
        TX_SEMAPHORE * p_next_semaphore;
        ULONG suspended_count;
        ULONG current_value;

        UINT tx_err = TX_SUCCESS;
        tx_err = tx_semaphore_info_get(p_cfg->sync->p_semaphore_i2c,
                                &p_name, &current_value,
                                &p_first_suspended,
                                &suspended_count,
                                &p_next_semaphore);
        error_mask *= error_mask > 1 || p_name != NULL               ) ? 1: 109;
        error_mask *= error_mask > 1 || current_value == 0           ) ? 1: 113;
        error_mask *= error_mask > 1 || p_first_suspended == NULL    ) ? 1: 127;
        error_mask *= error_mask > 1 || suspended_count == 0         ) ? 1: 131;
        error_mask *= error_mask > 1 || tx_err == TX_SUCCESS         ) ? 1: 137;

        tx_err = tx_semaphore_info_get(p_cfg->sync->p_semaphore_irq,
                                &p_name, &current_value,
                                &p_first_suspended,
                                &suspended_count,
                                &p_next_semaphore);
        error_mask *= (error_mask > 1 || p_name != NULL               ) ? 1: 139;
        error_mask *= (error_mask > 1 || current_value == 0           ) ? 1: 149;
        error_mask *= (error_mask > 1 || p_first_suspended == NULL    ) ? 1: 151;
        error_mask *= (error_mask > 1 || suspended_count == 0         ) ? 1: 157;
        error_mask *= (error_mask > 1 || tx_err == TX_SUCCESS         ) ? 1: 163;
    }
#endif /* (BSP_CFG_RTOS == 1) */

#if (BSP_CFG_RTOS == 2)
    /* Set semaphores back down to zero */
    {
        while(uxSemaphoreGetCount(p_ctrl->sync->p_semaphore_i2c) > 0)
        {
            if (xSemaphoreTake( p_ctrl->sync->p_semaphore_i2c, 0 ) != pdTRUE)
            {
                break;
            }

        }
        while(uxSemaphoreGetCount(p_ctrl->sync->p_semaphore_irq) > 0)
        {
            if (xSemaphoreTake( p_ctrl->sync->p_semaphore_irq, 0 ) != pdTRUE)
            {
                break;
            }
        }
    }
#endif /* (BSP_CFG_RTOS == 2) */

#endif /* (USE_EVENT_NOTIFICATIONS == 1) || (USE_SEMAPHORES == 1) */

#endif /* (BSP_CFG_RTOS == 0) */

    memset(p_ctrl, 0, sizeof(touch_gt911_ctrl_t));

    return FSP_SUCCESS;
}

fsp_err_t R_TOUCH_GT911_StatusGet(touch_gt911_ctrl_t * const p_ctrl, uint8_t * p_status, bool clear_after_read)
{
    fsp_err_t fsp_err = FSP_SUCCESS;


    i2c_master_instance_t const * const p_i2c = p_ctrl->p_i2c_master;

    uint8_t status = 0;

    uint16_t set_register_address = (uint16_t)(__REV16((uint16_t)GT911_STATUS) & UINT16_MAX);

    fsp_err = p_i2c->p_api->write(p_i2c->p_ctrl,
                                  (uint8_t * const) &set_register_address,
                                  sizeof(set_register_address),
                                  false);

    if (fsp_err != FSP_SUCCESS)
            return fsp_err;

    /* Wait for I2C Write to complete */
    {
        fsp_err = wait_for_event(p_ctrl->sync, TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_ERROR);

        if (fsp_err != FSP_SUCCESS)
            return fsp_err;
    }

    fsp_err = p_i2c->p_api->read(p_i2c->p_ctrl,
                                 (uint8_t * const)&status,
                                 sizeof(status),
                                 false);

    if (fsp_err != FSP_SUCCESS)
        return fsp_err;

    /* Wait for I2C read to complete */
    {
        fsp_err = wait_for_event(p_ctrl->sync, TOUCH_GT911_I2C_RX_COMPLETE | TOUCH_GT911_I2C_ERROR);

        if (fsp_err != FSP_SUCCESS)
            return fsp_err;
    }

    * p_status = status;

    if(clear_after_read == true)
    {
        uint8_t clear_status_register[sizeof(set_register_address) + 1] =
                {
                 set_register_address & 0xFFU,
                 (uint8_t)((set_register_address &0xFF00U)>> 8),
                 0x00,
                };



        fsp_err = p_i2c->p_api->write(p_i2c->p_ctrl,
                                      (uint8_t * const) &clear_status_register,
                                      sizeof(clear_status_register),
                                      false);

        if (fsp_err != FSP_SUCCESS)
            return fsp_err;

        /* Wait for I2C Write to complete */
        {
            fsp_err = wait_for_event(p_ctrl->sync, TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_ERROR);

            if (fsp_err != FSP_SUCCESS)
                return fsp_err;
        }
    }

    return FSP_SUCCESS;
}

fsp_err_t R_TOUCH_GT911_PointsGet(touch_gt911_ctrl_t * const p_ctrl, touch_gt911_data_t * p_data)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    i2c_master_instance_t const * const p_i2c = p_ctrl->p_i2c_master;

    gt911_point_payload_t payload[MAX_TOUCH_POINTS];

    memset(&payload[0], 0, sizeof(payload));

    uint16_t set_register_address = (uint16_t)(__REV16((uint16_t)GT911_COORDINATE_START) & UINT16_MAX);

    fsp_err = p_i2c->p_api->write(p_i2c->p_ctrl,
                                  (uint8_t * const) &set_register_address,
                                  sizeof(set_register_address),
                                  false);

    if (fsp_err != FSP_SUCCESS)
            return fsp_err;

    /* Wait for I2C Write to complete */
    {
        fsp_err = wait_for_event(p_ctrl->sync, TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_ERROR);

        if (fsp_err != FSP_SUCCESS)
            return fsp_err;
    }

    fsp_err = p_i2c->p_api->read(p_i2c->p_ctrl,
                                 (uint8_t * const)&payload,
                                 sizeof(payload),
                                 false);

    if (fsp_err != FSP_SUCCESS)
        return fsp_err;

    /* Wait for I2C read to complete */
    {
        fsp_err = wait_for_event(p_ctrl->sync, TOUCH_GT911_I2C_RX_COMPLETE | TOUCH_GT911_I2C_ERROR);

        if (fsp_err != FSP_SUCCESS)
            return fsp_err;
    }

    for(uint32_t itr = 0; (itr < p_data->touch_count) && (itr < MAX_TOUCH_POINTS) ; itr++)
    {

        uint32_t x = p_data->point[itr].x = (p_ctrl->transform_flags & TOUCH_GT911_COORD_ROTATE_X) ? p_ctrl->x_max - payload[itr].x : payload[itr].x;
        uint32_t y = p_data->point[itr].y = (p_ctrl->transform_flags & TOUCH_GT911_COORD_ROTATE_Y) ? p_ctrl->y_max - payload[itr].y : payload[itr].y;
        if(p_ctrl->transform_flags & TOUCH_GT911_COORD_EXCHANGE)
        {
            p_data->point[itr].x = y;
            p_data->point[itr].y = x;
        }

        p_data->point[itr].size = payload[itr].point_size;
    }

    return FSP_SUCCESS;
}


fsp_err_t R_TOUCH_GT911_VersionGet(touch_gt911_ctrl_t * const p_ctrl, uint16_t * const p_fw_version)
{
    fsp_err_t fsp_err = FSP_SUCCESS;


    i2c_master_instance_t const * const p_i2c = p_ctrl->p_i2c_master;

    chip_info_t chip_info;

    memset(&chip_info, 0, sizeof(chip_info));

    uint16_t set_register_address = (uint16_t)(__REV16((uint16_t)GT911_PRODUCT_ID_FIRST_BYTE) & UINT16_MAX);

    fsp_err = p_i2c->p_api->write(p_i2c->p_ctrl,
                                  (uint8_t * const) &set_register_address,
                                  sizeof(set_register_address),
                                  false);

    if (fsp_err != FSP_SUCCESS)
            return fsp_err;

    /* Wait for I2C Write to complete */
    {
        fsp_err = wait_for_event(p_ctrl->sync, TOUCH_GT911_I2C_TX_COMPLETE | TOUCH_GT911_I2C_ERROR);

        if (fsp_err != FSP_SUCCESS)
            return fsp_err;
    }

    fsp_err = p_i2c->p_api->read(p_i2c->p_ctrl,
                                 (uint8_t * const)&chip_info,
                                 sizeof(chip_info),
                                 false);

    if (fsp_err != FSP_SUCCESS)
        return fsp_err;

    /* Wait for I2C read to complete */
    {
        fsp_err = wait_for_event(p_ctrl->sync, TOUCH_GT911_I2C_RX_COMPLETE | TOUCH_GT911_I2C_ERROR);

        if (fsp_err != FSP_SUCCESS)
            return fsp_err;
    }

    * p_fw_version = chip_info.firmware_version;

    return FSP_SUCCESS;
}


fsp_err_t R_TOUCH_GT911_WaitForTouch(touch_gt911_ctrl_t * const p_ctrl)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = wait_for_event(p_ctrl->sync, TOUCH_GT911_INT);

    return fsp_err;
}
