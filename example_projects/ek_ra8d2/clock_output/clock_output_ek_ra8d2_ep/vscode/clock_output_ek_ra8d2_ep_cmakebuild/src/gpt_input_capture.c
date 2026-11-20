/***********************************************************************************************************************
 * File Name    : gpt_input_capture.c
 * Description  : Contains data structures and functions used in gpt_input_capture.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#if (GPT_MEASURE_CLKOUT)

#include "clock_output_example_app.h"
#include "gpt_input_capture.h"

/* Global variable */
uint32_t g_clkout_value = RESET_VALUE;

/* Private variables */
static volatile bool g_start_measurement = false;
static uint32_t g_capture_count = RESET_VALUE;
static uint32_t g_frequency = RESET_VALUE;

/* Private function declarations */
static fsp_err_t gpt_init(void);
static fsp_err_t gpt_info_get(timer_info_t * const p_info);
static fsp_err_t gpt_enable(void);
static fsp_err_t gpt_disable(void);

/***********************************************************************************************************************
 * Function Name: gpt_capture_operation.
 * Description  : This function performs GPT capture operation.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Operated successfully.
 *                Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t gpt_capture_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    timer_info_t info =
    {
        .clock_frequency = RESET_VALUE,
        .count_direction = RESET_VALUE,
        .period_counts = RESET_VALUE
    };

    g_clkout_value = RESET_VALUE;

    /* Open GPT module */
    err = gpt_init();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get the GPT information */
    err = gpt_info_get(&info);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Get the GPT frequency */
    g_frequency = info.clock_frequency;

    /* Enable GPT input capture */
    err = gpt_enable();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait for GPT capture event */
    while (!g_start_measurement)
    {
        /* Do nothing */
    }

    /* Reset the flag */
    g_start_measurement = false;

    /* Calculate the clock output value */
    g_clkout_value = ((g_frequency) / (g_capture_count));

    /* Reset the GPT capture count value */
    g_capture_count = RESET_VALUE;

    return err;
}
/***********************************************************************************************************************
* End of function gpt_capture_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: gpt_init.
 * Description  : This function initializes the GPT module.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static fsp_err_t gpt_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Open(&g_input_capture_ctrl, &g_input_capture_cfg);

    return err;
}
/***********************************************************************************************************************
* End of function gpt_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: gpt_info_get.
 * Description  : This function gets GPT timer information.
 * Arguments    : p_info                    Pointer to the timer information structure.
 * Return Value : FSP_SUCCESS               Got successfully.
 *                Any other error codes     Failed to get.
 **********************************************************************************************************************/
static fsp_err_t gpt_info_get(timer_info_t * const p_info)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_InfoGet(&g_input_capture_ctrl, p_info);

    return err;
}
/***********************************************************************************************************************
* End of function gpt_info_get
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: gpt_enable.
 * Description  : This function enables external event triggers that start, stop, clear, or capture the counter.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Enabled successfully.
 *                Any other error codes     Failed to enable.
 **********************************************************************************************************************/
static fsp_err_t gpt_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Enable(&g_input_capture_ctrl);

    return err;
}
/***********************************************************************************************************************
* End of function gpt_enable
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: gpt_disable.
 * Description  : This function disables external event triggers that start, stop, clear, or capture the counter.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Disabled successfully.
 *                Any other error codes     Failed to disable.
 **********************************************************************************************************************/
static fsp_err_t gpt_disable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Disable(&g_input_capture_ctrl);

    return err;
}
/***********************************************************************************************************************
* End of function gpt_disable
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: input_capture_user_callback.
 * Description  : User defined callback.
 * Arguments    : p_args                    Callback function parameter data.
 * Return Value : None.
 **********************************************************************************************************************/
void input_capture_user_callback(timer_callback_args_t *p_args)
{
    /* Check for the event */
    switch (p_args->event)
    {
        case TIMER_EVENT_CAPTURE_A:
        {
            /* Get GPT capture count value and store in g_capture_count */
            g_capture_count = p_args->capture;

            /* Check GPT capture count value to avoid noise signal */
            if (0 != g_capture_count)
            {
                /* Set start measurement flag */
                g_start_measurement = true;

                /* Disable GPT input capture */
                gpt_disable();

                /* De-initialize GPT input capture */
                gpt_deinit();
            }
            break;
        }
        default:
        {
            break;
        }
    }
}
/***********************************************************************************************************************
* End of function input_capture_user_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: gpt_deinit.
 * Description  : This function de-initializes the opened GPT module.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void gpt_deinit(void)
{
    /* De-initialize the opened GPT module */
    if (g_input_capture_ctrl.open)
    {
        R_GPT_Close(&g_input_capture_ctrl);
    }
}
/***********************************************************************************************************************
* End of function gpt_deinit
***********************************************************************************************************************/

#endif /* GPT_MEASURE_CLKOUT */
