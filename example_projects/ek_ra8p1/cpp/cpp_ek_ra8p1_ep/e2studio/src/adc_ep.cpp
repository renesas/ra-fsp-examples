/***********************************************************************************************************************
 * File Name    : adc_ep.cpp
 * Description  : Contains variables and functions used in adc_ep.cpp
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "adc_ep.h"

/*******************************************************************************************************************//**
 * @brief       This function creates the object.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
adc_ep::adc_ep()
{
    fsp_pack_version_t version = { RESET_VALUE };

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);
    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT(BANNER_ADC);
    APP_PRINT(MENUOPTIONS);
}

/*******************************************************************************************************************//**
 * @brief       This function deletes the created object.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
adc_ep::~adc_ep()
{
    APP_PRINT("** ADC object is deleted **\r\n");
}

/*******************************************************************************************************************//**
 * @brief       Reads the command input from RTT and processes it.
 * @param[IN]   None
 * @retval      FSP_SUCCESS         Successfully started or stopped the ADC based on the input command.
 * @retval      Other error code    Returned if starting or stopping the ADC fails
 **********************************************************************************************************************/
fsp_err_t adc_ep::read_process_input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    uint8_t readBuff[BUFF_SIZE] = { RESET_VALUE };

    uint32_t rByte = APP_READ(readBuff);

    /* Convert the input string to an integer value */
    int32_t inputRead = atoi((char *)readBuff);

    if (RESET_VALUE < rByte)
    {
        switch (inputRead)
        {
            case SCAN_START:
            {
               err = adc_scan_start();
            }
            break;
            case SCAN_STOP:
            {
               err = adc_scan_stop();
            }
            break;
            default:
            {
                APP_PRINT(MENUOPTIONS);
            }
            break;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function opens the ADC, configures it, and starts the scan.
 * @param[IN]   None
 * @retval      FSP_SUCCESS         ADC was successfully opened, configured, calibrated, and the scan was started.
 * @retval      Other error codes   Indicates failure in opening, configuring, calibrating, or starting the ADC scan.
 **********************************************************************************************************************/
fsp_err_t adc_ep::adc_scan_start(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    if (false == g_ready_to_read)
    {
        err = init_adc_module();

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** ADC module initialization failed **\r\n");
            return err;
        }

        /* Configures the ADC scan parameters */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
        err = R_ADC_D_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
        err = R_ADC_B_ScanCfg(&g_adc_ctrl, &g_adc_scan_cfg);
#else
        err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
#endif
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC failure message */
            APP_ERR_PRINT("** R_" ADC_TYPE "_ScanCfg API failed **\r\n");
            return err;
        }

#if defined(BOARD_RA2A1_EK) || defined(BOARD_RA6T2_MCK)
        /* Set Reference Voltage Circuit Control register */
        R_ADC0->VREFAMPCNT |= ((VREFADCG_VALUE << SHIFT_BY_ONE) | (VREFADCG_ENABLE << SHIFT_BY_THREE));
#endif
#if defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT)
        /* Calibrate the ADC */
        err = adc_start_calibration();

        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC failure message */
            APP_ERR_PRINT("** adc_start_calibration function failed **\r\n");
            return err;
        }
#endif

        /* Start the ADC scan */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
        err = R_ADC_D_ScanStart(&g_adc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC failure message */
            APP_ERR_PRINT("** R_ADC_D_ScanStart API failed **\r\n");
            return err;
        }
        /* Setting internal temperature sensor as output, the initial conversion result cannot be used.
         * Scan second time if adc_d scan in One-shot mode */
        if (ADC_D_CONVERSION_MODE_SEQUENTIAL != g_adc_cfg_extend.conversion_operation)
        {
            /* Wait for conversion to complete */
            adc_status_t status;
            status.state = ADC_STATE_SCAN_IN_PROGRESS;
            while (ADC_STATE_SCAN_IN_PROGRESS == status.state)
            {
                (void) R_ADC_D_StatusGet(&g_adc_ctrl, &status);
            }
            /* Start the ADC scan */
            err = R_ADC_D_ScanStart(&g_adc_ctrl);
            if (FSP_SUCCESS != err)
            {
                /* ADC failure message */
                APP_ERR_PRINT("** R_ADC_D_ScanStart API failed **\r\n");
                return err;
            }
            /* Wait for conversion to complete */
            status.state = ADC_STATE_SCAN_IN_PROGRESS;
            while (ADC_STATE_SCAN_IN_PROGRESS == status.state)
            {
                (void) R_ADC_D_StatusGet(&g_adc_ctrl, &status);
            }
        }
#else
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
        err = R_ADC_B_ScanStart(&g_adc_ctrl);
#else
        err = R_ADC_ScanStart(&g_adc_ctrl);
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC failure message */
            APP_ERR_PRINT("** R_" ADC_TYPE "_ScanStart API failed **\r\n");
            return err;
        }
#endif

        APP_PRINT("\r\nADC scan started\r\n");

        /* Indication to start reading the ADC data */
        g_ready_to_read = true;
    }
    else
    {
        APP_PRINT("\r\nADC scan already in progress\r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function stops the ADC scan and closes the ADC.
 * @param[IN]   None
 * @retval      FSP_SUCCESS              ADC scan was successfully stopped and the ADC was closed.
 * @retval      Other error codes        Failure to stop the scan or close the ADC.
 **********************************************************************************************************************/
fsp_err_t adc_ep::adc_scan_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /* Stop the scan if ADC was started (g_ready_to_read == true) and is in continuous scan mode or
     * sequential mode (ADC_D only) */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    if((ADC_D_CONVERSION_MODE_ONESHOT != g_adc_cfg_extend.conversion_operation) && (true == g_ready_to_read ))
    {
        err = R_ADC_D_ScanStop(&g_adc_ctrl);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT == 1U)
    if((ADC_MODE_SINGLE_SCAN != g_adc_cfg_extend.adc_b_mode) && (true == g_ready_to_read))
    {
        err = R_ADC_B_ScanStop (&g_adc_ctrl);
#else
    if((ADC_MODE_SINGLE_SCAN != g_adc_cfg.mode) && (true == g_ready_to_read ))
    {
        err = R_ADC_ScanStop(&g_adc_ctrl);
#endif
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_" ADC_TYPE "_ScanStop API failed **\r\n");
            return err;
        }

        APP_PRINT("\r\nADC scan stopped\r\n");

        /* Reset to indicate stop reading the ADC data */
        g_ready_to_read = false;

        /* Close the ADC module */
        deinit_adc_module();
    }
    else
    {
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
        APP_PRINT("\r\nStop command is not supported in One-shot mode or the user not pressed "\
                  "Start Scan in Sequential mode\r\n");
#else
        APP_PRINT("\r\nStop command is not supported in Single Scan mode or the user not pressed "\
                  "Start Scan in Continuous mode\r\n");
#endif
    }
    APP_PRINT("\r\nPress any other key (except 1 and 2) to go back to the main menu\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Reads ADC output data from the specified channel and checks the ADC status.
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Successfully read the MCU's die temperature value.
 * @retval      Other error codes       Returned when the function fails to read the value or the ADC status is invalid.
 **********************************************************************************************************************/
fsp_err_t adc_ep::adc_read_data(void)
{
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    /* Read the result */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    err = R_ADC_D_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &g_adc_data);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &g_adc_data);
#else
    err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &g_adc_data);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** R_" ADC_TYPE "_Read API failed **\r\n");
        return err;
    }

    APP_PRINT("\r\nMCU die temperature reading from ADC: %d\r\n", g_adc_data);

    /* In ADC single scan mode (or one-shot mode for ADC_D) the scan stops automatically after one reading.
     * Reset the g_ready_to_read flag to avoid unnecessary reads. Close the ADC module, as it was opened during
     * the scan start command. */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    if (ADC_D_CONVERSION_MODE_ONESHOT == g_adc_cfg_extend.conversion_operation)
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    if (ADC_MODE_SINGLE_SCAN == g_adc_cfg_extend.adc_b_mode)
#else
    if (ADC_MODE_SINGLE_SCAN == g_adc_cfg.mode)
#endif
    {
        g_ready_to_read = false;

        /* Close the ADC module */
        deinit_adc_module();
        APP_PRINT("\r\nPress any other key (except 1 and 2) to go back to the main menu\r\n");
    }
    else
    {
#ifdef BOARD_RA2A1_EK
    /* Check for deviation in ADC values to initiate the calibration again */
    err = adc_deviation_in_output();
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        APP_ERR_PRINT("** adc_deviation_in_output function failed **\r\n");
        return err;
    }

    /* Update the current ADC data to previous */
    g_prev_adc_data = g_adc_data;
#endif
    }
    /* Wait 1 second between successive readings */
    R_BSP_SoftwareDelay (ADC_READ_DELAY, BSP_DELAY_UNITS_SECONDS);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Opens the ADC driver.
 * @param[IN]   None
 * @retval      FSP_SUCCESS         Successfully opened the ADC module.
 * @retval      Other error codes   Returned if the ADC module fails to open.
 **********************************************************************************************************************/
fsp_err_t adc_ep::init_adc_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open/Initialize ADC module */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    err = R_ADC_D_Open(&g_adc_ctrl, &g_adc_cfg);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Open(&g_adc_ctrl, &g_adc_cfg);
#else
    err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
#endif
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Closes the ADC driver and handles any errors returned by the closing API.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void adc_ep::deinit_adc_module(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the ADC driver */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
    err = R_ADC_D_Close(&g_adc_ctrl);
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Close(&g_adc_ctrl);
#else
    err = R_ADC_Close(&g_adc_ctrl);
#endif
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC close API failure message */
        APP_ERR_PRINT("** R_" ADC_TYPE "_Close API failed **\r\n");
    }
}

#if defined(BOARD_RA2A1_EK) || (BSP_PERIPHERAL_ADC_B_PRESENT)
/*******************************************************************************************************************//**
 * @brief       Initiates ADC calibration.
 * @param[IN]   None
 * @retval      FSP_SUCCESS         ADC calibration completed successfully.
 * @retval      Other error code    Calibration failed.
 **********************************************************************************************************************/
fsp_err_t adc_ep::adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;                     /* Error status */
    adc_status_t adc_status = {ADC_STATE_IDLE};      /* To get ADC status */
    uint32_t time_out = UINT32_MAX;                  /* Max value of time_out */

    /* Initiate ADC calibration */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
    err = R_ADC_B_Calibrate(&g_adc_ctrl, NULL);
#else
    err = R_ADC_Calibrate(&g_adc_ctrl, NULL);
#endif
    APP_PRINT("\r\nADC calibration started\r\n");

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC failure message */
        APP_ERR_PRINT("** R_" ADC_TYPE "_Calibrate API failed **\r\n");
        return err;
    }

    do
    {
        /* To get the ADC status */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
        err = R_ADC_B_StatusGet(&g_adc_ctrl, &adc_status);
#else
        err = R_ADC_StatusGet(&g_adc_ctrl, &adc_status);
#endif
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC status failure message */
            APP_ERR_PRINT("** R_" ADC_TYPE "_StatusGet API failed **\r\n");
            return err;
        }
    } while ((ADC_STATE_IDLE != adc_status.state) && (RESET_VALUE < --time_out)); 
    /* Wait here till the calibration. It takes 24msec to 780msec based on clock */
    if (RESET_VALUE == time_out)
    {
        APP_PRINT("\r\nADC Calibration is not successful due to time out");
        err = FSP_ERR_INVALID_HW_CONDITION;
    }
    else
    {
        APP_PRINT("\r\nADC Calibration Successful\r\n");
    }

    return err;
}
#endif

#ifdef BOARD_RA2A1_EK
/*******************************************************************************************************************//**
 * @brief       Checks the deviation between the previously measured ADC value and the current value. If the deviation
 *              is observed consistently and exceeds the set limit, calibration is triggered again.
 * @param[IN]   None
 * @retval      FSP_SUCCESS         Successfully calibrated the measured output.
 * @retval      Other error code    Returned if calibration fails or deviation check encounters an error.
 **********************************************************************************************************************/
fsp_err_t adc_ep::adc_deviation_in_output(void)
{
    static uint8_t count = RESET_VALUE;
    fsp_err_t err = FSP_SUCCESS;     /* Error status */

    if (RESET_VALUE != g_prev_adc_data)
    {
        if (((g_prev_adc_data > g_adc_data) && (g_prev_adc_data - g_adc_data > TEMPERATURE_DEVIATION_LIMIT))
                || ((g_prev_adc_data < g_adc_data) && (g_adc_data - g_prev_adc_data > TEMPERATURE_DEVIATION_LIMIT)))
        {
            /* Increment deviation count */
            count++;
            if (TEMPERATURE_DEVIATION_LIMIT < count)
            {

                /* Reset the error count */
                count = RESET_VALUE;

                APP_PRINT("\r\nInitiating calibration of ADC due to deviation in output continuously "\
                          "beyond set limits\r\n");

                /* Stop the scan to initiate calibration */
                err = adc_scan_stop();
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* ADC failure message */
                    APP_ERR_PRINT("** adc_scan_stop function failed **\r\n");
                    return err;
                }

                /* Calibrate the ADC again */
                err = adc_start_calibration();
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    /* ADC failure message */
                    APP_ERR_PRINT("** adc_start_calibration function failed **\r\n");
                    return err;
                }

                /* Clear the current reading */
                g_adc_data = RESET_VALUE;
            }
        }
        else
        {
            count = RESET_VALUE;
        }
    }
    return err;
}
#endif
