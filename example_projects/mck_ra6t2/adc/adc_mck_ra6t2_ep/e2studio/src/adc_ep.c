/***********************************************************************************************************************
 * File Name    : adc_ep.c
 * Description  : Contains variables and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "adc_ep.h"


/*******************************************************************************************************************//**
 * @addtogroup adc_ep
 * @{
 **********************************************************************************************************************/

/* Flag to notify that adc scan is started, so start reading adc */
volatile bool b_ready_to_read = false;

static uint16_t g_adc_b_data;

#ifdef BOARD_RA6T2_MCK
extern const adc_b_extended_cfg_t g_adc_b_cfg_extend;
#endif

#ifdef BOARD_RA2A1_EK
static uint16_t g_prev_adc_data;
#endif
/*
 * private function declarations
 */

/* Open the adc module, configures and initiates the scan*/
static fsp_err_t adc_scan_start(void);

/* stops the adc scan if the adc is continuous scan and then close the module */
static fsp_err_t adc_scan_stop(void);


#ifdef BOARD_RA2A1_EK
static fsp_err_t adc_deviation_in_output(void);
#endif

#if (defined BOARD_RA2A1_EK) || (defined BOARD_RA6T2_MCK)
static fsp_err_t adc_start_calibration(void);
#endif

/*******************************************************************************************************************//**
 * @brief     This function reads the command (input) from RTT and process the command(input).
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful, action of adc start or adc stop takes place
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful start or stop
 ***********************************************************************************************************************/
fsp_err_t read_process_input_from_RTT(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    uint8_t readBuff[BUFF_SIZE] =  { RESET_VALUE };

    uint32_t rByte = APP_READ(readBuff);

    /* function returns the converted integral number as an int value.*/
    int32_t inputRead = atoi((char *)readBuff);

    if (rByte > RESET_VALUE)
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
                /* Menu for User Selection */
                APP_PRINT(MENUOPTION1);
                APP_PRINT(MENUOPTION2);
                APP_PRINT(MENUOPTION3);
                APP_PRINT(MENUOPTION4);
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
static fsp_err_t adc_scan_start(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status
    if (false == b_ready_to_read)
    {
        /* Open/Initialize ADC module */
#ifdef BOARD_RA6T2_MCK
        err = R_ADC_B_Open (&g_adc_b_ctrl, &g_adc_b_cfg);
#else
        err = R_ADC_Open (&g_adc_ctrl, &g_adc_cfg);
#endif
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_Open API failed ** \r\n");
            return err;
        }

#if (defined BOARD_RA2A1_EK)
        /* Set Reference Voltage Circuit Control register */
        R_ADC0->VREFAMPCNT |= ((VREFADCG_VALUE << SHIFT_BY_ONE) | (VREFADCG_ENABLE << SHIFT_BY_THREE));
#endif
#if (defined BOARD_RA2A1_EK) || (defined BOARD_RA6T2_MCK)
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
#ifdef BOARD_RA6T2_MCK
        err = R_ADC_B_ScanCfg (&g_adc_b_ctrl, &g_adc_b_scan_cfg);
#else
        err = R_ADC_ScanCfg (&g_adc_ctrl, &g_adc_channel_cfg);
#endif

        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_ScanCfg API failed ** \r\n");
            return err;
        };
        /* Start the ADC scan*/
#ifdef BOARD_RA6T2_MCK
        err = R_ADC_B_ScanStart(&g_adc_b_ctrl);
#else
        err = R_ADC_ScanStart (&g_adc_ctrl);
#endif
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
static fsp_err_t adc_scan_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Stop the scan if adc scan is started in continous scan mode else ignore */
#ifdef BOARD_RA6T2_MCK
    if((ADC_MODE_SINGLE_SCAN != g_adc_b_cfg_extend.adc_b_mode) && (true == b_ready_to_read ))
#else
    if((ADC_MODE_SINGLE_SCAN != g_adc_b_cfg.mode) && (true == b_ready_to_read ))
#endif
        {
#ifdef BOARD_RA6T2_MCK
        err = R_ADC_B_ScanStop (&g_adc_b_ctrl);
#else
        err = R_ADC_ScanStop (&g_adc_ctrl);
#endif
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
#ifdef BOARD_RA6T2_MCK
        err = R_ADC_B_Close (&g_adc_b_ctrl);
#else
        err = R_ADC_Close (&g_adc_ctrl);
#endif

        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_Close API failed ** \r\n");
            return err;
        }

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
 * @retval FSP_SUCCESS                  Upon successful stops the adc scan and closes the adc
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful stop or close of adc
 ***********************************************************************************************************************/
fsp_err_t adc_read_data(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Read the result */
#ifdef BOARD_RA6T2_MCK
    err = R_ADC_B_Read (&g_adc_b_ctrl, ADC_CHANNEL_TEMPERATURE, &g_adc_b_data);
#else
    err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_TEMPERATURE, &g_adc_data);
#endif

    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** R_ADC_Read API failed ** \r\n");
        return err;
    }

    APP_PRINT("\r\nMCU Temperature Reading from ADC: %d\r\n", g_adc_b_data);

    /* In adc single scan mode after reading the data, it stops.So reset the b_ready_to_read state to
     * avoid reading unnecessarily. close the adc module as it gets opened in start scan command.*/
#ifdef BOARD_RA6T2_MCK
    if (ADC_MODE_SINGLE_SCAN == g_adc_b_cfg_extend.adc_b_mode)
#else
    if (ADC_MODE_SINGLE_SCAN == g_adc_b_cfg.mode)
#endif
    {
        b_ready_to_read = false;

        /* Close the ADC module*/
#ifdef BOARD_RA6T2_MCK
        err = R_ADC_B_Close (&g_adc_b_ctrl);
#else
        err = R_ADC_Close (&g_adc_ctrl);
#endif

        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_Close API failed ** \r\n");
            return err;
        }

        APP_PRINT("\r\nPress any other key(except 1 and 2) to go back to the main menu\r\n");
    }
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

    /* 1 Seconds Wait time between successive readings */
    R_BSP_SoftwareDelay (ADC_READ_DELAY, BSP_DELAY_UNITS_SECONDS);
    return err;
}
/*******************************************************************************************************************//**
 * @brief    Close the adc driver and Handle the return closing API error, to the Application.
 * @param[IN]   None
 * @retval None.
 ***********************************************************************************************************************/
void deinit_adc_module(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* close the ADC driver */
#ifdef BOARD_RA6T2_MCK
        err = R_ADC_B_Close (&g_adc_b_ctrl);
#else
        err = R_ADC_Close (&g_adc_ctrl);
#endif

    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* GPT Close failure message */
        APP_ERR_PRINT("** R_ADC_Close API failed **  \r\n");
    }
}

#if (defined BOARD_RA2A1_EK) || (defined BOARD_RA6T2_MCK)
/*******************************************************************************************************************//**
 * @brief    This function initiates the calibration
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful of adc calibration
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful calibration failure
 ***********************************************************************************************************************/
static fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status
    adc_status_t adc_status;         // to get adc status

    /* Initiate ADC calibration */
#ifdef BOARD_RA6T2_MCK
    err = R_ADC_B_Calibrate (&g_adc_b_ctrl, NULL);
#else
    err = R_ADC_Calibrate (&g_adc_ctrl, NULL);
#endif


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
#ifdef BOARD_RA6T2_MCK
        err = R_ADC_B_StatusGet (&g_adc_b_ctrl, &adc_status);
#else
        err = R_ADC_StatusGet (&g_adc_ctrl, &adc_status);
#endif
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC status Failure message */
            APP_ERR_PRINT("** R_ADC_StatusGet API failed ** \r\n");
            return err;
        }
    } while (ADC_STATE_IDLE != adc_status.state); //wait here till the calibration.It takes 24msec to 780msec based on clock

    APP_PRINT("\r\nADC Calibration Successfull..\r\n");

    return err;
}
#endif

#ifdef BOARD_RA2A1_EK

/*******************************************************************************************************************//**
 * @brief    This function checks the deviation between previously measured adc value and current value. If the devi
 *           is observed continuously and exceeds set limit, triggers calibration again
 * @param[IN]   None
 * @retval FSP_SUCCESS                  Upon successful stops the adc scan and closes the adc
 * @retval Any Other Error code apart from FSP_SUCCES  Unsuccessful stop or close of adc
 ***********************************************************************************************************************/
static fsp_err_t adc_deviation_in_output(void)
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

/*******************************************************************************************************************//**
 * @} (end addtogroup adc_ep)
 **********************************************************************************************************************/
