/***********************************************************************************************************************
 * File Name    : poeg_ep.h
 * Description  : Contains macros and functions declaration.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef POEG_EP_H_
#define POEG_EP_H_

#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for commands to be received through RTT input */
#define POEG_TRIGGER_ON_GTETRG_INPUT_PIN_LEVEL          (1U)
#define POEG_OUTPUT_DISABLE_USING_SOFTWARE_API          (2U)
#define POEG_OUTPUT_DISABLE_ON_GTIOC_OUTPUT_LEVEL       (3U)

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
#define POEG_OUTPUT_DISABLE_ON_ACMPHS_EVENTS            (4U)
#define POEG_RESET                                      (5U)

/* Macros for ACMPHS */
#if (BSP_PERIPHERAL_DAC_PRESENT) || (BSP_PERIPHERAL_DAC_B_PRESENT)
#define DAC_MAX_VAL                                     (4095U)        /* Max count value for 12 bit DAC */
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
#define LOW_VAL                                         (1400U)        /* Lower value of fluctuating range */
#define HIGH_VAL                                        (1600U)        /* Higher value of fluctuating range */
#define V_REF                                           (3.3f)         /* Reference Voltage */
#define ADC_REF_VAL                                     "1.18V"
#else
#define DAC_REF_VAL                                     (2048U)        /* Reference value for 12 bit DAC */
#define ADC_REF_VAL                                     "1.65V"
#define LOW_VAL                                         (2000U)        /* Lower value of fluctuating range */
#define HIGH_VAL                                        (2100U)        /* Higher value of fluctuating range */
#endif
#endif /* BSP_PERIPHERAL_DAC_PRESENT, BSP_PERIPHERAL_DAC_B_PRESENT */

#define CHANNEL_ZERO                                    (0U)           /* Channel 0 of ACMPHS */

/* Macro for version of ACMPHS */
#if BSP_PERIPHERAL_ACMPHS_B_PRESENT
#define ACMPHS_TYPE                                     "ACMPHS_B"
#else
#define ACMPHS_TYPE                                     "ACMPHS"
#endif /* BSP_PERIPHERAL_ACMPHS_B_PRESENT */
#else
#define POEG_RESET                                      (4U)
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

#define POEG_CHANNEL_0                                  (1U)
#define POEG_CHANNEL_1                                  (2U)

/* Macro for GTETRG pin */
#if defined(BOARD_RA6M1_EK) || defined(BOARD_RA6M2_EK)
#define GTETRG_PIN                                      "P104 (J3:39)"
#elif defined(BOARD_RA6M3_EK)
#define GTETRG_PIN                                      "P104 (J1:3)"
#elif defined(BOARD_RA6T1_RSSK)
#define GTETRG_PIN                                      "P104 (CNC:16)"
#elif defined(BOARD_RA4T1_MCK)
#define GTETRG_PIN                                      "P104 (CN3:12)"
#elif defined(BOARD_RA6T2_MCK)
#define GTETRG_PIN                                      "PA12 (CN6:12)"
#elif defined(BOARD_RA6T3_MCK)
#define GTETRG_PIN                                      "P104 (J4:12)"
#elif defined(BOARD_RA8D1_EK)
#define GTETRG_PIN                                      "P401 (J51:7)"
#elif defined(BOARD_RA8M1_EK)
#define GTETRG_PIN                                      "P614 (J54:27)"
#elif defined(BOARD_RA8T1_MCK)
#define GTETRG_PIN                                      "P104 (CN4:8)"
#elif defined(BOARD_RA8E1_FPB)
#define GTETRG_PIN                                      "P104 (J1:45)"
#elif defined(BOARD_RA4L1_EK)
#define GTETRG_PIN                                      "P104 (J3:9)"
#elif defined(BOARD_RA8E2_EK)
#define GTETRG_PIN                                      "P614 (J37:9)"
#elif defined(BOARD_RA2L2_EK)
#define GTETRG_PIN                                      "P104 (J2:35)"
#elif defined(BOARD_RA2T1_FPB)
#define GTETRG_PIN                                      "P104 (J4:8)"
#elif defined(BOARD_RA8P1_EK) || defined(BOARD_RA8D2_EK)
#define GTETRG_PIN                                      "P614 (J14:20)"
#elif defined(BOARD_RA4C1_EK)
#define GTETRG_PIN                                      "P104 (J2:20)"
#elif defined(BOARD_RA8T2_MCK)
#define GTETRG_PIN                                      "PA07 (CN2:17)"
#elif defined(BOARD_RA8M2_EK)
#define GTETRG_PIN                                      "P113 (J1:15)"
#endif

#define SET_FLAG                                        (1)
#define CLEAR_FLAG                                      (0)

#define NULL_CHAR                                       ('\0')

#if (BSP_PERIPHERAL_DAC_B_PRESENT)
#define DAC_TYPE                                        "DAC_B"
#else
#define DAC_TYPE                                        "DAC"
#endif

#define EP_INFO   "\r\nThis project demonstrates the basic functionalities of the Port Output Enable for GPT (POEG)"\
                  "\r\ndriver on Renesas RA MCUs based on Renesas FSP. The user can disable the GPT output pins using"\
                  "\r\none of the following options:"\
                  "\r\n"EP_MENU""\
                  "\r\nInformation, guideline and error messages are displayed on the J-Link RTT Viewer."\
                  "\r\nLED1 is turned ON when the GPT output pin is disabled successfully. LED1 is turned OFF"\
                  "\r\nwhen POEG resets successfully. Additionally, the user can verify the EP operation using a"\
                  "\r\nlogic analyzer.\r\n"

#define MENU_TITLE              "\r\nMain Menu\r\n"

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) || (BSP_PERIPHERAL_ACMPHS_B_PRESENT)
#define EP_MENU                 "1: Disable GPT output pins based on the GTETRG input pin level\r\n"\
		                        "2: Disable GPT output pins by software API\r\n"\
                                "3: Disable GPT output pins when GTIOC pins are at the same logic level\r\n"\
                                "4: Disable GPT output pins based on comparator (ACMPHS) crossing events\r\n"\
                                "5: Reset the POEG status\r\n"

#define RESET_MENU              "\r\nReset Menu\r\n"\
                                "1: Reset POEG channel 0 (Required after running option 3)\r\n"\
                                "2: Reset POEG channel 1 (Required after running options 1, 2, or 4)\r\n"\
                                "User Input:  "

#define OPTION_1_INFO           "\r\n** Remove connection of GTETRG pin and reset of application are needed prior \r\n"\
		                        "to repeatedly selecting menu option '1' or selecting other menu options "\
		                        "(except option '5') **\r\n"

#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
#define OPTION_4_INFO           "** Reset of the application is needed prior to repeatedly selecting menu option '4'"\
                                "\nor selecting other menu options (except option '5') to restart DAC0 and ACMPHS. **"\
                                "\r\nUser Input:  "
#elif defined (BOARD_RA2T1_FPB)
#define OPTION_4_INFO           "** Reset of the application is needed prior to repeatedly selecting menu option '4'"\
                                "\nor selecting other menu options (except option '5') to restart ACMPHS. **\r\n"\
                                "\r\nUser Input:  "

#else
#define OPTION_4_INFO           "** Reset of the application is needed prior to repeatedly selecting menu option '4'"\
                                "\nor selecting other menu options (except option '5') to restart DAC0, DAC1 and "\
                                "ACMPHS. **"\
                                "\r\nUser Input:  "
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK , BOARD_RA2T1_FPB */
#else
#define EP_MENU                 "1: Disable GPT output pins based on the GTETRG input pin level\r\n"\
                                "2: Disable GPT output pins by software API\r\n"\
                                "3: Disable GPT output pins when GTIOC pins are at the same logic level\r\n"\
                                "4: Reset the POEG status\r\n"

#define OPTION_1_INFO           "** Remove connection of GTETRG pin and reset of application are needed prior \r\n"\
                                "to repeatedly selecting menu option '1' or selecting other menu options "\
                                "(except option '4') **\r\n"\
                                "User Input:  "

#define RESET_MENU              "\r\nReset Menu\r\n"\
                                "1: Reset POEG channel 0 (Required after running option 3)\r\n"\
                                "2: Reset POEG channel 1 (Required after running options 1 or 2)\r\n"\
                                "User Input:  "
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

/* Macros for LED */
#define   STATE_LED                                     (0U)    /* LED1 on board */
#if defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T1_RSSK) || defined(BOARD_RA6T2_MCK)\
 || defined(BOARD_RA6T3_MCK) || defined(BOARD_RA8T1_MCK) || defined(BOARD_RA8T2_MCK)
#define   LED_ON                                        (BSP_IO_LEVEL_LOW)
#define   LED_OFF                                       (BSP_IO_LEVEL_HIGH)
#else
#define   LED_ON                                        (BSP_IO_LEVEL_HIGH)
#define   LED_OFF                                       (BSP_IO_LEVEL_LOW)
#endif

void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl);
fsp_err_t init_poeg0_module(void);
fsp_err_t init_poeg1_module(void);
fsp_err_t init_gpt_timer_pwm1(void);
fsp_err_t init_gpt_timer_pwm2(void);
fsp_err_t deinit_poeg0_module(void);
fsp_err_t deinit_poeg1_module(void);
void deinit_poeg_module(void);

#if (BSP_PERIPHERAL_ACMPHS_PRESENT) ||(BSP_PERIPHERAL_ACMPHS_B_PRESENT)
fsp_err_t enable_acmphs_mode_modules(void);
void deinit_dac_and_acmphs(void);
void deinit_acmphs(void);
#if (BSP_PERIPHERAL_DAC_B_PRESENT)
void deinit_dac(dac_b_instance_ctrl_t * p_ctrl);
#else
void deinit_dac(dac_instance_ctrl_t * p_ctrl);
#endif /* BSP_PERIPHERAL_DAC_B_PRESENT */
extern comparator_info_t g_stabilize_time;
#endif /* BSP_PERIPHERAL_ACMPHS_PRESENT, BSP_PERIPHERAL_ACMPHS_B_PRESENT */

#if defined (BOARD_RA6T2_MCK)
void deinit_adc(adc_ctrl_t * p_ctrl);
#endif /* BOARD_RA6T2_MCK */

fsp_err_t poeg_output_disable_and_reset_options(void);
fsp_err_t reset_poeg_module(void);

#endif /* POEG_EP_H_ */
