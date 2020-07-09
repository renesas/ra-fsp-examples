/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
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
#include "dac8_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup dac8_ep
 * @{
 **********************************************************************************************************************/

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* Variable to store user input for DAC conversion */
static int32_t g_inputRead = RESET_VALUE;
/* Flag to notify that ADC read is complete */
static volatile bool b_is_read_complete = false;
/* Flag to notify any error in reading ADC data */
static volatile bool b_error_flag = false;
/* Flag to notify successful calibration completion */
static volatile bool b_is_calibration_complete = false;
/* variable to store scanned ADC data */
static uint16_t g_adc_data = RESET_VALUE;
/* Function for ADC calibration on RA2A1 board */
static fsp_err_t adc_start_calibration(void);
/* Function to process user input */
static fsp_err_t read_process_input_from_RTT(void);
/* Function to process user input for DAC8 conversion */
static fsp_err_t dac8_adc_operations(int32_t * input);

/* Functions to close opened modules */
static void dac8_deinit(void);
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
    APP_PRINT(BANNER_INFO, EP_VERSION, version.major, version.minor, version.patch);
    APP_PRINT(EP_INFO);

    /* Configure DAC8 output pin */
    err = R_IOPORT_PinCfg(&g_ioport_ctrl, DAC8_OUTPUT_PIN, IOPORT_CFG_ANALOG_ENABLE | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CAC_AD);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** IOPORT Pincfg failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Open the DAC8 channel */
    err = R_DAC8_Open (&g_dac8_ctrl, &g_dac8_cfg);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* DAC8 module open failed */
        APP_ERR_PRINT("\r\n ** DAC8 module Open failed ** \r\n");
        APP_ERR_TRAP(err);
    }
    /* Open/Initialize ADC module */
    err = R_ADC_Open (&g_adc_ctrl, &g_adc_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /*ADC module open failed */
        dac8_deinit();
        APP_ERR_PRINT("\r\n ** ADC module Open failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Set Reference Voltage Circuit Control register */
    g_adc_ctrl.p_reg->VREFAMPCNT |= ((VREFADCG_VALUE) | (VREFADCG_ENABLE));

    /* Calibrate the ADC for RA2A1 board */
    err = adc_start_calibration ();
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        adc_deinit();
        dac8_deinit();
        /* ADC Failure message */
        APP_ERR_PRINT("** ADC calibration failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Configures the ADC scan parameters */
    err = R_ADC_ScanCfg (&g_adc_ctrl, &g_adc_channel_cfg);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        adc_deinit();
        dac8_deinit();
        /* ADC Failure message */
        APP_ERR_PRINT("** R_ADC_ScanCfg API failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nEnter values for DAC8 conversion. Input Range:1 to 255\r\n")

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
                APP_ERR_PRINT("\r\nInvalid Input. Valid Range: 1 to 255. Please enter valid input \r\n");
            }
            /* Process valid input from user */
            else
            {
                err = dac8_adc_operations(&g_inputRead);
                /* handle error */
                if (FSP_SUCCESS != err)
                {
                    adc_deinit();
                    dac8_deinit();
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
        }
        else if (true == b_error_flag)
        {
            adc_deinit();
            dac8_deinit();
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_Read API failed ** \r\n");
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
    fsp_err_t err = FSP_SUCCESS;     // Error status

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
    err = R_ADC_Calibrate (&g_adc_ctrl, NULL);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n ** ADC Calibrate failed ** \r\n");
        return err;
    }

    while (!b_is_calibration_complete); //Wait here till the calibration is complete.It takes 24msec to 780msec based on clock


    APP_PRINT("\r\nADC Calibration Successful\r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @brief    This function performs DAC conversion and ADC Scan operation
 * @param[IN]   input                   User input for DAC conversion
 * @retval FSP_SUCCESS                  Upon successful of ADC calibration
 * @retval Any Other Error code apart from FSP_SUCCESS  Unsuccessful DAC conversion or ADC scan
 ***********************************************************************************************************************/
static fsp_err_t dac8_adc_operations(int32_t * input)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Write value to DAC module */
    err = R_DAC8_Write (&g_dac8_ctrl, (uint16_t) (* input));
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* DAC8 Write Failure message */
        APP_ERR_PRINT("** R_DAC8_Write API failed ** \r\n");
        return err;
    }

    /* Start DAC8 conversion */
    err = R_DAC8_Start (&g_dac8_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* DAC8 start failure message */
        APP_ERR_PRINT("** R_DAC8_Start API failed ** \r\n");
        return err;
    }

    /* Start the ADC scan in Single scan mode*/
    err = R_ADC_ScanStart (&g_adc_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Scan Failure message */
        APP_ERR_PRINT("** R_ADC_ScanStart API failed ** \r\n");
        return err;
    }

    /* Stop and start DAC8 conversion for consecutive user input values*/
    err = R_DAC8_Stop(&g_dac8_ctrl);
    /* Handle Error */
    if (FSP_SUCCESS != err)
    {
        /* DAC8 stop failure message */
        APP_ERR_PRINT("** R_DAC8_Stop API failed ** \r\n");
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
        err = R_ADC_Read (&g_adc_ctrl, ADC_CHANNEL_0, &g_adc_data);
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
    /*Check if calibration is complete and set flag */
    else if (ADC_EVENT_CALIBRATION_COMPLETE == p_args->event)
    {
        b_is_calibration_complete = true;
    }
    else
    {
        //Do Nothing
    }
}

/*******************************************************************************************************************//**
 * @brief This function closes  opened DAC8 module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void dac8_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close DAC8 module */
    err = R_DAC8_Close(&g_dac8_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* DAC8 Close failure message */
        APP_ERR_PRINT("** DAC8 module Close failed **  \r\n");
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
    err = R_ADC_Close(&g_adc_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ADC Close failure message */
        APP_ERR_PRINT("** ADC module Close failed **  \r\n");
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
 * @} (end addtogroup dac8_ep)
 **********************************************************************************************************************/
