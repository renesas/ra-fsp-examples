/***********************************************************************************************************************
 * File Name    : clock_output_ep.h
 * Description  : Contains data structures and functions used in clock_output_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CLOCK_OUTPUT_EP_H_
#define CLOCK_OUTPUT_EP_H_

#define EP_INFO     "\r\nThis example project demonstrates the typical use of the Multiple Clocks Output."\
		            "\r\nThe user inputs key from Terminal Application to start Clocks Output Option to the"\
					"\r\nclock output pin."\
					"\r\nEnter number 3 in the Terminal Application to stop the clocks output from the clock"\
					"\r\noutput pin of the MCU."\
					"\r\nThe EP information or any error messages will be displayed on the Terminal Application.\r\n"

#define MENU        "\r\nEnter 1 to Start Main_Clock Output"\
		            "\r\nEnter 2 to Start Sub_Clock Output"\
					"\r\nEnter 3 to Stop Clock Output\r\n"\
					"User Input:  "

/* Key code for writing PRCR register. */
#define PRV_PRCR_KEY                        (0xA500U)
#define PRV_PRCR_UNLOCK                     ((PRV_PRCR_KEY) | 0x3U)
#define PRV_PRCR_LOCK                       ((PRV_PRCR_KEY) | 0x0U)

/* Macro for board does not have clock out select definition. */
#if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
#define R_SYSTEM_CKOCR_CKOSEL_Pos               (0UL)       /* CKOSEL (Bit 0) */
#define R_SYSTEM_CKOCR_CKOSEL_Msk               (0x7UL)     /* CKOSEL (Bitfield-Mask: 0x07) */
#endif

/* Macro for main clock output frequency division ratio. */
#if (GPT_MEASURE_CLKOUT == 1)
#define MAIN_CLOCK_DIV                          (BSP_CLOCKS_SYS_CLOCK_DIV_32)
#else
#if defined (BOARD_RA2E1_EK) || defined (BOARD_RA2L1_EK)
#define MAIN_CLOCK_DIV                          (BSP_CLOCKS_SYS_CLOCK_DIV_2)
#else
#define MAIN_CLOCK_DIV                          (BSP_CLOCKS_SYS_CLOCK_DIV_1)
#endif
#endif

/* Macro for menu option. */
#define START_MAIN_CLOCK                        (1U)
#define START_SUB_CLOCK                         (2U)
#define STOP_ALL_CLOCKS                         (3U)

/* Macro for clock output pin. */
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

/* Public function declarations */
/***********************************************************************************************************************
 *  Function Name: clock_output_ep_entry
 *  Description  : This function is used to perform the clock output example operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void clock_output_ep_entry (void);

#if (GPT_MEASURE_CLKOUT == 1)
/***********************************************************************************************************************
 *  Function Name: gpt_capture_operation
 *  Description  : This function is used to perform the gpt capture operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
fsp_err_t gpt_capture_operation (void);

/***********************************************************************************************************************
 * Function Name: handle_error
 * Description  : This function handles error if any error occurred, closes opened GPT module and traps the error.
 * Arguments    : err       error status
 *                err_str   error string
 * Return Value : None
 **********************************************************************************************************************/
void handle_error (fsp_err_t err, uint8_t * err_str);
#endif /* GPT_MEASURE_CLKOUT */

#endif /* CLOCK_OUTPUT_EP_H_ */
