/***********************************************************************************************************************
 * File Name    : iic_master_addr_wu_ep.h
 * Description  : Contains data structures and functions used in iic_master_addr_wu_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef IIC_MASTER_ADDR_WU_EP_H_
#define IIC_MASTER_ADDR_WU_EP_H_

extern bsp_leds_t g_bsp_leds;

#define EP_VERSION      ("1.0")
#define MODULE_NAME     "I2C match address wakeup"

#define BANNER_INFO     "\r\n*********************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Mode   *"\
                        "\r\n*   Example Project Version %s                                     *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                *"\
						"\r\n*********************************************************************"\
						"\r\nRefer to readme.txt file for more details on Example Project and" \
						"\r\nFSP User's Manual for more information about "MODULE_NAME" mode\r\n"

#define EP_INFO         "\r\nThis example project demonstrates the I2C address match wakeup function on"\
                        "\r\nRenesas RA MCUs based on FSP. On the Master board, the user can select an"\
                        "\r\nI2C operation from the main menu, and then select the Slave address from"\
                        "\r\nthe Slave address menu. If the user selects the correct Slave address,"\
                        "\r\nthe I2C operation can be performed. Otherwise, it cannot be performed."\
						"\r\nIf there is no I2C operation, the Slave board will enter to Software Standby"\
						"\r\nmode with I2C wakeup enable after 5 seconds. Then, the user can select an I2C"\
						"\r\noperation and the Slave address on the Master board to send I2C packet to the"\
						"\r\nSlave board. If the Slave address matches, the Slave board will wake up and"\
						"\r\nthe I2C operation can continue. Additionally, the user can press S1 on the"\
						"\r\nSlave board to transition from Software Standby mode to Normal mode."\
						"\r\nThe information and error messages will be printed on the terminal application"\
						"\r\nduring the execution of the project.\r\n"

#define EP_MENU         "\r\nMain Menu:"\
                        "\r\nEnter '1' to read message from the Slave board."\
                        "\r\nEnter '2' to write message to the Slave board.\r\n"\
						"User Input:  "

#define ADDR_MENU       "\r\nSlave Address Menu:"\
                        "\r\nEnter '1' to select Slave address = 0x4A."\
                        "\r\nEnter '2' to select Slave address = 0x4B.\r\n"\
                        "User Input:  "

#define BOARD_TYPE      "\r\nMASTER BOARD\r\n"

/* Macros for LED */
#define STATE_LED               (0U)    /* LED1 on board */
#define ERR_LED                 (1U)    /* LED2 on board */
#define LED_ON                  (BSP_IO_LEVEL_HIGH)
#define LED_OFF                 (BSP_IO_LEVEL_LOW)
#define LED_TURN_ON(x)          (R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[(x)], LED_ON))
#define LED_TURN_OFF(x)         (R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[(x)], LED_OFF))

/* Macros for IIC Master operation */
#define MASTER_READ             (1U)
#define MASTER_WRITE            (2U)

/* Macros for IIC Slave address */
#define SLAVE_ADDR_1            (1U)
#define SLAVE_ADDR_2            (2U)
#define CORRECT_SLAVE_ADDR      (0x4A)
#define INCORRECT_SLAVE_ADDR    (0x4B)

/* Macro for the buffer size */
#define BUFF_SIZE               (40U)

/* Macro for the delay time */
#define DELAY_TEN_MS            (10U)

/* Macros for the timeout */
#define SAFETY_FACTOR           (2U)
#define IIC_BAUD_RATE           (100000U)
#define IIC_BIT_FRAME           (9U)
#define IIC_TIME_US             (BSP_DELAY_UNITS_SECONDS / (IIC_BAUD_RATE / IIC_BIT_FRAME))

/* Macro for the master message */
#define MASTER_MESSAGE          "This is a message from the Master board"

/* Macro for the length of master message */
#define MASTER_MESSAGE_SIZE     (strlen(MASTER_MESSAGE))

/* Macro for the expected message received from the Slave board */
#define EXPECTED_MESSAGE        "This is a message from the Slave board"

/* Macro for the length of expected message */
#define EXPECTED_MESSAGE_SIZE   (strlen(EXPECTED_MESSAGE))

/**********************************************************************************************************************
* Public function declarations
**********************************************************************************************************************/
void iic_master_wakeup_ep_entry(void);
void handle_error(fsp_err_t err, const char * err_str);

#endif /* IIC_MASTER_ADDR_WU_EP_H_ */
