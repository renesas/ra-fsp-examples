/***********************************************************************************************************************
 * File Name    : sci_i2c_ep.h
 * Description  : Contains macros and function declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SCI_I2C_EP_H_
#define SCI_I2C_EP_H_

/* For on-board LED */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA8T1_MCK)
#define LED_ON             ((bool)BSP_IO_LEVEL_LOW)
#define LED_OFF            ((bool)BSP_IO_LEVEL_HIGH)
#else
#define LED_ON             ((bool)BSP_IO_LEVEL_HIGH)
#define LED_OFF            ((bool)BSP_IO_LEVEL_LOW)
#endif

/* Macro to define the type of SCI API being used */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
#define SCI_TYPE                    "SCI_B"
#else
#define SCI_TYPE                    "SCI"
#endif

/* Macro to define the type of IIC SLAVE API being used */
#if (BSP_PERIPHERAL_IICA_PRESENT)
#define SLAVE_TYPE                  "A_SLAVE"
#elif defined (BOARD_RA6T2_MCK)
#define SLAVE_TYPE                  "_B_SLAVE"
#else
#define SLAVE_TYPE                  "_SLAVE"
#endif

/* Macro for checking if two buffers are equal */
#define BUFF_EQUAL          (0U)

/* Buffer size for slave and master data */
#define BUF_LEN             (0x06)

/* Led toggle delay */
#define TOGGLE_DELAY        (0x15E)

/* Delay added to recognize LED toggling after write/read operation */
#define DELAY_OPERATION     (1U)

#define EP_INFO   "\r\nThis example project demonstrates SCI_I2C Master operation through"\
                  "\r\nloopback with IIC Slave driver. 6 bytes of data will be transmitted"\
                  "\r\nand received continuously on successful initialization. The transmitted"\
                  "\r\ndata is compared with the received data. If the data matches, on-board"\
                  "\r\nLED starts blinking. On a data mismatch, LED stays ON. Failure messages"\
                  "\r\nand status is displayed on RTT Viewer.\r\n\n"

/* Enumerators to identify master event to be processed */
typedef enum e_master
{
    MASTER_READ  = 1U,
    MASTER_WRITE = 2U
}master_transfer_mode_t;

/* Global functions */
fsp_err_t init_i2c_driver(void);
fsp_err_t process_master_WriteRead(void);
void deinit_i2c_driver(void);
void set_led(bool b_value);

#endif /* SCI_I2C_EP_H_ */
