/***********************************************************************************************************************
 * File Name    : adc_ep.c
 * Description  : Contains variables and functions used in adc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "adc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup adc_ep
 * @{
 **********************************************************************************************************************/

/* local variables */
volatile bool g_ready_to_read = false;
static uint16_t g_adc_data;
static bool g_window_comp_event = false;
char volt_str[5] = {RESET_VALUE};
float adc_volt = {RESET_VALUE};
 
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

/* Callback to handle window compare event */
void adc_callback(adc_callback_args_t * p_args);

#ifdef BOARD_RA2A1_EK
static fsp_err_t adc_deviation_in_output(void);

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

    // Read user input
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
    g_window_comp_event = false;

    if (false == g_ready_to_read)
    {
        /* Open/Initialize ADC module */
        err = R_ADC_Open (&g_adc_ctrl, &g_adc_cfg);
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_Open API failed ** \r\n");
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

        /* Disable interrupts */
        R_BSP_IrqDisable((IRQn_Type)ADC_EVENT_SCAN_COMPLETE);

        APP_PRINT("\r\nADC Started Scan\r\n");

        /* Indication to start reading the adc data */
        g_ready_to_read = true;
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

    /* Stop the scan if adc scan is started in continuous scan mode else ignore */
    if((ADC_MODE_SINGLE_SCAN != g_adc_cfg.mode) && (true == g_ready_to_read ))
    {
        /* Stop ADC scan */
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
        g_ready_to_read = false;

        /* Close the ADC module*/
        err = R_ADC_Close (&g_adc_ctrl);
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
    APP_PRINT("\r\nPress any other key (except 1 and 2) to go back to the main menu\r\n");
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
    err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_0, &g_adc_data);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** R_ADC_Read API failed ** \r\n");
        return err;
    }

#ifdef BOARD_RA2A1_EK
    {
        adc_volt = (float)((g_adc_data * V_ref)/ADC_16_BIT);
    }
#else
    {
        adc_volt = (float)((g_adc_data * V_ref)/ADC_12_BIT);
    }
#endif

    snprintf(volt_str,SIZE_64, "%0.2f", adc_volt);

    APP_PRINT("\r\nThe Voltage Reading from ADC: %d\r\n", g_adc_data);
    APP_PRINT("\r\nThe ADC input voltage: %s\r\n", volt_str);

    /* In adc single scan mode after reading the data, it stops.So reset the g_ready_to_read state to
     * avoid reading unnecessarily. close the adc module as it gets opened in start scan command.*/
    if (ADC_MODE_SINGLE_SCAN == g_adc_cfg.mode || g_window_comp_event == true)
    {
        g_ready_to_read = false;

        /* Stop ADC scan */
        err = R_ADC_ScanStop (&g_adc_ctrl);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {   /* ADC ScanStop message */
            APP_ERR_PRINT("** R_ADC_ScanStop API failed ** \r\n");
            APP_ERR_TRAP(err);
        }

        if(((g_window_comp_event == true)&&(ADC_MODE_SINGLE_SCAN == g_adc_cfg.mode))||(ADC_MODE_CONTINUOUS_SCAN == g_adc_cfg.mode))
        {

            /* Print temperature status warning to RTT Viewer */
            if(ADC_L_LMT > g_adc_data)
            {
                APP_PRINT("\r\nADC Voltage is below the Lower Limit. \r\n");
            }
            else if(ADC_H_LMT < g_adc_data)
            {
                APP_PRINT("\r\nADC Voltage is above the Upper Limit.  \r\n");
            }
        }

        /* Close the ADC module*/
        err = R_ADC_Close (&g_adc_ctrl);
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_Close API failed ** \r\n");
            return err;
        }

        APP_PRINT("\r\nPress any other key(except 1 and 2) to go back to the main menu\r\n");
    }


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
    err = R_ADC_Close (&g_adc_ctrl);

    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* GPT Close failure message */
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
static fsp_err_t adc_start_calibration(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status
    adc_status_t adc_status;         // to get adc status

    /* Initiate ADC calibration */
    err = R_ADC_Calibrate (&g_adc_ctrl, NULL);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Failure message */
        APP_ERR_PRINT("** R_ADC_Calibrate API failed ** \r\n");
        return err;
    }

    APP_PRINT("\r\nADC Calibration Started \r\n");

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
    } while (ADC_STATE_IDLE != adc_status.state); //wait here till the calibration.It takes 24msec to 780msec based on clock

    APP_PRINT("\r\nADC Calibration Successful..\r\n");

    return err;
}




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




/* Callback procedure for when window A compare event occurs */
void adc_callback(adc_callback_args_t * p_args)
{
    if(ADC_EVENT_WINDOW_COMPARE_A == p_args->event)
    {
        g_window_comp_event = true;
        IRQn_Type irq = R_FSP_CurrentIrqGet();
        R_BSP_IrqDisable(irq);
    }

}

/*******************************************************************************************************************//**
 * @} (end addtogroup adc_ep)
 **********************************************************************************************************************/
