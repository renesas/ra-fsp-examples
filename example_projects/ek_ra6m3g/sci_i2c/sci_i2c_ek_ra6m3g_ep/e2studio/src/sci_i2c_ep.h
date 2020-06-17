/***********************************************************************************************************************
 * File Name    : sci_i2c_ep.h
 * Description  : Contains macros and function declarations.
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef SCI_I2C_EP_H_
#define SCI_I2C_EP_H_


/* External IRQ channel */
/* board specific */
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define IRQ_CHANNEL        0x0D
#elif defined (BOARD_RA2A1_EK)
#define IRQ_CHANNEL        0x06
#elif defined (BOARD_RA6M1_EK)
#define IRQ_CHANNEL        0x08
#elif defined (BOARD_RA6M2_EK) || defined (BOARD_RA4M1_EK)
#define IRQ_CHANNEL        0x00
#endif

/* for on board LED */
#define LED_ON             (bool)BSP_IO_LEVEL_HIGH
#define LED_OFF            (bool)BSP_IO_LEVEL_LOW

/* MACRO for checking if two buffers are equal */
#define BUFF_EQUAL (0U)

/* buffer size for slave and master data */
#define BUF_LEN            0x06

/* Led toggle delay */
#define TOGGLE_DELAY       0x15E

#define EP_INFO  "\r\nThis Example Project demonstrates SCI_I2C Master operation through\r\n"\
                  "loop-back with IIC Slave driver. On pressing user push button, 6 bytes\r\n"\
                  "of data will be transmitted and received. The transmitted data is\r\n"\
                  "compared with the received data. If the data matches, on-board LED\r\n"\
                  "starts blinking. On a data mismatch, LED stays ON. Failure messages\r\n"\
                  "and status is displayed on RTTViewer.\r\n\n\n"
/*
 *  Global functions
 */
fsp_err_t init_ext_irq(void);
fsp_err_t init_i2c_driver(void);
fsp_err_t process_master_WriteRead(void);
void deinit_i2c_driver(void);
void set_led(bool b_value);
void deinit_external_irq(void);


#endif /* SCI_I2C_EP_H_ */
