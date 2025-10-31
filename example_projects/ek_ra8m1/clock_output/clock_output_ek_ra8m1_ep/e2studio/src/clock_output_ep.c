/***********************************************************************************************************************
 * File Name    : clock_output_ep.c
 * Description  : Contains data structures and functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "clock_output_ep.h"
#include "bsp_clocks.h"
#include "bsp_common.h"

extern bsp_leds_t g_bsp_leds;

/* The buffer contains user input */
char g_rx_buffer [TERM_BUFFER_SIZE] = {RESET_VALUE};

/* Private function declarations */
static void start_main_clock (void);
static void start_sub_clock (void);
static void stop_all_clocks (void);
static void clock_output_operation (void);

static uint8_t get_user_input (void);

/***********************************************************************************************************************
 *  Function Name: clock_output_ep_entry
 *  Description  : This function starts the clock output example operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void clock_output_ep_entry(void)
{
    fsp_pack_version_t version = { RESET_VALUE };

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    fsp_err_t err = FSP_SUCCESS;

    /* Turn OFF error LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_LOW);

    /* Initialize terminal */
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Turn ON error LED to indicate uart_init failed */
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_HIGH);

        /* Error trap */
        ERROR_TRAP;
    }

    /* Print the EP banner on the RTT viewer */
    APP_PRINT (BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
                version.version_id_b.patch);

    /* Print the EP information on the RTT viewer */
    APP_PRINT (EP_INFO);

    while (true)
    {
        /* Print the EP message to indicate user about application status */
        APP_PRINT ("\r\nEnter your test option from Terminal Application\r\n");

        /* Print EP menu for User selection */
        APP_PRINT (MENU);

        /* Perform clock output operation */
        clock_output_operation();
    }
}
/***********************************************************************************************************************
* End of function clock_output_ep_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: clock_output_operation
 * Description  : This function performs clock output operation on user input
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void clock_output_operation(void)
{
    uint8_t user_input = RESET_VALUE;

    /* Get user input */
    user_input = get_user_input ();

    /* Configure clock output pin as peripheral clock out pin */
    R_IOPORT_PinCfg (&g_ioport_ctrl, CLKOUT_PIN, IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CLKOUT_COMP_RTC);

    /* Perform operation on user input */
    switch (user_input)
    {
        case START_MAIN_CLOCK:
        {
            start_main_clock ();
            break;
        }

        case START_SUB_CLOCK:
        {
            start_sub_clock ();
            break;
        }

        case STOP_ALL_CLOCKS:
        {
            stop_all_clocks ();
            break;
        }

        default:
        {
            APP_PRINT("\r\n\r\nInvalid Input!!!");
            APP_PRINT("\r\nPlease provide input from below option\r\n");
            break;
        }
    }
}
/***********************************************************************************************************************
* End of function clock_output_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: get_user_input
 * Description  : This function is called to get user input from Terminal Application.
 * Arguments    : None
 * Return value : input_value   Return input value after receive enter key
 **********************************************************************************************************************/
static uint8_t get_user_input(void)
{
    uint8_t input_value = RESET_VALUE;

    /* Clean buffer */
    memset(g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));

    /* Wait until there is any user input */
    while (!APP_CHECK_DATA)
    {
        ;
    }

    /* Read First byte of data provided by user */
    APP_READ (g_rx_buffer,sizeof(g_rx_buffer));
    /* Convert to integer value */
    input_value = (uint8_t)atoi((char*) &g_rx_buffer);

    return input_value;
}
/***********************************************************************************************************************
* End of function get_user_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: start_main_clock
 * Description  : This function performs main clock operation.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void start_main_clock(void)
{
    APP_PRINT ("\r\n\r\n ** Start Main Clock Output ** \r\n");

    /* Unlock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_UNLOCK;

    /* Clock Out Disable */
    R_SYSTEM->CKOCR_b.CKOEN = 0x0;

    /* Clock Output Frequency Division Ratio */
    R_SYSTEM->CKOCR_b.CKODIV = MAIN_CLOCK_DIV;

    /* Select Main clock in Clock Out Control Register */
#if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
    R_SYSTEM->CKOCR = (R_SYSTEM->CKOCR & (uint8_t) ~R_SYSTEM_CKOCR_CKOSEL_Msk)
    | (BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC << R_SYSTEM_CKOCR_CKOSEL_Pos);
#else
    R_SYSTEM->CKOCR_b.CKOSEL = BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC;
#endif

    /* Clock Out Enable */
    R_SYSTEM->CKOCR_b.CKOEN = 0x1;

    /* Lock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_LOCK;

#if (GPT_MEASURE_CLKOUT == 1)
    fsp_err_t err = FSP_SUCCESS;

    /* Perform gpt capture operation */
    err = gpt_capture_operation ();

    /* Handle error */
    APP_ERR_HANDLE (err, "\r\n gpt_capture_operation failed!!! \r\n");

#endif /* GPT_MEASURE_CLKOUT */
}
/***********************************************************************************************************************
* End of function start_main_clock
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: start_sub_clock
 * Description  : This function performs Sub clock operation.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void start_sub_clock(void)
{
    APP_PRINT ("\r\n\r\n ** Start Sub-Clock Output ** \r\n");

    /* Unlock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_UNLOCK;

    /* Clock Out Disable */
    R_SYSTEM->CKOCR_b.CKOEN = 0x0;

    /* Clock Output Frequency Division Ratio */
    R_SYSTEM->CKOCR_b.CKODIV = 0;

    /* Select Sub-clock in Clock Out Control Register */
#if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
    R_SYSTEM->CKOCR = (R_SYSTEM->CKOCR & (uint8_t) ~R_SYSTEM_CKOCR_CKOSEL_Msk)
    | (BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK << R_SYSTEM_CKOCR_CKOSEL_Pos);
#else
    R_SYSTEM->CKOCR_b.CKOSEL = BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK;
#endif

    /* Clock Out Enable */
    R_SYSTEM->CKOCR_b.CKOEN = 0x1;

    /* Lock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_LOCK;

#if (GPT_MEASURE_CLKOUT == 1)
    fsp_err_t err = FSP_SUCCESS;

    /* Perform gpt capture operation */
    err = gpt_capture_operation ();

    /* Handle error */
    APP_ERR_HANDLE (err, "\r\n gpt_capture_operation failed!!! \r\n");

#endif /* GPT_MEASURE_CLKOUT */
}
/***********************************************************************************************************************
* End of function start_sub_clock
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: stop_all_clocks
 * Description  : This function stops all clocks.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void stop_all_clocks(void)
{
    APP_PRINT ("\r\n\r\n ** Stop all Clocks Output ** \r\n");

    /* Unlock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_UNLOCK;

    /* Clock Out Disable */
    R_SYSTEM->CKOCR_b.CKOEN = 0x0;

    /* Lock protect register */
    R_SYSTEM->PRCR = (uint16_t) PRV_PRCR_LOCK;
}
/***********************************************************************************************************************
* End of function stop_all_clocks
***********************************************************************************************************************/
