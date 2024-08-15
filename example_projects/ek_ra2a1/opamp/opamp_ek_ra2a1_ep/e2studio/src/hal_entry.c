/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "opamp_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup opamp_ep
 * @{
 **********************************************************************************************************************/


FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


/*
 * private function
 */
static void deinit_module(module_name_t module);
#ifdef BOARD_RA2A1_EK
static fsp_err_t adc_start_calibration(void);
#endif
static fsp_err_t read_rtt_input(uint16_t * const p_val);
static fsp_err_t process_opamp_operation(uint16_t * const p_dac_val);

#ifndef BOARD_RA4W1_EK
/*
 * global variables
 */
static volatile bool b_scan_complete_event = false;
#endif

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void) {
    fsp_err_t err              = FSP_SUCCESS;
    uint16_t dac_val           = RESET_VALUE;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Banner information */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* open DAC module */
    err = R_DAC_Open(&g_dac_ctrl, &g_dac_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n R_DAC_Open failed\r\n");
        APP_ERR_TRAP(err);
    }
#ifndef BOARD_RA4W1_EK
    /* open ADC module */
    err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n R_ADC_Open failed \r\n");
        /* close DAC module */
        deinit_module(DAC_MODULE);
        APP_ERR_TRAP(err);
    }
#endif
    /* open OPAMP module */
    err = R_OPAMP_Open(&g_opamp_ctrl, &g_opamp_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\n R_OPAMP_Open failed \r\n");
        /* close DAC module */
        deinit_module(DAC_MODULE);
#ifndef BOARD_RA4W1_EK
        /* close ADC module */
        deinit_module(ADC_MODULE);
#endif
        APP_ERR_TRAP(err);
    }

#ifdef BOARD_RA4W1_EK
    /* Enable AMPPC to form voltage follower */
    R_OPAMP->AMPMC_b.AMPPC2 = SET_BIT;
#endif

#ifdef BOARD_RA2A1_EK
    /* Calibrate the ADC */
    err = adc_start_calibration();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("adc_start_calibration operation failed \r\n");
        /* close all open modules */
        deinit_module(ALL);
        APP_ERR_TRAP(err);
    }
#endif

#ifndef BOARD_RA4W1_EK
    /* Configures the ADC scan parameters */
    err = R_ADC_ScanCfg (&g_adc_ctrl, &g_adc_channel_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_ADC_ScanCfg failed \r\n");
        /* close all open modules */
        deinit_module(ALL);
        APP_ERR_TRAP(err);
    }
#endif
    /* Start DAC */
    err = R_DAC_Start(&g_dac_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_DAC_Start failed ** \r\n");
        /* close all open modules */
        deinit_module(ALL);
        APP_ERR_TRAP(err);
    }

    while (true)
    {
        /* read input from user through JLINK RTT Viewer */
        err = read_rtt_input(&dac_val);
        if (FSP_SUCCESS == err)
        {
            /* If user input is within permissible range process OPAMP operation */
            err = process_opamp_operation(&dac_val);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n OPAMP operation failed\r\n");
                /* close all open modules */
                deinit_module(ALL);
                APP_ERR_TRAP(err);
            }
        }
    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
    if (BSP_WARM_START_POST_C == event) {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*******************************************************************************************************************//**
 * @brief       Close modules
 * @param[IN]   module name
 * @retval      None
 ***********************************************************************************************************************/
static void deinit_module(module_name_t module)
{
    switch (module)
    {
        case DAC_MODULE:
        {
            if (FSP_SUCCESS != R_DAC_Close(&g_dac_ctrl))
            {
                APP_ERR_PRINT ("\r\n R_DAC_Close failed \r\n ");
            }
        }
        break;
#ifndef BOARD_RA4W1_EK
        case ADC_MODULE:
        {
            if (FSP_SUCCESS != R_ADC_Close(&g_adc_ctrl))
            {
                APP_ERR_PRINT ("\r\n R_ADC_Close failed \r\n ");
            }
        }
        break;
#endif
        case OPAMP_MODULE:
        {
            if (FSP_SUCCESS != R_OPAMP_Close(&g_opamp_ctrl))
            {
                APP_ERR_PRINT ("\r\n R_OPAMP_Close failed \r\n ");
            }
        }
        break;
        case ALL:
        default:
        {
            if (FSP_SUCCESS != R_DAC_Close(&g_dac_ctrl))
            {
                APP_ERR_PRINT ("\r\n R_DAC_Close failed \r\n ");
            }
#ifndef BOARD_RA4W1_EK
            if (FSP_SUCCESS != R_ADC_Close(&g_adc_ctrl))
            {
                APP_ERR_PRINT ("\r\n R_ADC_Close failed \r\n ");
            }
#endif
            if (FSP_SUCCESS != R_OPAMP_Close(&g_opamp_ctrl))
            {
                APP_ERR_PRINT ("\r\n R_OPAMP_Close failed \r\n ");
            }
        }
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief       read user input from JLINK RTT viewer
 * @param[OUT]  DAC Value obtained from user
 * @retval      FSP_SUCCESS or any other possible error codes
 ***********************************************************************************************************************/
static fsp_err_t read_rtt_input(uint16_t * const p_val)
{
    uint16_t dac_val               = RESET_VALUE;
    uint8_t num_bytes              = RESET_VALUE;
    char dac_buf[BUFFER_SIZE_DOWN] = {NULL_CHAR,};

    if (NULL == p_val)
    {
        return FSP_ERR_ASSERTION;
    }

    /* Entered DAC value is used as input to OPAMP AMP- pin */
    APP_PRINT(" \r\nEnter DAC Value (0 - %d) for OPAMP input:\r\n\r\n", DAC_MAX_VAL);


    while (RESET_VALUE == num_bytes)
    {
        if (APP_CHECK_DATA)
        {
            num_bytes = (uint8_t)APP_READ(dac_buf);
            if (RESET_VALUE == num_bytes)
            {
                APP_ERR_PRINT(" \r\nInvalid Input\r\n");
            }
        }
    }

    /* Conversion from input string to integer value */
    dac_val =  (uint16_t) (atoi(dac_buf));

    if (DAC_MAX_VAL < dac_val)
    {
        APP_ERR_PRINT(" \r\n ** INVALID INPUT, DAC VALUE IS OUT OF RANGE (0 - 4095)  ** \r\n");
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* display input DAC value in RTT viewer */
    ANALOG_VALUE(dac_val, DAC_MODULE);

    *p_val = dac_val;

    return FSP_SUCCESS;
}

#ifndef BOARD_RA4W1_EK
/*******************************************************************************************************************//**
 * @brief       ADC callback
 * @param[IN]   p_args callback arguments
 * @retval      None
 ***********************************************************************************************************************/
void adc_callback(adc_callback_args_t *p_args)
{
    if ( (NULL != p_args) && (ADC_EVENT_SCAN_COMPLETE == p_args->event) )
    {
        b_scan_complete_event = true;
    }
}
#endif

/*******************************************************************************************************************//**
 * @brief       This function process OPAMP operation DAC value is feed to OPAMP input and
 *              its output is sent to ADC
 * @param[IN]   user input DAC value
 * @retval      FSP_SUCCESS or any other possible error codes
 ***********************************************************************************************************************/
static fsp_err_t process_opamp_operation(uint16_t * const p_dac_val)
{
    fsp_err_t err     = FSP_SUCCESS;
    opamp_info_t info = {.min_stabilization_wait_us = RESET_VALUE};
#ifndef BOARD_RA4W1_EK
    int32_t timeout   = INT32_MAX;
    uint16_t adc_data = RESET_VALUE;
#endif
    /* Write user input value to DAC */
    err = R_DAC_Write(&g_dac_ctrl, *p_dac_val);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_DAC_Write failed ** \r\n");
        return err;
    }

    /* Analog input provided through DAC output -> starting OPAMP */
    err = R_OPAMP_Start(&g_opamp_ctrl, OPAMP_CH_MASK);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_OPAMP_Start failed ** \r\n");
        return err;
    }

    /* Look up the required stabilization wait time. */
    err = R_OPAMP_InfoGet(&g_opamp_ctrl, &info);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_OPAMP_InfoGet failed\r\n");
        return err;
    }

    /* Wait for the OPAMP to stabilize. */
    R_BSP_SoftwareDelay(info.min_stabilization_wait_us, BSP_DELAY_UNITS_MICROSECONDS);
#ifndef BOARD_RA4W1_EK
    /* Start the ADC scan*/
    err = R_ADC_ScanStart (&g_adc_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("R_ADC_ScanStart failed \r\n");
        return err;
    }

    /* wait for callback -> ADC event to accomplish scan process */
    while (true != b_scan_complete_event)
    {
        --timeout;
        if (RESET_VALUE >= timeout)
        {
            APP_ERR_PRINT("\r\n ** Timeout : Scan complete callback event not received ** \r\n");
            return FSP_ERR_TIMEOUT;
        }
    }

    /* reset callback flag */
    b_scan_complete_event = false;

    /* read ADC data */
    err =  R_ADC_Read(&g_adc_ctrl, ADC_CHANNEL, &adc_data);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("R_ADC_Read failed ** \r\n");
        return err;
    }

    /* Display read ADC data */
    ANALOG_VALUE(adc_data, ADC_MODULE);

    /* Stop OPAMP */
    err = R_OPAMP_Stop(&g_opamp_ctrl, OPAMP_CH_MASK);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_OPAMP_Stop failed ** \r\n");
    }
#endif
    return err;
}

#ifdef BOARD_RA2A1_EK
/*******************************************************************************************************************//**
 * @brief       Calibration is required for ADC16.Calibration allows high-precision A/D conversion
 *              by obtaining the Capacitor Array DAC (C-DAC) linearity error correction value
 *              and gain (offset) error correction value under the conditions of use.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful of adc calibration
 * @retval      Any Other Error code apart from FSP_SUCCES  Unsuccessful calibration failure
 ***********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;
    adc_status_t adc_status = {.state = ADC_STATE_IDLE};
    int32_t timeout = INT32_MAX;

    /* Initiate ADC calibration */
    err = R_ADC_Calibrate (&g_adc_ctrl, NULL);
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT(" R_ADC_Calibrate failed ** \r\n");
        return err;
    }

    do
    {
        /* To get the ADC status*/
        err = R_ADC_StatusGet (&g_adc_ctrl, &adc_status);
        if (FSP_SUCCESS != err)
        {
            /* ADC status Failure message */
            APP_ERR_PRINT("R_ADC_StatusGet failed ** \r\n");
            return err;
        }

        timeout--;

        /* To avoid infinite loop in case of consistent failures */
        if (RESET_VALUE >= timeout)
        {
            APP_ERR_PRINT("\r\n ** Timeout : status not received from R_ADC_StatusGet API ** \r\n");
            return FSP_ERR_TIMEOUT;
        }
        /*wait here till the calibration.It takes 24msec to 780msec based on clock*/
    } while (ADC_STATE_IDLE != adc_status.state);

    return err;
}
#endif

/*******************************************************************************************************************//**
 * @} (end addtogroup opamp_ep)
 **********************************************************************************************************************/
