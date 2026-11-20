/***********************************************************************************************************************
 * File Name    : iic_slave_ep.h
 * Description  : Contains macros, data structures, and functions used in iic_slave_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef IIC_SLAVE_EP_H_
#define IIC_SLAVE_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                                 (0x00)

/* Macro for checking if two buffers are equal */
#define BUFF_EQUAL                                  (0U)

/* Macro for buffer size */
#define BUF_LEN                                     (6U)

/* Macro for LED toggle delay */
#define TOGGLE_DELAY                                (1000U)

/* Macros for on-board LED */
#if defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T2_MCK) ||\
    defined (BOARD_RA8T2_MCK)
#define LED_ON                                      (BSP_IO_LEVEL_LOW)
#define LED_OFF                                     (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                                      (BSP_IO_LEVEL_HIGH)
#define LED_OFF                                     (BSP_IO_LEVEL_LOW)
#endif /* Specific board */

/* Macros for IIC Slave based on specific version */
#if (BSP_PERIPHERAL_IICA_PRESENT)
#define MODULE_NAME                                 "r_iica_slave"
#define MODULE_PADDING                              "  "
#define IIC_MASTER_OPEN(ctrl, cfg)                  (R_IICA_MASTER_Open((ctrl), (cfg)))
#define IIC_MASTER_WRITE(ctrl, src, len, restart)   (R_IICA_MASTER_Write((ctrl), (src), (len), (restart)))
#define IIC_MASTER_READ(ctrl, dest, len, restart)   (R_IICA_MASTER_Read((ctrl), (dest), (len), (restart)))
#define IIC_MASTER_CLOSE(ctrl)                      (R_IICA_MASTER_Close(ctrl))
#define IIC_SLAVE_OPEN(ctrl, cfg)                   (R_IICA_SLAVE_Open((ctrl), (cfg)))
#define IIC_SLAVE_WRITE(ctrl, src, len)             (R_IICA_SLAVE_Write((ctrl), (src), (len)))
#define IIC_SLAVE_READ(ctrl, dest, len)             (R_IICA_SLAVE_Read((ctrl), (dest), (len)))
#define IIC_SLAVE_CLOSE(ctrl)                       (R_IICA_SLAVE_Close(ctrl))

#elif defined (BOARD_RA6T2_MCK)
#define MODULE_NAME                                 "r_iic_b_slave"
#define MODULE_PADDING                              " "
#define IIC_MASTER_OPEN(ctrl, cfg)                  (R_IIC_B_MASTER_Open((ctrl), (cfg)))
#define IIC_MASTER_WRITE(ctrl, src, len, restart)   (R_IIC_B_MASTER_Write((ctrl), (src), (len), (restart)))
#define IIC_MASTER_READ(ctrl, dest, len, restart)   (R_IIC_B_MASTER_Read((ctrl), (dest), (len), (restart)))
#define IIC_MASTER_CLOSE(ctrl)                      (R_IIC_B_MASTER_Close(ctrl))
#define IIC_SLAVE_OPEN(ctrl, cfg)                   (R_IIC_B_SLAVE_Open((ctrl), (cfg)))
#define IIC_SLAVE_WRITE(ctrl, src, len)             (R_IIC_B_SLAVE_Write((ctrl), (src), (len)))
#define IIC_SLAVE_READ(ctrl, dest, len)             (R_IIC_B_SLAVE_Read((ctrl), (dest), (len)))
#define IIC_SLAVE_CLOSE(ctrl)                       (R_IIC_B_SLAVE_Close(ctrl))

#else
#define MODULE_NAME                                 "r_iic_slave"
#define MODULE_PADDING                              "   "
#define IIC_MASTER_OPEN(ctrl, cfg)                  (R_IIC_MASTER_Open((ctrl), (cfg)))
#define IIC_MASTER_WRITE(ctrl, src, len, restart)   (R_IIC_MASTER_Write((ctrl), (src), (len), (restart)))
#define IIC_MASTER_READ(ctrl, dest, len, restart)   (R_IIC_MASTER_Read((ctrl), (dest), (len), (restart)))
#define IIC_MASTER_CLOSE(ctrl)                      (R_IIC_MASTER_Close(ctrl))
#define IIC_SLAVE_OPEN(ctrl, cfg)                   (R_IIC_SLAVE_Open((ctrl), (cfg)))
#define IIC_SLAVE_WRITE(ctrl, src, len)             (R_IIC_SLAVE_Write((ctrl), (src), (len)))
#define IIC_SLAVE_READ(ctrl, dest, len)             (R_IIC_SLAVE_Read((ctrl), (dest), (len)))
#define IIC_SLAVE_CLOSE(ctrl)                       (R_IIC_SLAVE_Close(ctrl))
#endif /* IIC version */

typedef enum slave_transfer_mode
{
    SLAVE_READ  = 1U,
    SLAVE_WRITE = 2U,
    SLAVE_NO_RW = 3U
} slave_transfer_mode_t;

/* External variable - Used for LED */
extern bsp_leds_t g_bsp_leds;

/* Function declarations */
fsp_err_t init_i2c_driver(void);
fsp_err_t i2c_slave_write(void);
fsp_err_t i2c_slave_read(void);
void deinit_i2c_driver(void);
void blink_led(void);
void set_led(bsp_io_level_t led_state);

#endif /* IIC_SLAVE_EP_H_ */
