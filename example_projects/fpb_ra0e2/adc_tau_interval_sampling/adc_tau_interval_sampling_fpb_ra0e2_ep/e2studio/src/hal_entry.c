/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "common_utils.h"
#include "dtc_hal.h"
#include "adc_interval_scan.h"
#include "adc_hal.h"
#include "tau_hal.h"
#include "elc_hal.h"

/*******************************************************************************************************************//**
 * @addtogroup adc_tau_interval_sampling_ep
 * @{
 **********************************************************************************************************************/

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/* Private function declarations */
static void general_signal_acquisition_init (void);
static void handle_error(fsp_err_t err, char *err_str, module_name_t module);

/* Extern variables */
extern uint16_t g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_NUM][NUM_ADC_CHANNELS][NUM_SAMPLE_BUFFER]\
                            [NUM_SAMPLES_PER_CHANNEL];
extern transfer_info_t g_transfer_adc_group[];
extern volatile bool g_adc0_group_flag;
extern volatile bool g_err_flag_adc0;

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used. This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_pack_version_t version                 = {RESET_VALUE};
    fsp_err_t          err                     = FSP_SUCCESS;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the RTT */
    APP_PRINT (BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, \
               version.version_id_b.patch);
    APP_PRINT (EP_INFO);

    /* Initialize the HAL driver's for signal acquisition */
    general_signal_acquisition_init();

    APP_PRINT("\r\nADC interval scan started...\r\n");

    /* Start ADC */
    err = scan_start_adc(&g_adc0_ctrl);
    handle_error(err,"\r\n** start_adc FAILED ** \r\n", ALL);

    /* Start tau timer */
    err = tau_timer_start(&g_timer0_ctrl);
    handle_error(err,"\r\n** start TAU timer FAILED ** \r\n", ALL);

    APP_PRINT("\r\nThe user can now open waveform rendering of memory in e2studio to observe the output "
              "of adc tau interval sampling.\n\n");

    /* Wait for callback event */
    while(true)
    {
        /* Check if ADC 0 scan complete event for ADC group is received */
        if(true == g_adc0_group_flag)
        {
            /* Buffer select for ping pong buffer */
            static uint8_t buffer_select = ZERO;
            buffer_select = (buffer_select == ZERO) ? ONE : ZERO;

            /* Update destination address and length for transfer */
            g_transfer_adc_group[ZERO].p_dest
            = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN][ZERO][buffer_select][ZERO];
            g_transfer_adc_group[ONE].p_dest
            = (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN][ONE][buffer_select][ZERO];
            g_transfer_adc_group[ZERO].length = NUM_SAMPLES_PER_CHANNEL;
            g_transfer_adc_group[ONE].length = NUM_SAMPLES_PER_CHANNEL;

            /* Enable transfer */
            err = dtc_enable(&g_transfer_adc0_group_ctrl);
            handle_error(err,"\r\n** dtc enable failed for adc group  ** \r\n", ALL);

            /* Reset the variable */
            g_adc0_group_flag = false;
        }
        /* Check if ADC scan complete event is not received */
        else if (true == g_err_flag_adc0)
        {
            handle_error(FSP_ERR_ABORTED,"\r\n** adc scan complete event for ADC group is not received  ** \r\n", ALL);
        }
        else
        {
            /* Do nothing */
        }
    }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * @brief       This function initializes and enables ADC, TAU, DTC and ELC modules to be used as signal
 *              acquisition module.
 * @param[IN]   None
 * @retval      None
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open.
 **********************************************************************************************************************/
static void general_signal_acquisition_init (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize all the links in the Event Link Controller */
    err = init_hal_elc(&g_elc_ctrl,&g_elc_cfg);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** init_hal_elc FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Initialize DTC instance and reconfigure in chain mode for instance unit, ADC group */
    err = init_hal_dtc(&g_transfer_adc0_group_ctrl, &g_transfer_adc0_group_cfg);
    handle_error(err,"\r\n** dtc_init for unit 0, group a failed ** \r\n", ELC_MODULE);

    err = dtc_hal_reconfigure(&g_transfer_adc0_group_ctrl,  &g_transfer_adc_group[ZERO]);
    handle_error(err,"\r\n** dtc reconfiguration for unit 0, group a failed ** \r\n", ELC_DTC_MODULE);

    /* Initialize ADC Unit and configure channels for it */
    err = init_hal_adc(&g_adc0_ctrl,&g_adc0_cfg);
    handle_error(err,"\r\n** adc_init for unit 0 failed ** \r\n", ELC_DTC_MODULE);

    err = adc_channel_config(&g_adc0_ctrl, &g_adc0_channel_cfg);
    handle_error(err,"\r\n** adc_channel_config for unit 0 failed ** \r\n", ELC_DTC_ADC_MODULE);

    /* Initialize TAU timer 0 */
    err = init_hal_tau(&g_timer0_ctrl, &g_timer0_cfg);
    handle_error(err,"\r\n** tau_init for timer 0 failed ** \r\n", ELC_DTC_ADC_MODULE);

    /* Enable the operation of the Event Link Controller */
    err = elc_enable(&g_elc_ctrl);
    handle_error(err,"\r\n** R_ELC_Enable failed ** \r\n", ALL);

    /* Enable transfers for ADC group */
    err = dtc_enable(&g_transfer_adc0_group_ctrl);
    handle_error(err,"\r\n** dtc_enable for ADC unit 0 group a failed ** \r\n", ALL);
}

/*******************************************************************************************************************//**
 *  @brief       Closes the ELC, DTC, TAU and ADC module, Print and traps error.
 *  @param[IN]   status    error status
 *  @param[IN]   err_str   error string
 *  @param[IN]   module    module to be closed
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error( fsp_err_t err, char *err_str, module_name_t module)
{
    if(FSP_SUCCESS != err)
    {
        switch (module)
        {
            case ELC_MODULE:
            {
                /* Close ELC instance */
                deinit_hal_elc(&g_elc_ctrl);
            }
            break;
            case ELC_DTC_MODULE:
            {
                /* Close ELC instance */
                deinit_hal_elc(&g_elc_ctrl);

                /* Close DTC instance */
                deinit_hal_dtc(&g_transfer_adc0_group_ctrl);
            }
            break;
            case ELC_DTC_ADC_MODULE:
            {
                /* Close ELC instance */
                deinit_hal_elc(&g_elc_ctrl);

                /* Close DTC instance */
                deinit_hal_dtc(&g_transfer_adc0_group_ctrl);

                /* Close ADC instance */
                deinit_hal_adc(&g_adc0_ctrl);
            }
            break;
            case ALL:
            {
                /* Close ELC instance */
                deinit_hal_elc(&g_elc_ctrl);

                /* Close DTC instance */
                deinit_hal_dtc(&g_transfer_adc0_group_ctrl);

                /* Close ADC instance */
                deinit_hal_adc(&g_adc0_ctrl);

                /* Close TAU instance */
                deinit_hal_tau(&g_timer0_ctrl);
            }
            break;
            default:
            {
                /* Do nothing */
            }
        }
        APP_PRINT(err_str);
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process. This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take
         * more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open( &g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is
 * not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif

/*******************************************************************************************************************//**
 * @} (end addtogroup adc_tau_interval_sampling_ep)
 **********************************************************************************************************************/
