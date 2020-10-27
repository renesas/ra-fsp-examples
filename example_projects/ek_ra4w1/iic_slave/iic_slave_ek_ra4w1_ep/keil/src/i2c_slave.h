/***********************************************************************************************************************
 * File Name    : i2c_slave.h
 * Description  : Contains data structures and functions used in i2c_slave.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef I2C_SLAVE_H_
#define I2C_SLAVE_H_


/* macro definition */
/* for on board LED */
#if defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4W1_EK)
#define LED_ON             BSP_IO_LEVEL_LOW
#define LED_OFF            BSP_IO_LEVEL_HIGH
#else
#define LED_ON             BSP_IO_LEVEL_HIGH
#define LED_OFF            BSP_IO_LEVEL_LOW
#endif

/* MACRO for checking if two buffers are equal */
#define BUFF_EQUAL         (0U)

/* buffer size for slave and master data */
#define BUF_LEN            0x06

/* Human eye noticeable LED toggle delay */
#define TOGGLE_DELAY       0x3E8

#define EP_INFO    "\r\nThis EP demonstrates IIC slave operation using two I2C channels." \
        "\r\nIt performs Slave read and write operation continuously once initialization  " \
        "\r\nis successful. On successful I2C transaction(6 bytes), Data transceived is  "\
        "\r\ncompared. Led blinks on data match else it is turned ON as sign of failure. " \
        "\r\nFor both cases corresponding slave operation message is displayed on RTT. "\
        "\r\nAny API/event failure message is also displayed.\n\n\n\n"

/*
 *  Global functions
 */
fsp_err_t init_i2C_driver(void);
fsp_err_t process_slave_WriteRead(void);
void deinit_i2c_driver(void);
void set_led(bsp_io_level_t led_state);


#endif /* I2C_SLAVE_H_ */
