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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef SCI_I2C_EP_H_
#define SCI_I2C_EP_H_

/* for on board LED */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK)
#define LED_ON             ((bool)BSP_IO_LEVEL_LOW)
#define LED_OFF            ((bool)BSP_IO_LEVEL_HIGH)
#else
#define LED_ON             ((bool)BSP_IO_LEVEL_HIGH)
#define LED_OFF            ((bool)BSP_IO_LEVEL_LOW)
#endif

/* MACRO for checking if two buffers are equal */
#define BUFF_EQUAL (0U)

/* buffer size for slave and master data */
#define BUF_LEN            (0x06)

/* Led toggle delay */
#define TOGGLE_DELAY       (0x15E)

/*Delay added to recognise LED toggling after wrie/read operation */
#define DELAY_OPERATION  (1U)

#define EP_INFO  "\r\n  This Example Project demonstrates SCI_I2C Master operation through"\
                  "\r\n loop-back with IIC Slave driver. 6 bytes of data will be transmitted"\
                  "\r\n and received continuously on successful initialization."\
                  "\r\n The transmitted data is compared with the received data. If the data matches, on-board LED"\
                  "\r\n starts blinking. On a data mismatch, LED stays ON."\
                  "\r\n Failure messages and status is displayed on RTTViewer.\r\n\n"


/* enumerators to identify Master event to be processed */
typedef enum e_master
{
    MASTER_READ  = 1U,
    MASTER_WRITE = 2U
}master_transfer_mode_t;



/*
 *  Global functions
 */
fsp_err_t init_i2c_driver(void);
fsp_err_t process_master_WriteRead(void);
void deinit_i2c_driver(void);
void set_led(bool b_value);


#endif /* SCI_I2C_EP_H_ */
