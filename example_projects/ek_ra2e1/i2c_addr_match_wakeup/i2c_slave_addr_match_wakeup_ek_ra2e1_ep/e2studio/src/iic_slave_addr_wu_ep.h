/***********************************************************************************************************************
 * File Name    : iic_slave_addr_wu_ep.h
 * Description  : Contains data structures and functions used in iic_slave_addr_wu_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef IIC_SLAVE_ADDR_WU_EP_H_
#define IIC_SLAVE_ADDR_WU_EP_H_

#include "r_iic_slave.h"

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

#define BOARD_TYPE      "\r\nSLAVE BOARD\r\n"

#define NORMAL_STATUS   "\r\nBoard state: ***NORMAL MODE***"\
                        "\r\nLED1 is turned ON."\
						"\r\nMCU enters Low Power Mode (LPM) after 5 seconds of I2C bus idle state.\r\n"

#define LPM_STATUS      "\r\nBoard state: ***SOFTWARE STANDBY MODE***"\
                        "\r\nLED1 is turned OFF.\r\n"

#define LPM_CANCEL_SOURCE_IIC               "\r\n***I2C Slave Address matched. LPM is canceled by IIC module***\r\n"
#define LPM_CANCEL_SOURCE_IRQ               "\r\n***LPM is canceled by ICU External IRQ module***\r\n"
#define LPM_CANCEL_SOURCE_NOT_DETECTED      "\r\n***LPM is canceled, but source has not been detected!!!***\r\n"

/* The user can choose wake up mode as below:
 * 0: Normal wakeup mode 1
 * 1: Normal wakeup mode 2
 * 2: Command recovery mode
 * 3: EEP response mode */
#define IIC_WAKEUP_MODE (0u)

#if (IIC_WAKEUP_MODE == 0)
#define IIC_ICWUR_WUACK_VALUE       (0U)
#define IIC_IICR1_IICRST_VALUE      (0U)
#elif (IIC_WAKEUP_MODE == 1)
#define IIC_ICWUR_WUACK_VALUE       (1U)
#define IIC_IICR1_IICRST_VALUE      (0U)
#elif (IIC_WAKEUP_MODE == 2) || (IIC_WAKEUP_MODE == 3)
#error "Not supported"
#endif

/* Macros for board does not have clock out select definition */
#if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
#define R_SYSTEM_CKOCR_CKOSEL_Pos   (0UL)   /* CKOSEL (Bit 0) */
#define R_SYSTEM_CKOCR_CKOSEL_Msk   (0x7UL) /* CKOSEL (Bitfield-Mask: 0x07) */
#endif

/* Macros for key code for writing PRCR register */
#define BSP_PRV_PRCR_KEY        (0xA500U)
#define BSP_PRV_PRCR_UNLOCK     ((BSP_PRV_PRCR_KEY) | 0x3U)
#define BSP_PRV_PRCR_LOCK       ((BSP_PRV_PRCR_KEY) | 0x0U)

/* Macro for the slave message */
#define SLAVE_MESSAGE           "This is a message from the Slave board"

/* Macro for the length of slave message */
#define SLAVE_MESSAGE_SIZE      (strlen(SLAVE_MESSAGE))

/* Macro for the expected message received from the Master board */
#define EXPECTED_MESSAGE        "This is a message from the Master board"

/* Macro for the length of expected message */
#define EXPECTED_MESSAGE_SIZE   (strlen(EXPECTED_MESSAGE))

/* Macro for the buffer size */
#define BUFF_SIZE               (40U)

/* Macros for LED */
#define STATE_LED               (0U)    /* LED1 on board */
#define ERR_LED                 (1U)    /* LED2 on board */
#define LED_ON                  (BSP_IO_LEVEL_HIGH)
#define LED_OFF                 (BSP_IO_LEVEL_LOW)
#define LED_TURN_ON(x)          (R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[(x)], LED_ON))
#define LED_TURN_OFF(x)         (R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[(x)], LED_OFF))

/* Macro for the WUI priority level */
#define WUI_PRIORITY            (2U)

/* Macro for the delay time */
#define DELAY_ONE_HUNDRED_MS    (100U)

/* Macros for clock output pin */
#if defined (BOARD_RA2A1_EK)
#define CLKOUT_PIN      (BSP_IO_PORT_01_PIN_10)
#elif defined (BOARD_RA2A2_EK)
#define CLKOUT_PIN      (BSP_IO_PORT_02_PIN_10)
#elif defined (BOARD_RA2E1_EK) || defined (BOARD_RA2L1_EK) || defined (BOARD_RA4E2_EK) || defined (BOARD_RA6E2_EK)
#define CLKOUT_PIN      (BSP_IO_PORT_01_PIN_09)
#elif defined (BOARD_RA8D1_EK)
#define CLKOUT_PIN      (BSP_IO_PORT_09_PIN_13)
#else
#define CLKOUT_PIN      (BSP_IO_PORT_02_PIN_05)
#endif
#define CLK_DIV                 (BSP_CLOCKS_SYS_CLOCK_DIV_16)
#define CLK_ENABLE              (0x1U)
#define CLK_DISABLE             (0x0U)

/* The timeout interrupt enable bit */
#define IIC_TMO_EN_BIT          (0x01U)

/* The arbitration loss detection interrupt enable bit */
#define IIC_ALD_EN_BIT          (0x02U)

/* The start condition detection interrupt enable bit */
#define IIC_STR_EN_BIT          (0x04U)

/* The stop condition detection interrupt enable bit */
#define IIC_STP_EN_BIT          (0x08U)

/* The NAK reception interrupt enable bit */
#define IIC_NAK_EN_BIT          (0x10U)

/* The receive data full interrupt enable bit */
#define IIC_RXI_EN_BIT          (0x20U)

/* The transmit end interrupt enable bit */
#define IIC_TEI_EN_BIT          (0x40U)

/* The transmit data empty interrupt enable bit */
#define IIC_TXI_EN_BIT          (0x80U)

/* Bit position for Timeout function (TMOF) detection flag in ICSR2 */
#define ICSR2_TMOF_BIT          (0x01U)

/* Bit position for Arbitration loss (AL) detection flag in ICSR2 */
#define ICSR2_AL_BIT            (0x02U)

/* Bit position for the START condition detection flag in ICSR2 */
#define ICSR2_START_BIT         (0x04U)

/* Bit position for STOP condition flag in ICSR2 */
#define ICSR2_STOP_BIT          (0x08U)

/* Bit position for No Acknowledgment (NACKF) flag in ICSR2 */
#define ICSR2_NACKF_BIT         (0x10U)

/* Buffer size for slave and master data */
#define BUF_LEN                 (0x04)

/**********************************************************************************************************************
* Public function declarations
**********************************************************************************************************************/
void iic_wakeup_callback(void);
void iic_slave_wakeup_ep_entry(void);
void iic_wakeup_instance_set(iic_slave_instance_ctrl_t* p_ctrl);
void iic_wakeup_cpu_interrupt_enable(uint32_t priority, void * p_context, void (* p_callback)(void));
void iic_wakeup_cpu_interrupt_disable(void);
void iic_wakeup_mode_operation_pre(void);
void iic_wakeup_mode_operation_post(void);
void handle_error(fsp_err_t err, const char * err_str);

#endif /* IIC_SLAVE_ADDR_WU_EP_H_ */
