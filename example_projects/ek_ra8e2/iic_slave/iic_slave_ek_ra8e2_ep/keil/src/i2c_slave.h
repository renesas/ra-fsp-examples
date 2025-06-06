/***********************************************************************************************************************
 * File Name    : i2c_slave.h
 * Description  : Contains data structures and functions used in i2c_slave.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef I2C_SLAVE_H_
#define I2C_SLAVE_H_

/* Macros definition */
/* For on board LED */
#if defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T2_MCK)
#define LED_ON             (BSP_IO_LEVEL_LOW)
#define LED_OFF            (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON             (BSP_IO_LEVEL_HIGH)
#define LED_OFF            (BSP_IO_LEVEL_LOW)
#endif
/* Macro for IIC version */
#if (BSP_PERIPHERAL_IIC_B_PRESENT)
#define IIC_TYPE            "IIC_B"
#else
#define IIC_TYPE            "IIC"
#endif
/* Macro for checking if two buffers are equal */
#define BUFF_EQUAL         (0U)

/* Buffer size for slave and master data */
#define BUF_LEN            (0x06)

/* Human eye noticeable LED toggle delay */
#define TOGGLE_DELAY       (0x3E8)

#define EP_INFO    "\r\nThis EP demonstrates IIC slave operation using two I2C channels." \
        "\r\nIt performs Slave read and write operation continuously once initialization  " \
        "\r\nis successful. On successful I2C transaction (6 bytes), data transceived is  "\
        "\r\ncompared. Led blinks on data match else it is turned ON as sign of failure. " \
        "\r\nFor both cases corresponding slave operation message is displayed on RTT Viewer. "\
        "\r\nAny API/event failure message is also displayed.\n\n\n\n"

/* Global functions */
fsp_err_t init_i2C_driver(void);
fsp_err_t process_slave_WriteRead(void);
void deinit_i2c_driver(void);
void set_led(bsp_io_level_t led_state);

#endif /* I2C_SLAVE_H_ */
