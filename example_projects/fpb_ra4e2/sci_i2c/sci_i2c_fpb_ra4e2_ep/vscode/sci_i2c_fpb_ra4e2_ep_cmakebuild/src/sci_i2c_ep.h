/***********************************************************************************************************************
 * File Name    : sci_i2c_ep.h
 * Description  : Contains macros, variables, data structures and functions used in sci_i2c_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SCI_I2C_EP_H_
#define SCI_I2C_EP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                             (0x00)

/* Macros for on-board LED */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA8T1_MCK) ||\
    defined (BOARD_RA8T2_MCK)
#define LED_ON                                  (BSP_IO_LEVEL_LOW)
#define LED_OFF                                 (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                                  (BSP_IO_LEVEL_HIGH)
#define LED_OFF                                 (BSP_IO_LEVEL_LOW)
#endif /* Specific board */

/* Macro for checking if two buffers are equal */
#define BUFF_EQUAL                              (0U)

/* Macro for buffer length for slave and master data */
#define BUFF_LEN                                (0x06)

/* Macro for LED toggle delay */
#define TOGGLE_DELAY                            (0x15E)

/* Macro for delay between write/read operation */
#define DELAY_OPERATION                         (1U)

/* Macros based on the SCI version */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
#define MODULE_NAME                             "r_sci_b_i2c"
#define MODULE_PADDING                          "  "
#define SCI_I2C_OPEN(ctrl, cfg)                 (R_SCI_B_I2C_Open((ctrl), (cfg)))
#define SCI_I2C_WRITE(ctrl, src, len, rs)       (R_SCI_B_I2C_Write((ctrl), (src), (len), (rs)))
#define SCI_I2C_READ(ctrl, dest, len, rs)       (R_SCI_B_I2C_Read((ctrl), (dest), (len), (rs)))
#define SCI_I2C_CLOSE(ctrl)                     (R_SCI_B_I2C_Close(ctrl))
#elif (BSP_PERIPHERAL_SCI_PRESENT)
#define MODULE_NAME                             "r_sci_i2c"
#define MODULE_PADDING                          "    "
#define SCI_I2C_OPEN(ctrl, cfg)                 (R_SCI_I2C_Open((ctrl), (cfg)))
#define SCI_I2C_WRITE(ctrl, src, len, rs)       (R_SCI_I2C_Write((ctrl), (src), (len), (rs)))
#define SCI_I2C_READ(ctrl, dest, len, rs)       (R_SCI_I2C_Read((ctrl), (dest), (len), (rs)))
#define SCI_I2C_CLOSE(ctrl)                     (R_SCI_I2C_Close(ctrl))
#endif /* SCI version */

/* Macros based on the IIC version */
#if (BSP_PERIPHERAL_IICA_PRESENT)
#define IIC_SLAVE_OPEN(ctrl, cfg)               (R_IICA_SLAVE_Open((ctrl), (cfg)))
#define IIC_SLAVE_READ(ctrl, dest, len)         (R_IICA_SLAVE_Read((ctrl), (dest), (len)))
#define IIC_SLAVE_WRITE(ctrl, src, len)         (R_IICA_SLAVE_Write((ctrl), (src), (len)))
#define IIC_SLAVE_CLOSE(ctrl)                   (R_IICA_SLAVE_Close(ctrl))
#elif defined (BOARD_RA6T2_MCK) || defined (BOARD_RA4E2_FPB)
#define IIC_SLAVE_OPEN(ctrl, cfg)               (R_IIC_B_SLAVE_Open((ctrl), (cfg)))
#define IIC_SLAVE_READ(ctrl, dest, len)         (R_IIC_B_SLAVE_Read((ctrl), (dest), (len)))
#define IIC_SLAVE_WRITE(ctrl, src, len)         (R_IIC_B_SLAVE_Write((ctrl), (src), (len)))
#define IIC_SLAVE_CLOSE(ctrl)                   (R_IIC_B_SLAVE_Close(ctrl))
#elif (BSP_PERIPHERAL_IIC_PRESENT)
#define IIC_SLAVE_OPEN(ctrl, cfg)               (R_IIC_SLAVE_Open((ctrl), (cfg)))
#define IIC_SLAVE_READ(ctrl, dest, len)         (R_IIC_SLAVE_Read((ctrl), (dest), (len)))
#define IIC_SLAVE_WRITE(ctrl, src, len)         (R_IIC_SLAVE_Write((ctrl), (src), (len)))
#define IIC_SLAVE_CLOSE(ctrl)                   (R_IIC_SLAVE_Close(ctrl))
#endif /* IIC version */

/* Extern variable - used for on-board LEDs */
extern bsp_leds_t g_bsp_leds;

/* Function declarations */
fsp_err_t init_i2c(void);
fsp_err_t sci_i2c_master_write(void);
fsp_err_t sci_i2c_master_read(void);
void deinit_sci_i2c(void);
void deinit_iic(void);
void toggle_led(void);
void set_led(bsp_io_level_t led_status);

#endif /* SCI_I2C_EP_H_ */
