/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdio.h>

#include "r_typedefs.h"
#include "r_console_if.h"

#include "board_cfg.h"
#include "common_init.h"
#include "common_utils.h"

#include "board_cfg.h"

/*setting NUM_RATES as the size of the array s_pwm_rates*/
#define NUM_RATES             (sizeof(s_pwm_rates) / sizeof(s_pwm_rates[0]))
/*setting NUM_DCS as the size of the array s_pwm_dcs*/
#define NUM_DCS               (sizeof(s_pwm_dcs)   / sizeof(s_pwm_dcs[0]))
/*setting NUM_SWITCH as the size of the array s_pwm_pins*/
#define NUM_SWITCH            (sizeof(s_irq_pins)  / sizeof(s_irq_pins[0]))

typedef struct irq_pins
{
    const external_irq_instance_t * const p_irq;
} st_irq_pins_t;


/* Terminal window escape sequences */
const char_t * const gp_clear_screen   = "\x1b[2J";
const char_t * const gp_cursor_home    = "\x1b[H";

const char_t * const gp_cursor_store     = "\x1b[s";
const char_t * const gp_cursor_restore   = "\x1b[u";
const char_t * const gp_cursor_temp      = "\x1b[7;41H\x1b[K";
const char_t * const gp_cursor_frequency = "\x1b[8;41H\x1b[K";
const char_t * const gp_cursor_intensity = "\x1b[9;41H\x1b[K";

const char_t * const gp_green_fg   = "\x1B[2;32m";
const char_t * const gp_white_fg   = "\x1B[2;37m";


extern bsp_leds_t g_bsp_leds;

st_board_status_t g_board_status;
uint8_t g_pwm_dcs_data[]   = {10, 50, 90};
uint8_t g_pwm_rates_data[] = {1, 5, 10};

/* 111 IRQ4_A & 205 IRQ1*/
static st_irq_pins_t s_irq_pins[] =
{
        {&g_external_irq0},
};

static uint32_t s_pwm_dcs[3] = {LED_INTENSITY_10, LED_INTENSITY_50, LED_INTENSITY_90};
static uint32_t s_pwm_rates[3] = {BLINK_FREQ_1HZ, BLINK_FREQ_5HZ, BLINK_FREQ_10HZ};

static uint32_t s_blueled_flashing = OFF;
static uint32_t s_intense = 0;
static uint32_t s_duty = 1;

/* Define the units to be used with the software delay function */
static const bsp_delay_units_t s_bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;

void button_nmi_callback(bsp_grp_irq_t irq);


/**********************************************************************************************************************
 * Function Name: adc_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t adc_initialize(void)
{
    adc_status_t status;

    fsp_err_t fsp_err = FSP_SUCCESS;
    fsp_err           = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }


#if BSP_FEATURE_ADC_CALIBRATION_REG_AVAILABLE
    fsp_err = R_ADC_Calibrate (&g_adc_ctrl, NULL);
    if (FSP_SUCCESS != fsp_err)
        return fsp_err;
#endif


    while ((R_ADC_StatusGet(&g_adc_ctrl, &status) == FSP_SUCCESS) && (ADC_STATE_IDLE != status.state))
    {
        R_BSP_SoftwareDelay(20, s_bsp_delay_units);
    }

    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanStart(&g_adc_ctrl);

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    return fsp_err;
}
/**********************************************************************************************************************
 End of function adc_initialize
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: icu_initialize
 * Description  : .
 * Return Value : Error/Success result
 *********************************************************************************************************************/
static fsp_err_t icu_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < NUM_SWITCH; i++ )
    {
        fsp_err = R_ICU_ExternalIrqOpen(s_irq_pins[i].p_irq->p_ctrl, s_irq_pins[i].p_irq->p_cfg);

        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }

        fsp_err = R_ICU_ExternalIrqEnable(s_irq_pins[i].p_irq->p_ctrl);

        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    return fsp_err;
}
/**********************************************************************************************************************
 End of function icu_initialize
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: gpt_initialize
 * Description  : .
 * Return Value : Error/Success result
 *********************************************************************************************************************/
static fsp_err_t gpt_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_AGT_Open (g_periodic_timer.p_ctrl, g_periodic_timer.p_cfg);
    if (fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }

    fsp_err = R_AGT_Open (g_agt_blue.p_ctrl, g_agt_blue.p_cfg);
    if (fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }
    return fsp_err;
}
/**********************************************************************************************************************
 End of function gpt_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: nmi_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t nmi_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_BSP_GroupIrqWrite(BSP_GRP_IRQ_NMI_PIN, button_nmi_callback);
    if (FSP_SUCCESS != fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }
    R_ICU->NMICR = 0x00;        //Write 0 to NFLTEN bit
    R_ICU->NMICR = 0xB1;        //Rising edge, PCLKB/64, Digital Filtering enabled
    R_ICU->NMICLR_b.NMICLR = 1; //Clear NMISR.NMIST flag
    R_ICU->NMIER_b.NMIEN = 1;   //Enable the NMI Pin interrupt

    return fsp_err;
}
/**********************************************************************************************************************
 End of function nmi_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: common_init
 * Description  : .
 * Return Value : Error/Success result
 *********************************************************************************************************************/
fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    if (STATUS_GET(STATUS_UPDATE_TEMP_INFO))
    {
        STATUS_SET(STATUS_UPDATE_INTENSE_INFO);
    }


    fsp_err = adc_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }

    fsp_err = icu_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }

    fsp_err = gpt_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }

    fsp_err = nmi_initialize();
    if (FSP_SUCCESS != fsp_err)
    {
        SYSTEM_ERROR
        return fsp_err;
    }

    /* Set baseline die temperature */
    fsp_err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &g_board_status.adc_temperature_data);

    /* Handle error */
    if (FSP_SUCCESS != fsp_err)
    {
        print_to_console ("** R_ADC_Read API failed ** \r\n");

        SYSTEM_ERROR
    }


    /* Set baseline LED status */
    g_board_status.led_intensity = 0;
    g_board_status.led_frequency = 0;
    R_AGT_PeriodSet(g_periodic_timer.p_ctrl, s_pwm_rates[g_board_status.led_frequency]);

    /* Start the timers */
    R_AGT_Start(g_periodic_timer.p_ctrl);
    R_AGT_Start(g_agt_blue.p_ctrl);

    return fsp_err;
}
/**********************************************************************************************************************
 End of function common_init
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: print_to_console
 * Description  : .
 * Argument     : p_data contains address of buffer to be printed
 * Return Value : Error/Success result
 *********************************************************************************************************************/
fsp_err_t print_to_console(char_t *p_data)
{
    printf(p_data);
    return (0);
}
/**********************************************************************************************************************
 End of function print_to_console
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: check_for_input_from_console
 * Description  : .
 * Return Value : Error/Success result
 *********************************************************************************************************************/
bool_t check_for_input_from_console(void)
{
    if (STATUS_GET(STATUS_DISPLAY_MENU_KIS) == 0)
    {
        STATUS_CLEAR(STATUS_UPDATE_FREQ_INFO);
        STATUS_CLEAR(STATUS_UPDATE_INTENSE_INFO);
    }

    return (R_CONSOLE_IsInputReady());
}
/**********************************************************************************************************************
 End of function check_for_input_from_console
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: input_from_console
 * Description  : Gets single byte input from console, use function check_for_input_from_console()
 *                to confirm input is available
 * Return Value : Read char
 *********************************************************************************************************************/
int8_t input_from_console(void)
{
    char_t read_byte_buffer;

    R_CONSOLE_ReadByte(&read_byte_buffer);
    return (read_byte_buffer);
}
/**********************************************************************************************************************
 End of function input_from_console
 *********************************************************************************************************************/

/** External IRQ for SW 1 on ICU Instance. */
/**********************************************************************************************************************
 * Function Name: button_nmi_callback
 * Description  : NMI for SW 1 on ICU Instance.
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void button_nmi_callback(bsp_grp_irq_t irq)
{
    /* Void the unused params */
    FSP_PARAMETER_NOT_USED(irq);

    if (STATUS_GET(STATUS_UPDATE_INTENSE_INFO) == 0)
    {
        /* Cast as compiler will assume calc is int */
        g_board_status.led_intensity = (uint16_t)((g_board_status.led_intensity + 1) % 3);
        STATUS_SET(STATUS_UPDATE_INTENSE_INFO);
    }
    R_ICU->NMICLR_b.NMICLR = 1;
}
/**********************************************************************************************************************
 End of function button_nmi_callback
 *********************************************************************************************************************/

/** External IRQ for SW 2 on ICU Instance. */

/**********************************************************************************************************************
 * Function Name: button_irq0_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void button_irq0_callback(external_irq_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    if (STATUS_GET(STATUS_UPDATE_FREQ_INFO) == 0)
    {
        /* Cast to uint16_6, as compiler will assume int for the calculation */
        g_board_status.led_frequency = (uint16_t)((g_board_status.led_frequency + 1) % 3);
        R_AGT_PeriodSet(g_periodic_timer.p_ctrl, s_pwm_rates[g_board_status.led_frequency]);
        STATUS_SET(STATUS_UPDATE_FREQ_INFO);
    }
}
/**********************************************************************************************************************
 End of function button_irq0_callback
 *********************************************************************************************************************/

/** Callback function for driver g_periodic_timer. */
/**********************************************************************************************************************
 * Function Name: periodic_timer_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    if (OFF == s_blueled_flashing)
    {
        s_blueled_flashing = ON;
    }
    else
    {
        s_blueled_flashing = OFF;
    }
}
/**********************************************************************************************************************
 End of function periodic_timer_callback
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: agt_blue_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void agt_blue_callback(timer_callback_args_t *p_args)
{
    /* Cast unused params to void */
    FSP_PARAMETER_NOT_USED(p_args);

    switch (s_blueled_flashing)
    {
        case ON:
        {
            if ((s_intense++ ) < s_duty)
            {
                TURN_BLUE_ON
            }
            else
            {
                TURN_BLUE_OFF
            }

            if ((s_intense) >= 100)
            {
                s_intense = 0;
                s_duty    = s_pwm_dcs[g_board_status.led_intensity];
            }
            break;
        }
        default:
        {
            TURN_BLUE_OFF
            s_intense = 0;
            s_duty = s_pwm_dcs[g_board_status.led_intensity];
        }
    }
}
/**********************************************************************************************************************
 End of function agt_blue_callback
 *********************************************************************************************************************/
