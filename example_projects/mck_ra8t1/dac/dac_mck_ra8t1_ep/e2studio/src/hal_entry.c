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
#include "dac_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup dac_ep
 * @{
 **********************************************************************************************************************/

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* Variable to store user input for DAC conversion */
static int32_t g_inputRead = RESET_VALUE;
/* Flag to notify that ADC read is complete */
static volatile bool b_is_read_complete = false;
/* Flag to notify any error in reading ADC data */
static volatile bool b_error_flag = false;

/* variable to store scanned ADC data */
static uint16_t g_adc_data = RESET_VALUE;
#if (defined BOARD_RA2A1_EK) || (defined BOARD_RA6T2_MCK)
/* Function for ADC calibration on RA2A1 board */
static fsp_err_t adc_start_calibration(void);
/* Flag to notify successful calibration completion */
static volatile bool b_is_calibration_complete = false;
#endif
/* Function to process user input */
static fsp_err_t read_process_input_from_RTT(void);
/* Function to process user input for dac conversion */
static fsp_err_t dac_adc_operations(int32_t * input);

/* Functions to close opened modules */
static void dac_deinit(void);
static void adc_deinit(void);


/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Configure DAC12 output pin for EK-RA2A1 */
#if defined BOARD_RA2A1_EK
    err = R_IOPORT_PinCfg(&g_ioport_ctrl, DAC12_OUTPUT_PIN, IOPORT_CFG_ANALOG_ENABLE | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CAC_AD);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** IOPORT Pincfg failed ** \r\n");
        APP_ERR_TRAP(err);
    }
#endif

    /* Open the DAC channel */
    err = R_DAC_Open (&g_dac_ctrl, &g_dac_cfg);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* DAC module open failed */
        APP_ERR_PRINT("\r\n ** DAC Open API failed ** \r\n");
        APP_ERR_TRAP(err);
    }
    /* Open/Initialize ADC module */
#if ! (defined(BOARD_RA4W1_EK) || defined(BOARD_RA6T1_RSSK))
#if defined (BOARD_RA6T2_MCK)
    err = R_ADC_B_Open (&g_adc_ctrl, &g_adc_cfg);
#else
    err = R_ADC_Open (&g_adc_ctrl, &g_adc_cfg);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /*ADC module open failed */
        dac_deinit();
        APP_ERR_PRINT("\r\n ** ADC module Open failed ** \r\n");
        APP_ERR_TRAP(err);
    }

#if (defined BOARD_RA2A1_EK) || (defined BOARD_RA6T2_MCK)
    /* Set Reference Voltage Circuit Control register */
    R_ADC0->VREFAMPCNT |= ((VREFADCG_VALUE) | (VREFADCG_ENABLE));

    /* Calibrate the ADC for RA2A1 board */
    err = adc_start_calibration ();
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        adc_deinit();
        dac_deinit();
        /* ADC Failure message */
        APP_ERR_PRINT("** ADC Calibrate API failed ** \r\n");
        APP_ERR_TRAP(err);
    }
#endif

    /* Configures the ADC scan parameters */
#if defined (BOARD_RA6T2_MCK)
    err = R_ADC_B_ScanCfg (&g_adc_ctrl, &g_adc_scan_cfg);
#else
    err = R_ADC_ScanCfg (&g_adc_ctrl, &g_adc_channel_cfg);
#endif
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        adc_deinit();
        dac_deinit();
        /* ADC Failure message */
        APP_ERR_PRINT("** ADC ScanCfg API failed ** \r\n");
        APP_ERR_TRAP(err);
    }
#endif

    APP_PRINT("\r\nEnter values for DAC conversion. Input Range:1 to 4095\r\n");

    while (true)
    {
        if (APP_CHECK_DATA)
        {
            /* Validate input value from user */
            err = read_process_input_from_RTT ();
            /* handle error */
            if (FSP_SUCCESS != err)
            {
                /* Notify user if the input is not valid */
                APP_PRINT("\r\nInvalid Input. Valid Range: 1 to 4095. Please enter valid input \r\n");
            }
            /* Process valid input from user */
            else
            {
                err = dac_adc_operations(&g_inputRead);
                /* handle error */
                if (FSP_SUCCESS != err)
                {
                    adc_deinit();
                    dac_deinit();
                    APP_ERR_PRINT("** Processing input failed ** \r\n");
                    APP_ERR_TRAP(err);
                }
            }
        }

        /* Print read ADC output data */
        if (true == b_is_read_complete)
        {
            b_is_read_complete = false;
            APP_PRINT("\r\nData read from ADC channel: %d\r\n", g_adc_data);

#if defined(BOARD_RA4W1_EK) || defined( BOARD_RA6T1_RSSK)
            adc_deinit();
#endif
        }
        else if (true == b_error_flag)
        {
            adc_deinit();
            dac_deinit();
            /* ADC Failure message */
            APP_ERR_PRINT("** ADC Read API failed ** \r\n");
            APP_ERR_TRAP(err);
        }
        else
        {
            // Do Nothing
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function reads the user input from RTT and processes it.
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful of conversion of user input value
 * @retval Any Other Error code apart from FSP_SUCCESS  Unsuccessful DAC conversion
 ***********************************************************************************************************************/
static fsp_err_t read_process_input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;

    uint8_t readBuff[BUFFER_SIZE_DOWN] = { RESET_VALUE };

    /* Read input value for DAC conversion from user */
    APP_READ(readBuff);

    /* function returns the converted integral number as an int value.*/
    g_inputRead = atoi ((char *) readBuff);

    if ((DAC_MIN_VAL <= g_inputRead) && (DAC_MAX_VAL >= g_inputRead))
    {
        APP_PRINT("\r\nUser input value for DAC conversion : %d",g_inputRead);
    }
    else
    {
        err = FSP_ERR_INVALID_ARGUMENT;
    }

    return err;
}

#if (defined BOARD_RA2A1_EK) || (defined BOARD_RA6T2_MCK)
/*******************************************************************************************************************//**
 * @brief    This function initiates the ADC calibration
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful of ADC calibration
 * @retval Any Other Error code apart from FSP_SUCCESS  Unsuccessful calibration failure
 ***********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    APP_PRINT("\r\nADC Calibration Started \r\n");

    /* Initiate ADC calibration */
#if defined (BOARD_RA6T2_MCK)
    err = R_ADC_B_Calibrate (&g_adc_ctrl, NULL);
#else
    err = R_ADC_Calibrate (&g_adc_ctrl, NULL);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** ADC Calibrate API failed ** \r\n");
        return err;
    }

    while (!b_is_calibration_complete); //Wait here till the calibration is complete.It takes 24msec to 780msec based on clock

    APP_PRINT("\r\nADC Calibration Successful\r\n");

    return err;
}
#endif

/*******************************************************************************************************************//**
 * @brief    This function performs DAC conversion and ADC Scan operation
 * @param[IN]   input                   User input for DAC conversion
 * @retval FSP_SUCCESS                  Upon successful of ADC calibration
 * @retval Any Other Error code apart from FSP_SUCCESS  Unsuccessful DAC conversion or ADC scan
 ***********************************************************************************************************************/
static fsp_err_t dac_adc_operations(int32_t * input)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Write value to DAC module */
    err = R_DAC_Write (&g_dac_ctrl, (uint16_t) (* input));
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* dac Write Failure message */
        APP_ERR_PRINT("** DAC Write API failed ** \r\n");
        return err;
    }

    /* Start DAC conversion */
    err = R_DAC_Start (&g_dac_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* dac start failure message */
        APP_ERR_PRINT("** DAC Start API failed ** \r\n");
        return err;
    }

    /*As per mention in section 35.3.1 of EK-RA4W1 hardware user manual, DAC can't be synchronized with ADC
     * because both are sharing the same bus */
#if defined(BOARD_RA4W1_EK) || defined( BOARD_RA6T1_RSSK)
    /* Open/Initialize ADC module */
    err = R_ADC_Open (&g_adc_ctrl, &g_adc_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /*ADC module open failed */
        dac_deinit();
        APP_ERR_PRINT("\r\n ** ADC module Open failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Configures the ADC scan parameters */
    err = R_ADC_ScanCfg (&g_adc_ctrl, &g_adc_channel_cfg);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        adc_deinit();
        dac_deinit();
        /* ADC Failure message */
        APP_ERR_PRINT("** ADC ScanCfg API failed ** \r\n");
        APP_ERR_TRAP(err);
    }
#endif

    /* As mentioned in DAC properties and Electrical characteristics of RA2A1 User Manual (Section 47.7),
     * a software delay has to be given for stabilizing the voltage on DAC output pin.
     * Also referred as DAC conversion time delay.*/
    R_BSP_SoftwareDelay(STABILIZATION_DELAY, BSP_DELAY_UNITS_MILLISECONDS);

    /* Start the ADC scan in Single scan mode*/
#if defined (BOARD_RA6T2_MCK)
    err = R_ADC_B_ScanStart (&g_adc_ctrl);
#else
    err = R_ADC_ScanStart (&g_adc_ctrl);
#endif
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Scan Failure message */
        APP_ERR_PRINT("** ADC ScanStart API failed ** \r\n");
        return err;
    }

    /* Stop and start DAC conversion for consecutive user input values*/
    err = R_DAC_Stop(&g_dac_ctrl);
    /* Handle Error */
    if (FSP_SUCCESS != err)
    {
        /* DAC stop failure message */
        APP_ERR_PRINT("** DAC Stop API failed ** \r\n");
    }
    return err;
}


/*******************************************************************************************************************//**
 * @brief ADC callback function.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void g_adc_callback(adc_callback_args_t *p_args)
{
    if (ADC_EVENT_SCAN_COMPLETE == p_args->event)
    {
        fsp_err_t err = FSP_SUCCESS;     // Error status

        /* Read the result after ADC Scan is complete */
#if defined BOARD_RA4W1_EK
        err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_4, &g_adc_data);
#elif defined BOARD_RA6T2_MCK
        err = R_ADC_B_Read (&g_adc_ctrl, ADC_CHANNEL_0, &g_adc_data);
#elif defined BOARD_RA8T1_MCK
        err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_2, &g_adc_data);
#else
        err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_0, &g_adc_data);
#endif
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            b_error_flag = true;
        }
        else
        {
            b_is_read_complete = true;
        }
    }

#if (defined BOARD_RA2A1_EK) || (defined BOARD_RA6T2_MCK)
    /*Check if calibration is complete and set flag */
    else if (ADC_EVENT_CALIBRATION_COMPLETE == p_args->event)
    {
        b_is_calibration_complete = true;
    }
    else
    {
        //Do Nothing
    }
#endif
}

/*******************************************************************************************************************//**
 * @brief This function closes  opened DAC module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void dac_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close DAC module */
    err = R_DAC_Close(&g_dac_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* DAC Close failure message */
        APP_ERR_PRINT("** DAC Close API failed **  \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief This function closes  opened ADC module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void adc_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close ADC module */
#if defined (BOARD_RA6T2_MCK)
    err = R_ADC_B_Close(&g_adc_ctrl);
#else
    err = R_ADC_Close(&g_adc_ctrl);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Close failure message */
        APP_ERR_PRINT("** ADC Close API failed **  \r\n");
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
 * @} (end addtogroup dac_ep)
 **********************************************************************************************************************/
