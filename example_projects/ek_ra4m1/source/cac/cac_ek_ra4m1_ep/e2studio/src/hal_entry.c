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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "cac_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup cac_ep
 * @{
 **********************************************************************************************************************/

/* Global variable */
/* variable to capture CAC events. */
static volatile uint8_t g_cac_event = CAC_EVENT_VALUE;
static volatile bool b_cac_status_flag = false;
/* Boolean flag to determine switch is pressed or not.*/
static volatile bool b_sw_press = false;

/* Private function*/
static fsp_err_t cac_measurement_process(void);

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/

void hal_entry(void)
{
    fsp_err_t err                                  = FSP_SUCCESS;
    fsp_pack_version_t version                     = {RESET_VALUE};
    uint16_t cgc_time_out                          = UINT16_MAX;
    /* used to get reference clock from CAC configurable property "Reference clock source".*/
    uint8_t ref_clk                                = RESET_VALUE;
    /* used to get target clock from CAC configurable property "measurement clock source".*/
    uint8_t target_clk                             = RESET_VALUE;
    /* Supported clocks in this EP */
    uint8_t target_cgc_clk_src[TARGET_CGC_CLK_CNT] = {TARGET_CLOCK_SOURCE_MAIN_OSC,
                                                      TARGET_CLOCK_SOURCE_SUBCLOCK,
                                                      TARGET_CLOCK_SOURCE_HOCO,
                                                      TARGET_CLOCK_SOURCE_MOCO,
                                                      TARGET_CLOCK_SOURCE_lOCO,
                                                      TARGET_CLOCK_SOURCE_PCLKB};

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the RTT */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.major, version.minor, version.patch);
    APP_PRINT(EP_INFO);

    /* Open CGC module */
    err = R_CGC_Open(&g_cgc_ctrl, &g_cgc_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_CGC_Open API FAILED ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Open CAC module */
    err = R_CAC_Open(&g_cac_ctrl, &g_cac_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_CAC_Open API FAILED ** \r\n");

        /* de-initialize all the opened modules before trap*/
        deinit_cgc();
        APP_ERR_TRAP(err);
    }

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Open failure message */
        APP_ERR_PRINT ("\r\n** R_ICU_ExternalIrqOpen API FAILED **\r\n");

        /* de-initialize all the opened modules before trap*/
        deinit_cac();
        deinit_cgc();
        APP_ERR_TRAP(err);
    }

    /* copy the reference clock selected from configurator to local buffer*/
    ref_clk = g_cac_cfg.cac_ref_clock.clock;

    /* If reference clock is not IWDT clock, then start and check stability of reference clock selected in configurator.*/
    if (CAC_CLOCK_SOURCE_IWDT != ref_clk)
    {
        /* Start selected target clock. */
        err = R_CGC_ClockStart(&g_cgc_ctrl, target_cgc_clk_src[ref_clk], NULL);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** R_CGC_ClockStart API FAILED ** \r\n");
            /* close all the opened modules before trap*/
            clean_up();
            APP_ERR_TRAP(err);
        }

        /* Check selected target clock for stability */
        do
        {
            /*start checking for timeout to avoid infinite loop*/
            --cgc_time_out;

            err = R_CGC_ClockCheck(&g_cgc_ctrl, target_cgc_clk_src[ref_clk]);

            /*check for time elapse*/
            if ((RESET_VALUE == cgc_time_out) || (FSP_ERR_CLOCK_INACTIVE == err))
            {
                /*we have reached to a scenario where CGC clock is not stabilized*/
                APP_ERR_PRINT (" ** Unable to  stabilize CGC clock **\r\r");
                /* close all the opened modules before trap*/
                clean_up();
                APP_ERR_TRAP(err);
            }
        }while(FSP_ERR_NOT_STABILIZED == err);
        APP_PRINT("\r\nSelected CGC Reference clock started and stabilized.\r\n");
    }

    /* copy the target clock selected from configurator to local buffer*/
    target_clk = g_cac_cfg.cac_meas_clock.clock;

    /* If target clock is not IWDT clock, then start and check stability of target clock selected in configurator.*/
    if (CAC_CLOCK_SOURCE_IWDT != target_clk)
    {
        /* Start selected target clock. */
        err = R_CGC_ClockStart(&g_cgc_ctrl, target_cgc_clk_src[target_clk], NULL);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** R_CGC_ClockStart API FAILED ** \r\n");
            /* close all the opened modules before trap*/
            clean_up();
            APP_ERR_TRAP(err);
        }

        /* Check selected target clock for stability */
        do
        {
            /*start checking for timeout to avoid infinite loop*/
            --cgc_time_out;

            err = R_CGC_ClockCheck(&g_cgc_ctrl, target_cgc_clk_src[target_clk]);

            /*check for time elapse*/
            if ((RESET_VALUE == cgc_time_out) || (FSP_ERR_CLOCK_INACTIVE == err))
            {
                /*we have reached to a scenario where CGC clock is not stabilized*/
                APP_ERR_PRINT (" ** Unable to  stabilize CGC clock **\r\r");
                /* close all the opened modules before trap*/
                clean_up();
                APP_ERR_TRAP(err);
            }
        }while(FSP_ERR_NOT_STABILIZED == err);
        APP_PRINT("\r\nSelected CGC Target clock started and stabilized.\r\n");
    }

    /* Enable ICU module */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Enable failure message */
        APP_ERR_PRINT ("\r\n**R_ICU_ExternalIrqEnable API FAILED**\r\n");
        /* close all the opened modules before trap*/
        clean_up();
        APP_ERR_TRAP(err);
    }

    while (true)
    {
        if(true == b_sw_press)
        {
            /* Clear user pushbutton flag */
            b_sw_press = false;

            APP_PRINT("\r\nStart CAC measurement operation\r\n");
            /* process cac measurement operation.*/
            err = cac_measurement_process();
            if(FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n** CAC MEASUREMENT PROCESS FAILED **\r\n");
                /* close all the opened modules before trap*/
                clean_up();
                APP_ERR_TRAP(err);
            }
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       Perform CAC measurement operation
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS     Upon successful CAC measurement.
 *  @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful measurement .
 **********************************************************************************************************************/
static fsp_err_t cac_measurement_process(void)
{
    fsp_err_t err                     = FSP_SUCCESS;
    uint16_t cac_counter_val          = RESET_VALUE;
    uint16_t cac_time_out             = UINT16_MAX;

    /* Reset flags before starting CAC measurement*/
    g_cac_event = CAC_EVENT_VALUE;
    b_cac_status_flag = false;

    /* Begin a measurement */
    err = R_CAC_StartMeasurement(&g_cac_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_CAC_StartMeasurement API FAILED ** \r\n");
        return err;
    }

    APP_PRINT("\r\nCAC measurement started.\r\n")

    /* Wait for measurement complete or error generation event. */
    while (true != b_cac_status_flag)
    {
        /*start checking for timeout to avoid infinite loop*/
        --cac_time_out;

        /*check for time elapse*/
        if (RESET_VALUE == cac_time_out)
        {
            /*we have reached to a scenario where CAC event not occurred*/
            APP_ERR_PRINT (" ** No event received during CAC measurement operation **\r\r");
            return FSP_ERR_TIMEOUT;
        }
    }

    /* Check for event received from CAC callback. */
    switch (g_cac_event)
    {
        case CAC_EVENT_MEASUREMENT_COMPLETE :
        {
            err = R_CAC_Read(&g_cac_ctrl, &cac_counter_val);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n** R_CAC_Read API FAILED ** \r\n");
                return err;
            }

            APP_PRINT("\r\nCAC measurement completed and selected target clock is accurate.\r\n")
            APP_PRINT("\r\nCAC counter value : %d\r\n",cac_counter_val);

            /* Stop a measurement */
            err = R_CAC_StopMeasurement(&g_cac_ctrl);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\n** R_CAC_StopMeasurement API FAILED ** \r\n");
                return err;
            }
            APP_PRINT("\r\nCAC measurement stopped.\r\n");
        }
        break;

        case CAC_EVENT_FREQUENCY_ERROR :
        {
            APP_ERR_PRINT("\r\n** Frequency error occurred and CAC counter value is not in range of configured upper "
                          "and lower limit.**\r\n");

            /* returning from here as counter value is not in the range of configured upper limit and lower limit.*/
            err = FSP_ERR_INVALID_CAC_REF_CLOCK;
        }
        break;

        default :
        {
            APP_ERR_PRINT("\r\n** CAC counter overflow occurred. **\r\n");

            /* returning from here as counter buffer overflows. */
            err = FSP_ERR_OVERFLOW;
        }
        break;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief      User defined CAC callback.
 * @param[IN]  p_args
 * @retval     None
 **********************************************************************************************************************/
void cac_callback(cac_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        g_cac_event = p_args->event;
        b_cac_status_flag = true;
    }
}

/*******************************************************************************************************************//**
 * @brief      User defined external irq callback.
 * @param[IN]  p_args
 * @retval     None
 **********************************************************************************************************************/
void external_irq_cb(external_irq_callback_args_t *p_args)
{
    /* Make sure it's the right interrupt*/
    if ( (NULL != p_args) && (USER_SW_IRQ_NUMBER == p_args->channel) )
    {
        b_sw_press = true;
    }
}

/*****************************************************************************************************************
 * @brief      Close the CAC HAL driver.
 * @param[IN]  None
 * @retval     None
 ****************************************************************************************************************/
void deinit_cac(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close cac module */
    err = R_CAC_Close(&g_cac_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_CAC_Close API FAILED ** \r\n");
    }
}

/*****************************************************************************************************************
 * @brief      Close the CGC HAL driver.
 * @param[IN]  None
 * @retval     None
 ****************************************************************************************************************/
void deinit_cgc(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close CGC module */
    err = R_CGC_Close(&g_cgc_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** R_CGC_Close API FAILED ** \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened ICU module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void deinit_external_irq(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* close opened external interrupt module */
    err = R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("** R_ICU_ExternalIrqClose API FAILED **\r\n");
    }
}

/*****************************************************************************************************************
 *  @brief      Close all modules.
 *  @param[in] 	None
 *  @retval     None
 ****************************************************************************************************************/
void clean_up(void)
{
    /* de-initialize all the opened modules before trap*/
    deinit_cac();
    deinit_cgc();
    deinit_external_irq();
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, &g_bsp_pin_cfg);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup cac_ep)
 **********************************************************************************************************************/
