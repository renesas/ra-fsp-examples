/***********************************************************************************************************************
 * File Name    : adc_ep.cpp
 * Description  : Contains variables and functions used in adc_ep.cpp
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/


#include <adc_ep.h>
/*******************************************************************************************************************//**
 * @brief  This function is to create the object.
 * @param[IN]   None
 * @retval      None
  ***********************************************************************************************************************/
adc_ep::adc_ep()
{
    fsp_pack_version_t version = { RESET_VALUE };

    /* version get API for FLEX pack information */
    R_FSP_VersionGet (&version);
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(BANNER_ADC);
    APP_PRINT(MENUOPTIONS);
}

/*******************************************************************************************************************//**
 * @brief  This function is to delete the created object
 * @param[IN]   None
 * @retval      None
  ***********************************************************************************************************************/
adc_ep::~adc_ep()
{
    APP_PRINT("** ADC object is deleted ** \r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function reads the command (input) from RTT and process the command(input).
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful, action of adc start or adc stop takes place
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful start or stop
 ***********************************************************************************************************************/
fsp_err_t adc_ep::read_process_input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    uint8_t readBuff[BUFF_SIZE] =  { RESET_VALUE };

    uint32_t rByte = APP_READ(readBuff);

    /* function returns the converted integral number as an int value.*/
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
 * @brief    This function open the ADC, configures and starts the scan
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful open,configure adc,on success or calibrate and start adc scan
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful open or configure or start
 ***********************************************************************************************************************/
fsp_err_t adc_ep::adc_scan_start(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    if (false == b_ready_to_read)
    {
        err = init_adc_module();

        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** ADC module initialization failed ** \r\n");
            return err;
        }
#ifdef BOARD_RA2A1_EK
        /* Set Reference Voltage Circuit Control register */
        R_ADC0->VREFAMPCNT |= ((VREFADCG_VALUE << SHIFT_BY_ONE) | (VREFADCG_ENABLE << SHIFT_BY_THREE));

        /* Calibrate the ADC */
        err = adc_start_calibration();

        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** adc_start_calibration function failed ** \r\n");
            return err;
        }
#endif
        /* Configures the ADC scan parameters */
        err = R_ADC_ScanCfg (&g_adc_ctrl, &g_adc_channel_cfg);
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_ScanCfg API failed ** \r\n");
            return err;
        }

        /* Start the ADC scan*/
        err = R_ADC_ScanStart (&g_adc_ctrl);

        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_ScanStart API failed ** \r\n");
            return err;
        }

        APP_PRINT("\r\nADC Started Scan\r\n");

        /* Indication to start reading the adc data */
        b_ready_to_read = true;
    }
    else
    {
        APP_PRINT("\r\nADC Scan already in progress\r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief    This function stops the scanning of adc
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful stops the adc scan and closes the adc
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful stop or close of adc
 ***********************************************************************************************************************/
fsp_err_t adc_ep::adc_scan_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Stop the scan if adc scan is started in continous scan mode else ignore */
    if((ADC_MODE_SINGLE_SCAN != g_adc_cfg.mode) && (true == b_ready_to_read ))
    {
        err = R_ADC_ScanStop (&g_adc_ctrl);
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_ScanStop API failed ** \r\n");
            return err;
        }

        APP_PRINT("\r\nADC Scan stopped\r\n");

        /* reset to indicate stop reading the adc data */
        b_ready_to_read = false;

        /* Close the ADC module*/
        deinit_adc_module();

    }
    else
    {
        APP_PRINT("\r\nStop command is not supported in Single Scan mode or User not pressed Start Scan in Continuous mode \r\n");
    }
    APP_PRINT("\r\nPress any other key(except 1 and 2) to go back to the main menu\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief    This function reads the adc output data from the prescribed channel and checks adc status
 * @param[IN]   None
 * @retval FSP_SUCCESS   Successfully read the mcu's die temperature values
 * @retval Returns error code if return value of err is apart from FSP_SUCCESS.
 ***********************************************************************************************************************/
fsp_err_t adc_ep::adc_read_data(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status
   /* For MCU FPB_RA4E1 and FPB_RA6E1 input is internal reference voltage, as temperature sensor is not present*/
 #if defined (BOARD_RA4E1_FPB)  || defined (BOARD_RA6E1_FPB)

         /* Read the result */
         err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_VOLT, &g_adc_data);

            /* handle error */
            if (FSP_SUCCESS != err)
            {
                /* ADC Failure message */
                APP_ERR_PRINT("** R_ADC_Read API failed ** \r\n");
                return err;
            }

            APP_PRINT("\r\nMCU Internal Reference Voltage Reading from ADC: %d\r\n", g_adc_data);

    #else
    /* Read the result */
    err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &g_adc_data);

    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** R_ADC_Read API failed ** \r\n");
        return err;
    }

    APP_PRINT("\r\nMCU Die Temperature Reading from ADC: %d\r\n", g_adc_data);

#endif
    /* In adc single scan mode after reading the data, it stops.So reset the b_ready_to_read state to
     * avoid reading unnecessarily. close the adc module as it gets opened in start scan command.*/
    if (ADC_MODE_SINGLE_SCAN == g_adc_cfg.mode)
    {
        b_ready_to_read = false;

        /* Close the ADC module*/
        deinit_adc_module();
        APP_PRINT("\r\nPress any other key(except 1 and 2) to go back to the main menu\r\n");
    }
    else
    {
#ifdef BOARD_RA2A1_EK
    /* check for deviation in adc values to initiate the calibration again*/
    err = adc_deviation_in_output ();
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** adc_deviation_in_output function failed ** \r\n");
        return err;
    }

    /* update the current adc data to previous */
    g_prev_adc_data = g_adc_data;

#endif
    }
    /* 1 Seconds Wait time between successive readings */
    R_BSP_SoftwareDelay (ADC_READ_DELAY, BSP_DELAY_UNITS_SECONDS);
    return err;
}

/*******************************************************************************************************************//**
 * @brief    Opens the adc driver
 * @param[IN]   None
 * @retval FSP_SUCCESS  Successfully opened the adc module
 * @retval Returns error code if return value of err is apart from FSP_SUCCESS.
 ***********************************************************************************************************************/
fsp_err_t adc_ep::init_adc_module(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open/Initialize ADC module */
    err = R_ADC_Open (&g_adc_ctrl, &g_adc_cfg);
    return err;
}

/*******************************************************************************************************************//**
 * @brief    Close the adc driver and Handle the return closing API error, to the Application.
 * @param[IN]   None
 * @retval None.
 ***********************************************************************************************************************/
void adc_ep::deinit_adc_module(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* close the ADC driver */
    err = R_ADC_Close (&g_adc_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Close API failure message */
        APP_ERR_PRINT("** R_ADC_Close API failed **  \r\n");
    }
}

#ifdef BOARD_RA2A1_EK
/*******************************************************************************************************************//**
 * @brief    This function initiates the calibration
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful of adc calibration
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful calibration failure
 ***********************************************************************************************************************/
fsp_err_t adc_ep::adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status
    adc_status_t adc_status = {ADC_STATE_IDLE};      // to get adc status
    uint32_t time_out = UINT32_MAX;                 //max value of time_out

    /* Initiate ADC calibration */
    err = R_ADC_Calibrate (&g_adc_ctrl, NULL);

    APP_PRINT("\r\nADC Calibration Started \r\n");

    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** R_ADC_Calibrate API failed ** \r\n");
        return err;
    }

    do
    {
        /* To get the adc status*/
        err = R_ADC_StatusGet (&g_adc_ctrl, &adc_status);
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC status Failure message */
            APP_ERR_PRINT("** R_ADC_StatusGet API failed ** \r\n");
            return err;
        }
    } while ((ADC_STATE_IDLE != adc_status.state) && (RESET_VALUE < --time_out)); //wait here till the calibration.It takes 24msec to 780msec based on clock
    if (RESET_VALUE == time_out)
    {
        APP_PRINT("\r\n ADC Calibration is not successful due to time out");
        err = FSP_ERR_INVALID_HW_CONDITION;
    }
    else
    {
        APP_PRINT("\r\nADC Calibration Successfull..\r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief    This function checks the deviation between previously measured adc value and current value. If the devi
 *           is observed continuously and exceeds set limit, triggers calibration again
 * @param[IN]   None
 * @retval FSP_SUCCESS   Successfully calibrated the measured output.
 * @retval Returns error code if return value of err is apart from FSP_SUCCESS.
 ***********************************************************************************************************************/
fsp_err_t adc_ep::adc_deviation_in_output(void)
{
    static uint8_t count = RESET_VALUE;
    fsp_err_t err = FSP_SUCCESS;     // Error status

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

                APP_PRINT("\r\nInitiating calibration of ADC due to deviation in output continuously beyond set limits\r\n");

                /* Stop the scan to initiate calibration */
                err = adc_scan_stop ();
                /* handle error */
                if (FSP_SUCCESS != err)
                {
                    /* ADC Failure message */
                    APP_ERR_PRINT("** adc_scan_stop function failed ** \r\n");
                    return err;
                }

                /* Calibrate the ADC again */
                err = adc_start_calibration ();
                /* handle error */
                if (FSP_SUCCESS != err)
                {
                    /* ADC Failure message */
                    APP_ERR_PRINT("** adc_start_calibration function failed ** \r\n");
                    return err;
                }

                /* clear the current reading */
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
