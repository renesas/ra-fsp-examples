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
 * File Name    : touch_gt911.c
 * Version      : 1.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2020 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "touch_gt911/touch_gt911.h"
#include "touch_thread.h"
#include "system_thread.h"
/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#define GT911_DOWN          0
#define GT911_UP            1
#define GT911_CONTACT       2

#define extract_e(t) ((uint8_t) ((t).event))
#define extract_x(t) ((int16_t) (((t).x_msb << 8) | ((t).x_lsb)))
#define extract_y(t) ((int16_t) (((t).y_msb << 8) | ((t).y_lsb)))

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
void touch_i2c_callback (i2c_master_callback_args_t * p_args)
{

    switch (p_args->event)
    {
        case I2C_MASTER_EVENT_TX_COMPLETE:
        {
#if (BSP_CFG_RTOS == 0)
            * p_flags = TOUCH_GT911_I2C_TX_COMPLETE;
#elif (BSP_CFG_RTOS == 1)

#if USE_EVENT_FLAGS == 1
            EVENT_FLAG_T * p_flags = (EVENT_FLAG_T *)p_args->p_context;
            UINT err = tx_event_flags_set(p_flags, TOUCH_GT911_I2C_TX_COMPLETE, TX_OR);
            FSP_PARAMETER_NOT_USED(err);
#elif USE_SEMAPHORES == 1
            sync_objects_t * p_sync = (sync_objects_t *) p_args->p_context;
            UINT tx_err = tx_semaphore_put(p_sync->p_semaphore_i2c);
            FSP_PARAMETER_NOT_USED(TX_SUCCESS == tx_err);
#endif

#elif (BSP_CFG_RTOS == 2)

#endif
        }
        break;
        case I2C_MASTER_EVENT_RX_COMPLETE:
        {
#if (BSP_CFG_RTOS == 0)
            * p_flags = TOUCH_GT911_I2C_RX_COMPLETE;
#elif (BSP_CFG_RTOS == 1)

#if USE_EVENT_FLAGS == 1
            EVENT_FLAG_T * p_flags = (EVENT_FLAG_T *)p_args->p_context;
            UINT err = tx_event_flags_set(p_flags, TOUCH_GT911_I2C_RX_COMPLETE, TX_OR);
            FSP_PARAMETER_NOT_USED(err);
#elif USE_SEMAPHORES == 1
            sync_objects_t * p_sync = (sync_objects_t *) p_args->p_context;
            UINT tx_err = tx_semaphore_put(p_sync->p_semaphore_i2c);
            FSP_PARAMETER_NOT_USED(TX_SUCCESS == tx_err);
#endif

#elif (BSP_CFG_RTOS == 2)

#endif
        }
        break;
        case I2C_MASTER_EVENT_ABORTED:
        {
#if (BSP_CFG_RTOS == 0)
            * p_flags = TOUCH_GT911_I2C_ERROR;
#elif (BSP_CFG_RTOS == 1)

#if USE_EVENT_FLAGS == 1
            EVENT_FLAG_T * p_flags = (EVENT_FLAG_T *)p_args->p_context;
            UINT err = tx_event_flags_set(p_flags, TOUCH_GT911_I2C_ERROR, TX_OR);
            FSP_PARAMETER_NOT_USED(err);
#elif USE_SEMAPHORES == 1
            sync_objects_t * p_sync = (sync_objects_t *) p_args->p_context;
            UINT tx_err = tx_semaphore_put(p_sync->p_semaphore_i2c);
            FSP_PARAMETER_NOT_USED(TX_SUCCESS == tx_err);
#endif

#elif (BSP_CFG_RTOS == 2)

#endif
        }
        break;

        default:
        {
            ;
        }
        break;
    }
    return;
}



void touch_int_callback (external_irq_callback_args_t * p_args)
{

    switch (p_args->channel)
    {

        case INT_IRQ_CHANNEL:
        {
#if (BSP_CFG_RTOS == 1)

#if USE_EVENT_FLAGS == 1
            TX_EVENT_FLAGS_GROUP * p_flags = (TX_EVENT_FLAGS_GROUP *) p_args->p_context;
            UINT err = tx_event_flags_set(p_flags, TOUCH_GT911_INT, TX_OR);
            FSP_PARAMETER_NOT_USED(err);
#elif USE_SEMAPHORES == 1
            sync_objects_t * p_sync = (sync_objects_t *) p_args->p_context;
            UINT tx_err = tx_semaphore_put(p_sync->p_semaphore_irq);
            FSP_PARAMETER_NOT_USED(TX_SUCCESS == tx_err);
#endif

#elif (BSP_CFG_RTOS == 2)

#endif
        }
        break;

        default:
        {
            ;
        }
        break;
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
    uint32_t error_mask = 0;
    uint32_t error_value = 0;
#if defined(BOARD_RA8E2_EK)
    error_mask |= (p_cfg->reset_pin == IOPORT_PORT_10_PIN_01) ? 0: 1U << error_value;
    error_value++;
    error_mask |= (p_cfg->i2c_scl_pin == IOPORT_PORT_05_PIN_12) ? 0: 1U << error_value;
    error_value++;
    error_mask |= (p_cfg->i2c_sda_pin == IOPORT_PORT_05_PIN_11) ? 0: 1U << error_value;
    error_value++;
    error_mask |= (p_cfg->irq_pin == IOPORT_PORT_05_PIN_10) ? 0: 1U << error_value;
    error_value++;

    error_mask |= p_cfg->p_i2c_master != NULL ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_cfg != NULL ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_api != NULL ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_ctrl != NULL ? 0 : 1;

    error_mask |= p_cfg->p_i2c_master->p_cfg->p_extend != NULL ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_cfg->p_context == NULL ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_cfg->channel == 1 ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_cfg->rate == I2C_MASTER_RATE_STANDARD ? 0 : 1;

    error_mask |= p_cfg->p_i2c_master->p_cfg->slave == 0x14 ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_cfg->addr_mode == I2C_MASTER_ADDR_MODE_7BIT ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_cfg->p_callback == touch_i2c_callback ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_cfg->ipl > 0 ? 0 : 1;

#if 0
    error_mask |= p_cfg->p_i2c_master->p_cfg->rxi_irq > 0 ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_cfg->txi_irq > 0 ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_cfg->tei_irq > 0 ? 0 : 1;
    error_mask |= p_cfg->p_i2c_master->p_cfg->eri_irq > 0 ? 0 : 1;
#endif

    error_mask |= p_cfg->p_external_irq != NULL ? 0 : 1;
    error_mask |= p_cfg->p_external_irq->p_cfg != NULL ? 0 : 1;
    error_mask |= p_cfg->p_external_irq->p_api != NULL ? 0 : 1;
    error_mask |= p_cfg->p_external_irq->p_ctrl != NULL ? 0 : 1;

    error_mask |= p_cfg->p_external_irq->p_cfg->channel == 3 ? 0 : 1;
    error_mask |= p_cfg->p_external_irq->p_cfg->p_context == NULL ? 0 : 1;
    error_mask |= p_cfg->p_external_irq->p_cfg->p_callback == touch_int_callback ? 0 : 1;
    error_mask |= p_cfg->p_external_irq->p_cfg->trigger == EXTERNAL_IRQ_TRIG_RISING ? 0 : 1;

    error_mask |= p_cfg->p_external_irq->p_cfg->ipl > 0 ? 0 : 1;
#if 0
    error_mask |= p_cfg->p_external_irq->p_cfg->irq > 0 ? 0 : 1;
#endif
    error_mask |= p_cfg->p_ioport != NULL ? 0 : 1;
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
    {
#if USE_EVENT_FLAGS == 1
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
        error_mask |= p_name != NULL ? 0 : 1;
        error_mask |= current_flags == 0 ? 0 : 1;
        error_mask |= p_first_suspended == NULL ? 0 : 1;
        error_mask |= suspended_count == 0 ? 0 : 1;
        error_mask |= tx_err == TX_SUCCESS ? 0 : 1;
#elif USE_SEMAPHORES == 1
        error_mask |= p_cfg->sync != NULL ? 0 : 1;
#endif
    }
#elif (BSP_CFG_RTOS == 2)

#endif


    * err = error_mask;

    if (error_mask > 0)
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

#if USE_EVENT_FLAGS == 1
    i2c_master.p_context = p_cfg->sync->p_event_flag;
    irq_setting.p_context = p_cfg->sync->p_event_flag;
    p_ctrl->sync->p_event_flag = p_cfg->sync->p_event_flag;
#elif USE_SEMAPHORES == 1
    irq_setting.p_context = i2c_master.p_context = p_ctrl->sync = p_cfg->sync;
    {
//        UINT tx_err = TX_SUCCESS;
//        tx_err = tx_semaphore_put(p_ctrl->sync->p_semaphore_irq);
//        tx_err = tx_semaphore_put(p_ctrl->sync->p_semaphore_i2c);
    }
#endif

#elif (BSP_CFG_RTOS == 2)

#endif

    ioport_instance_t const * const p_ioport = p_cfg->p_ioport;

    err = p_ioport->p_api->pinCfg(p_ioport->p_ctrl,
                                  (bsp_io_port_pin_t)p_cfg->i2c_scl_pin,
                                  ((uint32_t) IOPORT_CFG_DRIVE_MID_IIC |
                                          (uint32_t) IOPORT_CFG_PERIPHERAL_PIN |
                                          (uint32_t) IOPORT_PERIPHERAL_IIC));
    if (err != FSP_SUCCESS)
        return err;

    err = p_ioport->p_api->pinCfg(p_ioport->p_ctrl,
                                  (bsp_io_port_pin_t)p_cfg->i2c_sda_pin,
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

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
 * End of function private_function
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: private_function
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t R_TOUCH_GT911_Reset(touch_gt911_ctrl_t * const p_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    UINT tx_err = TX_SUCCESS;

    ioport_instance_t const * const p_ioport = p_ctrl->p_ioport;
    /* Power-ON timing for GT911 */
    {
        /* Enter reset keeping both INT and RESET pin low*/
        {
            err = p_ioport->p_api->pinCfg(p_ioport->p_ctrl,
                                          (bsp_io_port_pin_t)p_ctrl->irq_pin,
                                          ((uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT |
                                                  (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW));
            if (err != FSP_SUCCESS)
                return err;

            err = p_ioport->p_api->pinCfg(p_ioport->p_ctrl,
                                          (bsp_io_port_pin_t)p_ctrl->reset_pin,
                                          ((uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT |
                                                  (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW));
            if (err != FSP_SUCCESS)
                return err;
        }

        /* Wait for 10 ms */
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        tx_err = tx_thread_sleep(1);
        if(TX_SUCCESS != tx_err)
            return FSP_ERR_ABORTED;
#elif (BSP_CFG_RTOS == 2)

#endif

        /* First set INT pin high */
        err = R_IOPORT_PinWrite(p_ioport->p_ctrl, (bsp_io_port_pin_t)p_ctrl->irq_pin, BSP_IO_LEVEL_HIGH);
        if (err != FSP_SUCCESS)
            return err;

        /* Wait for 100 us */
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        tx_err = tx_thread_sleep(1);
        if(TX_SUCCESS != tx_err)
            return FSP_ERR_ABORTED;
#elif (BSP_CFG_RTOS == 2)

#endif

        /* Then RESET can go high */
        err = R_IOPORT_PinWrite(p_ioport->p_ctrl, (bsp_io_port_pin_t)p_ctrl->reset_pin, BSP_IO_LEVEL_HIGH);
        if (err != FSP_SUCCESS)
            return err;

        /* Wait for 5 ms*/
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        tx_err = tx_thread_sleep(1);
        if(TX_SUCCESS != tx_err)
            return FSP_ERR_ABORTED;
#elif (BSP_CFG_RTOS == 2)

#endif

        /* Then INT pin can go low */
        R_IOPORT_PinWrite(p_ioport->p_ctrl, (bsp_io_port_pin_t)p_ctrl->irq_pin, BSP_IO_LEVEL_LOW);
        if (err != FSP_SUCCESS)
            return err;

        /* Wait for 50 ms */
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        tx_err = tx_thread_sleep(5);
        if(TX_SUCCESS != tx_err)
            return FSP_ERR_ABORTED;
#elif (BSP_CFG_RTOS == 2)

#endif

        /* Set IRQ pin as input */
        err = p_ioport->p_api->pinCfg(p_ioport->p_ctrl,
                                      (bsp_io_port_pin_t)p_ctrl->irq_pin,
                                      ((uint32_t) IOPORT_CFG_IRQ_ENABLE |
                                              (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT));
        if (err != FSP_SUCCESS)
            return err;
    }

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
 * End of function private_function
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: private_function
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *********************************************************************************************************************/
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

    memset(p_ctrl, 0, sizeof(touch_gt911_ctrl_t));

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
 * End of function private_function
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_TOUCH_GT911_StatusGet
 * Description  : Returns the status register read from GT911 device.
 * Arguments    : .
 * Return Value : .
 *********************************************************************************************************************/
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
#if BSP_CFG_RTOS == 0

#elif BSP_CFG_RTOS == 1
        UINT tx_err = TX_SUCCESS;
#if USE_EVENT_FLAGS == 1
        ULONG actual_flags = 0;
        tx_err = tx_event_flags_get(p_ctrl->sync->p_event_flag,
                                    TOUCH_GT911_I2C_TX_COMPLETE|TOUCH_GT911_I2C_ERROR,
                                    TX_OR_CLEAR,
                                    &actual_flags,
                                    TX_WAIT_FOREVER);

        if((TX_SUCCESS != tx_err) || (actual_flags & TOUCH_GT911_I2C_ERROR))
        {
            return FSP_ERR_ABORTED;
        }
#elif USE_SEMAPHORES == 1
        tx_err = tx_semaphore_get(p_ctrl->sync->p_semaphore_i2c, TX_WAIT_FOREVER);
        if(TX_SUCCESS != tx_err)
        {
            return FSP_ERR_ABORTED;
        }
#endif

#elif BSP_CFG_RTOS == 2

#endif
    }

    fsp_err = p_i2c->p_api->read(p_i2c->p_ctrl,
                                 (uint8_t * const)&status,
                                 sizeof(status),
                                 false);

    if (fsp_err != FSP_SUCCESS)
        return fsp_err;
    /* Wait for I2C read to complete */
    {
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        UINT tx_err = TX_SUCCESS;
#if USE_EVENT_FLAGS == 1
        ULONG actual_flags = 0;
        tx_err = tx_event_flags_get(p_ctrl->sync->p_event_flag,
                                    TOUCH_GT911_I2C_RX_COMPLETE|TOUCH_GT911_I2C_ERROR,
                                    TX_OR_CLEAR,
                                    &actual_flags,
                                    TX_WAIT_FOREVER);


        if((TX_SUCCESS != tx_err) || (actual_flags & TOUCH_GT911_I2C_ERROR))
        {
            return FSP_ERR_ABORTED;
        }
#elif USE_SEMAPHORES == 1
        tx_err = tx_semaphore_get(p_ctrl->sync->p_semaphore_i2c, TX_WAIT_FOREVER);
        if(TX_SUCCESS != tx_err)
        {
            return FSP_ERR_ABORTED;
        }
#endif

#elif (BSP_CFG_RTOS == 2)

#endif
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
#if BSP_CFG_RTOS == 0

#elif BSP_CFG_RTOS == 1
            UINT tx_err = TX_SUCCESS;
#if USE_EVENT_FLAGS == 1
            ULONG actual_flags = 0;
            tx_err = tx_event_flags_get(p_ctrl->sync->p_event_flag,
                                        TOUCH_GT911_I2C_TX_COMPLETE|TOUCH_GT911_I2C_ERROR,
                                        TX_OR_CLEAR,
                                        &actual_flags,
                                        TX_WAIT_FOREVER);

            if((TX_SUCCESS != tx_err) || (actual_flags & TOUCH_GT911_I2C_ERROR))
            {
                return FSP_ERR_ABORTED;
            }

#elif USE_SEMAPHORES == 1
            tx_err = tx_semaphore_get(p_ctrl->sync->p_semaphore_i2c, TX_WAIT_FOREVER);
            if(TX_SUCCESS != tx_err)
            {
                return FSP_ERR_ABORTED;
            }
#endif

#elif BSP_CFG_RTOS == 2

#endif
        }
    }

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
 * End of function private_function
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_TOUCH_GT911_PointsGet
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t R_TOUCH_GT911_PointsGet(touch_gt911_ctrl_t * const p_ctrl, touch_gt911_data_t * p_data)
{

    fsp_err_t fsp_err = FSP_SUCCESS;

    i2c_master_instance_t const * const p_i2c = p_ctrl->p_i2c_master;

    gt911_point_payload_t payload[5];

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
#if BSP_CFG_RTOS == 0

#elif BSP_CFG_RTOS == 1
        UINT tx_err = TX_SUCCESS;
#if USE_EVENT_FLAGS == 1
        ULONG actual_flags = 0;
        tx_err = tx_event_flags_get(p_ctrl->sync->p_event_flag,
                                    TOUCH_GT911_I2C_TX_COMPLETE|TOUCH_GT911_I2C_ERROR,
                                    TX_OR_CLEAR,
                                    &actual_flags,
                                    TX_WAIT_FOREVER);

        if((TX_SUCCESS != tx_err) || (actual_flags & TOUCH_GT911_I2C_ERROR))
        {
            return FSP_ERR_ABORTED;
        }
#elif USE_SEMAPHORES == 1
        tx_err = tx_semaphore_get(p_ctrl->sync->p_semaphore_i2c, TX_WAIT_FOREVER);
        if(TX_SUCCESS != tx_err)
        {
            return FSP_ERR_ABORTED;
        }
#endif

#elif BSP_CFG_RTOS == 2

#endif
    }

    fsp_err = p_i2c->p_api->read(p_i2c->p_ctrl,
                                 (uint8_t * const)&payload,
                                 sizeof(payload),
                                 false);

    if (fsp_err != FSP_SUCCESS)
        return fsp_err;
    /* Wait for I2C read to complete */
    {
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        UINT tx_err = TX_SUCCESS;
#if USE_EVENT_FLAGS == 1
        ULONG actual_flags = 0;
        tx_err = tx_event_flags_get(p_ctrl->sync->p_event_flag,
                                    TOUCH_GT911_I2C_RX_COMPLETE|TOUCH_GT911_I2C_ERROR,
                                    TX_OR_CLEAR,
                                    &actual_flags,
                                    TX_WAIT_FOREVER);


        if((TX_SUCCESS != tx_err) || (actual_flags & TOUCH_GT911_I2C_ERROR))
        {
            return FSP_ERR_ABORTED;
        }
#elif USE_SEMAPHORES == 1
        tx_err = tx_semaphore_get(p_ctrl->sync->p_semaphore_i2c, TX_WAIT_FOREVER);
        if(TX_SUCCESS != tx_err)
        {
            return FSP_ERR_ABORTED;
        }
#endif

#elif (BSP_CFG_RTOS == 2)

#endif
    }

    for(uint32_t itr = 0; (itr < p_data->touch_count) && (itr < MAX_TOUCH_POINTS) ; itr++)
    {
        p_data->point[itr].x = payload[itr].x;
        p_data->point[itr].y = payload[itr].y;
        p_data->point[itr].size = payload[itr].point_size;
    }

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
 * End of function private_function
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_TOUCH_GT911_PointsGet
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *********************************************************************************************************************/
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
#if BSP_CFG_RTOS == 0

#elif BSP_CFG_RTOS == 1
        UINT tx_err = TX_SUCCESS;
#if USE_EVENT_FLAGS == 1
        ULONG actual_flags = 0;
        tx_err = tx_event_flags_get(p_ctrl->sync->p_event_flag,
                                    TOUCH_GT911_I2C_TX_COMPLETE|TOUCH_GT911_I2C_ERROR,
                                    TX_OR_CLEAR,
                                    &actual_flags,
                                    TX_WAIT_FOREVER);

        if((TX_SUCCESS != tx_err) || (actual_flags & TOUCH_GT911_I2C_ERROR))
        {
            return FSP_ERR_ABORTED;
        }
#elif USE_SEMAPHORES == 1
        tx_err = tx_semaphore_get(p_ctrl->sync->p_semaphore_i2c, TX_WAIT_FOREVER);
        if(TX_SUCCESS != tx_err)
        {
            return FSP_ERR_ABORTED;
        }
#endif

#elif BSP_CFG_RTOS == 2

#endif
    }

    fsp_err = p_i2c->p_api->read(p_i2c->p_ctrl,
                                 (uint8_t * const)&chip_info,
                                 sizeof(chip_info),
                                 false);

    if (fsp_err != FSP_SUCCESS)
        return fsp_err;

    /* Wait for I2C read to complete */
    {
#if (BSP_CFG_RTOS == 0)

#elif (BSP_CFG_RTOS == 1)
        UINT tx_err = TX_SUCCESS;
#if USE_EVENT_FLAGS == 1
        ULONG actual_flags = 0;
        tx_err = tx_event_flags_get(p_ctrl->sync->p_event_flag,
                                    TOUCH_GT911_I2C_RX_COMPLETE|TOUCH_GT911_I2C_ERROR,
                                    TX_OR_CLEAR,
                                    &actual_flags,
                                    TX_WAIT_FOREVER);


        if((TX_SUCCESS != tx_err) || (actual_flags & TOUCH_GT911_I2C_ERROR))
        {
            return FSP_ERR_ABORTED;
        }
#elif USE_SEMAPHORES == 1
        tx_err = tx_semaphore_get(p_ctrl->sync->p_semaphore_i2c, TX_WAIT_FOREVER);
        if(TX_SUCCESS != tx_err)
        {
            return FSP_ERR_ABORTED;
        }
#endif

#elif (BSP_CFG_RTOS == 2)

#endif
    }

    * p_fw_version = chip_info.firmware_version;

    return FSP_SUCCESS;
}
/**********************************************************************************************************************
 * End of function private_function
 *********************************************************************************************************************/
