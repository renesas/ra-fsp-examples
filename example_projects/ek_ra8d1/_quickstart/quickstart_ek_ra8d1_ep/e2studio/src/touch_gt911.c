/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : touch_gt911.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include <string.h>
#include <stdio.h>

#include "common_data.h"
#include "common_init.h"
#include "touch_gt911.h"
#include "r_typedefs.h"

#define GT_911_I2C_ADDRESS_0x5D    (0x5D)
#define GT_911_I2C_ADDRESS_0x14    (0x14)

#define GT_911_I2C_ADDRESS         (GT_911_I2C_ADDRESS_0x14)

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define GT911_DOWN                 (0)
#define GT911_UP                   (1)
#define GT911_CONTACT              (2)

#define GT911_REG_TD_STATUS        (0x02)

#define DSP_INT_PIN                (BSP_IO_PORT_05_PIN_10)
#define DSP_RST_PIN                (BSP_IO_PORT_10_PIN_01)
#define DSP_BLEN_PIN               (BSP_IO_PORT_04_PIN_04)

#define MIPI_MIPI_DSI_TE           (BSP_IO_PORT_02_PIN_06)

#define extract_e(t)    ((uint8_t) ((t).event))
#define extract_x(t)    ((int16_t) (((t).x_msb << 8) | ((t).x_lsb)))
#define extract_y(t)    ((int16_t) (((t).y_msb << 8) | ((t).y_lsb)))

extern byte wrSensorReg16_8(int regID, int regDat);
extern byte rdSensorReg16_8(uint16_t regID, uint8_t * regDat);
extern byte rdSensorReg16_Multi(uint16_t regID, uint8_t * regDat, uint32_t len);

/**********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************/

/* Driver-specific touch point register mapping */
typedef struct st_GT911_touch
{
    uint8_t x_msb : 4;
    uint8_t       : 2;
    uint8_t event : 2;
    uint8_t x_lsb;

    uint8_t y_msb : 4;
    uint8_t id    : 4;
    uint8_t y_lsb : 8;

    uint8_t res1;
    uint8_t res2;
} GT911_touch_t;

/* Complete GT911 data payload (number of active points + all five touch points) */
typedef struct st_GT911_payload
{
    uint8_t       num_points_active;
    GT911_touch_t data_raw[GT911_NUM_POINTS];
} GT911_payload_t;

#define byte                  uint8_t

#define GOODIX_REG_COMMAND    (0x8040)
static char product_id[5];

// #define ES_DEMO (1)

/* Read from a virgin ER45RA-MW276-C LCD */
/* reset cfg version o 'A' 0x00 1st bit */
static const uint8_t g911xFW[] =
{
    0x00, 0xe0, 0x01, 0x56, 0x03, 0x05, 0x05, 0x00, 0x01, 0x08, 0x28, 0x05, 0x50,
    0x32, 0x03, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x86, 0x27, 0x08, 0x17, 0x15, 0x31, 0x0d, 0x00, 0x00, 0x02, 0xbb, 0x03,
    0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x64, 0x32, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x10, 0x0e, 0x0c, 0x0a,
    0x08, 0x06, 0x04, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24,
    0x22, 0x21, 0x20, 0x1f, 0x1e, 0x1d, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00
};

char_t g_ts_buffer[128] = {};

/**********************************************************************************************************************
 * Static global variables
 **********************************************************************************************************************/
static i2c_master_instance_t const * gp_i2c_instance;
static bsp_io_port_pin_t             g_reset_pin;
#if (BSP_RTOS == 2)
static SemaphoreHandle_t * g_i2c_done_sem;
#else
#endif

touch_coord_t   new_touch;
GT911_payload_t touch_payload;
touch_data_t  * gp_touch_data;

uint8_t read_config_array[6][32];
uint8_t read_config[184];
uint8_t fresh_regs[2];

volatile touch_data_t g_touch_data = {0};

extern byte rdSensorReg16_Multi(uint16_t regID, uint8_t * regDat, uint32_t len);
void        reset_gt911(void);

bool enable_ts_mode = false;

/**********************************************************************************************************************
 * Function definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: touch_irq_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void touch_irq_callback (external_irq_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    /* Start payload reception */
    GT911_payload_get(&g_touch_data);
}

/**********************************************************************************************************************
 * End of function touch_irq_callback
 *********************************************************************************************************************/

static iic_master_instance_ctrl_t * p_api_ctrl = &g_i2c_master1_ctrl;

/*******************************************************************************************************************//**
 * Callback for touch I2C module
 * @param      p_args          I2C master callback data
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: touch_i2c_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void touch_i2c_callback (i2c_master_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    return;

    if (enable_ts_mode == false)
    {
        return;
    }

    R_BSP_IrqStatusClear(R_FSP_CurrentIrqGet());

#if (BSP_RTOS == 2)
    BaseType_t context_switch;

    /* Set Vsync semaphore */
    xSemaphoreGiveFromISR(*g_i2c_done_sem, &context_switch);

    portYIELD_FROM_ISR(context_switch);
#else
    if (p_args->event == I2C_MASTER_EVENT_TX_COMPLETE)
    {
        R_IIC_MASTER_Read(&p_api_ctrl, &touch_payload, sizeof(GT911_payload_t), false);
    }
    else if (p_args->event == I2C_MASTER_EVENT_RX_COMPLETE)
    {
        uint8_t i = 0;

        /* Process the raw data ffor the touch point(s) into useful data */
        for ( ; i < GT911_NUM_POINTS; i++)
        {
            new_touch.x     = (uint16_t) extract_x(touch_payload.data_raw[i]);
            new_touch.y     = (uint16_t) extract_y(touch_payload.data_raw[i]);
            new_touch.event = extract_e(touch_payload.data_raw[i]);

            /* Set event type based on received data */
            switch (new_touch.event)
            {
                case GT911_DOWN:
                {
                    gp_touch_data->point[i].event = TOUCH_EVENT_DOWN;
                    break;
                }

                case GT911_UP:
                {
                    gp_touch_data->point[i].event = TOUCH_EVENT_UP;
                    break;
                }

                case GT911_CONTACT:
                {
                    /* Check if the point is moving or not */
                    if ((gp_touch_data->point[i].x != new_touch.x) || (gp_touch_data->point[i].y != new_touch.y))
                    {
                        gp_touch_data->point[i].event = TOUCH_EVENT_MOVE;
                    }
                    else
                    {
                        gp_touch_data->point[i].event = TOUCH_EVENT_HOLD;
                    }

                    break;
                }

                default:
                {
                    gp_touch_data->point[i].event = TOUCH_EVENT_NONE;
                    break;
                }
            }

            /* Set new coordinates */
            gp_touch_data->point[i].x = new_touch.x;
            gp_touch_data->point[i].y = new_touch.y;
        }

        /* Pass the number of active touch points through */
        gp_touch_data->num_points = touch_payload.num_points_active;

        /* Reenable IRQn interrupt */
        R_ICU_ExternalIrqEnable(&g_external_irq3_ctrl);
    }
    else
    {
        /* In case of error, reenable IRQn interrupt */
        R_ICU_ExternalIrqEnable(&g_external_irq3_ctrl);
    }
#endif
}

/**********************************************************************************************************************
 * End of function touch_i2c_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: reset_gt911
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void reset_gt911 (void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, DISP_RST, BSP_IO_LEVEL_LOW);

    R_IOPORT_PinCfg(&g_ioport_ctrl,
                    DISP_INT,
                    (uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW);

    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MICROSECONDS);

#if   (GT_911_I2C_ADDRESS_0x14 == GT_911_I2C_ADDRESS)
    R_IOPORT_PinWrite(&g_ioport_ctrl, DISP_INT, BSP_IO_LEVEL_HIGH);
#elif (GT_911_I2C_ADDRESS_0x5D == GT_911_I2C_ADDRESS)
    R_IOPORT_PinWrite(&g_ioport_ctrl, DISP_INT, BSP_IO_LEVEL_LOW);
#endif

    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MICROSECONDS);

    R_IOPORT_PinWrite(&g_ioport_ctrl, DISP_RST, BSP_IO_LEVEL_HIGH);

    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    R_IOPORT_PinWrite(&g_ioport_ctrl, DISP_INT, BSP_IO_LEVEL_LOW);

    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);

    R_IOPORT_PinCfg(&g_ioport_ctrl, DISP_INT,
                    ((uint32_t) IOPORT_CFG_IRQ_ENABLE | (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT));
}

/**********************************************************************************************************************
 * End of function reset_gt911
 *********************************************************************************************************************/

#if (BSP_RTOS == 2)

/*******************************************************************************************************************//**
 * Basic function to wait for I2C comms completion
 **********************************************************************************************************************/
static void i2c_wait ()
{
    xSemaphoreTake(*g_i2c_done_sem, portMAX_DELAY);
}

#endif

/**********************************************************************************************************************
 * Function Name: clear_touch
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void clear_touch (void)
{
    memset(&g_touch_data, 0, sizeof(GT911_payload_t));
}

/**********************************************************************************************************************
 * End of function clear_touch
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: test_tp
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
void test_tp ()
{
    fsp_err_t status   = FSP_ERR_NOT_OPEN;
    uint16_t  regID    = 0x8150;
    byte      reg_val1 = 0;
    byte      reg_val2 = 0;
    byte      sa_check = 0x5d;

    R_BSP_PinAccessEnable();

    // initialise touch pannel
    R_IOPORT_PinWrite(&g_ioport_ctrl, DSP_INT_PIN, BSP_IO_LEVEL_LOW);
    R_IOPORT_PinCfg(&g_ioport_ctrl, DSP_INT_PIN, ((uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT));

    /* set i2c addresses to 0x28/0x29 */
    /* 1 set reset low */
    R_IOPORT_PinWrite(&g_ioport_ctrl, DSP_RST_PIN, BSP_IO_LEVEL_LOW);

    /* short delay then set INT high */
    R_BSP_SoftwareDelay(100u, BSP_DELAY_UNITS_MICROSECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, DSP_INT_PIN, BSP_IO_LEVEL_HIGH);

    /* 2 400us delay */
    R_BSP_SoftwareDelay(400u, BSP_DELAY_UNITS_MICROSECONDS);

    /* 3 set reset high */
    R_IOPORT_PinWrite(&g_ioport_ctrl, DSP_RST_PIN, BSP_IO_LEVEL_HIGH);

    /* 4 Hold INT high for 10ms (minimum is 5), then set low */
    R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, DSP_INT_PIN, BSP_IO_LEVEL_LOW);

    /* 5 Wait a further 50ms before setting to high impedance input */
    R_BSP_SoftwareDelay(10u, BSP_DELAY_UNITS_MILLISECONDS);

    /*pass control of the pin to the GT911 */
    R_IOPORT_PinCfg(&g_ioport_ctrl, DSP_INT_PIN,
                    ((uint32_t) (IOPORT_CFG_IRQ_ENABLE | (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT)));

    /* Check for touch panel */
    reg_val1 = 0xA5;

    /* 6 wait a further 500ms */
    R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

    i2c_master_status_t i2c_status;
    R_IIC_MASTER_StatusGet(&g_i2c_master1_ctrl, &i2c_status);

    if (i2c_status.open != true)       // I2c not open
    {
        R_IIC_MASTER_Open(&g_i2c_master1_ctrl, &g_i2c_master1_cfg);
    }

    R_IIC_MASTER_SlaveAddressSet(&g_i2c_master1_ctrl, 0x14, I2C_MASTER_ADDR_MODE_7BIT);
    rdSensorReg16_8(regID, &reg_val1);
    R_BSP_PinAccessDisable();
}

/**********************************************************************************************************************
 * End of function test_tp
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: productId
 * Description  : .
 * Argument     : target
 * Return Value : .
 *********************************************************************************************************************/
static bool_t productId (char * target)
{
    uint16_t regID = 0x8140;
    bool_t   ret   = true;

    rdSensorReg16_Multi(regID, (uint8_t *) target, 4);

    return ret;
}

/**********************************************************************************************************************
 * End of function productId
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: calcCheckSum
 * Description  : .
 * Arguments    : buf
 *              : len
 * Return Value : .
 *********************************************************************************************************************/
static uint8_t calcCheckSum (uint8_t * buf, uint8_t len)
{
    uint8_t ccsum = 0;

    for (uint8_t i = 0; i < len; i++)
    {
        ccsum += buf[i];
    }

    ccsum = (~ccsum) + 1;

    return ccsum;
}

/**********************************************************************************************************************
 * End of function calcCheckSum
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: init_ts
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t init_ts (void)
{
    bool_t ret = false;

    R_BSP_PinAccessEnable();

    /** Reset touch chip by setting GPIO reset pin low. */
    R_IOPORT_PinWrite(NULL, BSP_IO_PORT_10_PIN_01, BSP_IO_LEVEL_LOW);

    /** Wait 10 ms. */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    /** Release touch chip from reset */
    R_IOPORT_PinWrite(NULL, BSP_IO_PORT_10_PIN_01, BSP_IO_LEVEL_HIGH);

    /** Wait 10 ms. */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    /** Wait 100 ms. */
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

    R_BSP_PinAccessDisable();

    R_IIC_MASTER_SlaveAddressSet(&g_i2c_master1_ctrl, 0x14, I2C_MASTER_ADDR_MODE_7BIT);

    wrSensorReg16_8(GT911_REG_CONFIG_VERSION, 0x00U);

    productId(&product_id);

    if (product_id[0] == '9')
    {
        /* Product ID should be 9xx */
        ret = true;
    }

    memset(read_config, 0, sizeof(read_config));

    return ret;
}

/**********************************************************************************************************************
 * End of function init_ts
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: enable_ts
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void enable_ts (void)
{
    fsp_err_t err;

    /* Initialize touch panel driver and associated IRQn channel */
    uint8_t buf[2];

    buf[0] = calcCheckSum((uint8_t *) g911xFW, sizeof(g911xFW)); // Config_Chksum (0x80FF)
    buf[1] = 0x01;                                               // Config_Fresh (0x8100)

    reset_gt911();

    err = R_IIC_MASTER_SlaveAddressSet(&g_i2c_master1_ctrl, 0x14, I2C_MASTER_ADDR_MODE_7BIT);

    wrSensorReg16_8(GT911_REG_CONFIG_VERSION, 0x00U);

    productId(product_id);             // product id should be 911

    uint8_t x = 0U;

    x = 0U;
#if 0
    print_to_console((void *) "Data read from g911xFW\r\n");

    for (uint32_t row = 0; row < 8; row++)
    {
        for (uint32_t col = 0; col < 30; col++)
        {
            if (x > 183)
            {
                break;
            }

            sprintf(g_ts_buffer, "0x%02x ", g911xFW[x++]);
            print_to_console(g_ts_buffer);
        }

        print_to_console((void *) "\r\n");
    }
#endif

    /* Write the registers with a known good config */
    if (FSP_SUCCESS == err)            /* Checking that slave address set worked */
    {
        reset_gt911();

        err = R_IIC_MASTER_SlaveAddressSet(&g_i2c_master1_ctrl, GT_911_I2C_ADDRESS, I2C_MASTER_ADDR_MODE_7BIT);

        /* Configure i2c version */
        buf[0] = calcCheckSum((uint8_t *) g911xFW, sizeof(g911xFW)); // Config_Chksum (0x80FF)
        buf[1] = 0x01;                                               // Config_Fresh (0x8100)

        /* Write the configuration registers with g911xFW[]. */
        for (x = 0U; x < (sizeof(g911xFW)); x++)
        {
            err = wrSensorReg16_8(GT911_REG_CONFIG_VERSION + x, (int) g911xFW[x]);
            if (FSP_SUCCESS != err)
            {
                break;
            }
        }

        if (FSP_SUCCESS == err)
        {
            /* Write the checksum and "fresh config" registers with buf[]. */
            for (x = 0U; x < (sizeof(buf)); x++)
            {
                err = wrSensorReg16_8(GT911_REG_CONFIG_CHECKSUM + x, (int) buf[0 + x]);
                if (FSP_SUCCESS != err)
                {
                    break;
                }
            }
        }
    }

    x = 0U;
#if 0
    print_to_console((void *) "Data read from read_config\r\n");

    int regID = 0x8047;
    rdSensorReg16_Multi(regID, (uint8_t *) read_config, 187);

    for (uint32_t row = 0; row < 8; row++)
    {
        for (uint32_t col = 0; col < 30; col++)
        {
            if (x > 183)
            {
                break;
            }

            sprintf(g_ts_buffer, "0x%02x ", read_config[x++]);
            print_to_console(g_ts_buffer);
        }

        print_to_console((void *) "\r\n");
    }
#endif

    R_ICU_ExternalIrqOpen((void *) &g_external_irq3_ctrl, &g_external_irq3_cfg);
    R_ICU_ExternalIrqEnable((void *) &g_external_irq3_ctrl);

    enable_ts_mode = true;
}

/**********************************************************************************************************************
 * End of function enable_ts
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Reset the GT911
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: GT911_reset
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
static void GT911_reset ()
{
    R_BSP_PinAccessEnable();

    /** Reset touch chip by setting GPIO reset pin low. */
    R_BSP_PinWrite(g_reset_pin, BSP_IO_LEVEL_LOW);

    /** Wait 10 ms. */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    /** Release touch chip from reset */
    R_BSP_PinWrite(g_reset_pin, BSP_IO_LEVEL_HIGH);

    /** Wait 10 ms. */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    R_BSP_PinAccessDisable();
}

/**********************************************************************************************************************
 * End of function GT911_reset
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 * Initialize the connection with the GT911 touch controller
 *
 * @param      p_i2c_instance  I2C Master instance to use for communication
 * @param      i2c_semaphore   Semaphore indicating I2C completion
 * @param[in]  reset_pin       Pin connected to GT911 reset line
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: GT911_init
 * Description  : .
 * Arguments    : p_i2c_instance
 *              : i2c_semaphore
 *              : reset_pin
 * Return Value : .
 *********************************************************************************************************************/
void GT911_init (i2c_master_instance_t const * p_i2c_instance, void * i2c_semaphore, bsp_io_port_pin_t reset_pin)
{
    /* Initialize control variables */
    gp_i2c_instance = p_i2c_instance;
    g_reset_pin     = reset_pin;

#if (BSP_RTOS == 2)
    g_i2c_done_sem = i2c_semaphore;
#else
    FSP_PARAMETER_NOT_USED(i2c_semaphore);
#endif

    /* Reset GT911 controller */
    GT911_reset();

    /* Set slave address and callback */
    R_IIC_MASTER_SlaveAddressSet(p_i2c_instance->p_ctrl, 0x14, I2C_MASTER_ADDR_MODE_7BIT);
    R_IIC_MASTER_CallbackSet(p_i2c_instance->p_ctrl, touch_i2c_callback, NULL, NULL);

    enable_ts_mode = true;
}

/**********************************************************************************************************************
 * End of function GT911_init
 *********************************************************************************************************************/

#if (BSP_RTOS == 2)

/*******************************************************************************************************************//**
 * Get all touch data from the GT911 touch controller
 * @param      touch_data      Pointer to struct for output touch data
 **********************************************************************************************************************/
void GT911_payload_get (touch_data_t * touch_data)
{
    // TODO:: Add a non-RTOS version that calls each next step from the callback
    touch_coord_t   new_touch;
    GT911_payload_t touch_payload;

    /* Clear payload struct */
    memset(&touch_payload, 0, sizeof(GT911_payload_t));

    /* Read the data about the touch point(s) */
    uint8_t reg = GT911_REG_TD_STATUS;

    /* Write TD_STATUS address */
    R_IIC_MASTER_Write(gp_i2c_instance->p_ctrl, &reg, 1, true);
    i2c_wait();

    /* Read TD_STATUS through all five TOUCHn_** register sets */
    R_IIC_MASTER_Read(gp_i2c_instance->p_ctrl, (uint8_t *) &touch_payload, sizeof(GT911_payload_t), false);
    i2c_wait();

    /* Process the raw data for the touch point(s) into useful data */
    for (uint8_t i = 0; i < GT911_NUM_POINTS; i++)
    {
        new_touch.x     = (uint16_t) extract_x(touch_payload.data_raw[i]);
        new_touch.y     = (uint16_t) extract_y(touch_payload.data_raw[i]);
        new_touch.event = extract_e(touch_payload.data_raw[i]);

        /* Set event type based on received data */
        switch (new_touch.event)
        {
            case GT911_DOWN:
            {
                touch_data->point[i].event = TOUCH_EVENT_DOWN;
                break;
            }

            case GT911_UP:
            {
                touch_data->point[i].event = TOUCH_EVENT_UP;
                break;
            }

            case GT911_CONTACT:
            {
                /* Check if the point is moving or not */
                if ((touch_data->point[i].x != new_touch.x) || (touch_data->point[i].y != new_touch.y))
                {
                    touch_data->point[i].event = TOUCH_EVENT_MOVE;
                }
                else
                {
                    touch_data->point[i].event = TOUCH_EVENT_HOLD;
                }

                break;
            }

            default:
            {
                touch_data->point[i].event = TOUCH_EVENT_NONE;
                break;
            }
        }

        /* Set new coordinates */
        touch_data->point[i].x = new_touch.x;
        touch_data->point[i].y = new_touch.y;
    }

    /* Pass the number of active touch points through */
    touch_data->num_points = touch_payload.num_points_active;
}

#else

/*******************************************************************************************************************//**
 * Get all touch data from the GT911 touch controller
 * @param      touch_data      Pointer to struct for output touch data
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: GT911_payload_get
 * Description  : .
 * Argument     : touch_data
 * Return Value : .
 *********************************************************************************************************************/
void GT911_payload_get (touch_data_t * touch_data)
{
    /* Save pointer to touch data for use by I2C callback */
    gp_touch_data = touch_data;

    uint16_t buffer_regID = 0x814E;
    uint16_t coord_regID  = 0x8150;

    uint8_t reg_val1  = 0U;
    uint8_t xl        = 0U;
    uint8_t xh        = 0U;
    uint8_t yl        = 0U;
    uint8_t yh        = 0U;
    uint8_t tp_regval = 0U;

    uint16_t xcord = 0;
    uint16_t ycord = 0;

    /* Disable IRQn interrupt while payload reception is in progress */
    R_ICU_ExternalIrqDisable(&g_external_irq3_ctrl);

    rdSensorReg16_8(buffer_regID, &reg_val1);
    tp_regval               = reg_val1 & 0x0f;
    touch_data->num_points  = tp_regval;
    g_touch_data.num_points = tp_regval;

    R_BSP_PinAccessEnable();

    if (tp_regval > 0)
    {
// R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_07, BSP_IO_LEVEL_HIGH);
        R_IIC_MASTER_SlaveAddressSet(&g_i2c_master1_ctrl, 0x14, I2C_MASTER_ADDR_MODE_7BIT);

        for (uint8_t np = 0; np < tp_regval; np++)
        {
            rdSensorReg16_8(coord_regID + 0, &xl);
            rdSensorReg16_8(coord_regID + 1, &xh);
            xcord = (uint16_t) ((xh << 8) + xl);

            rdSensorReg16_8(coord_regID + 2, &yl);
            rdSensorReg16_8(coord_regID + 3, &yh);
            ycord = (uint16_t) ((yh << 8) + yl);

            g_touch_data.point[np].event = TOUCH_EVENT_DOWN;
            g_touch_data.point[np].x     = ycord;
            g_touch_data.point[np].y     = xcord;

            coord_regID += 8;
        }
    }
    else
    {
        /* Clear payload struct */
        memset(&g_touch_data, 0, sizeof(GT911_payload_t));

// R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_07, BSP_IO_LEVEL_LOW);
    }

    R_BSP_PinAccessDisable();

    wrSensorReg16_8(buffer_regID, 0x00U);

    /* Re-enable IRQn interrupt once payload reception completes */
    R_ICU_ExternalIrqEnable((void *) &g_external_irq3_ctrl);
}

/**********************************************************************************************************************
 * End of function GT911_payload_get
 *********************************************************************************************************************/

#endif
