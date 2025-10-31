/***********************************************************************************************************************
 * File Name    : gpt_input_capture.c
 * Description  : Contains data structures and functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#if (GPT_MEASURE_CLKOUT == 1)

#include "common_utils.h"
#include "clock_output_ep.h"

/* Private Variables */
static volatile bool g_start_measurement   = false;

static uint32_t    g_capture_count = RESET_VALUE;
static uint32_t    g_frequency     = RESET_VALUE;

/* Private function declarations */
static void gpt_deinit (void);

/***********************************************************************************************************************
 *  Function Name: gpt_capture_operation
 *  Description  : This function performs gpt capture operation.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS  gpt capture operattion runs successfully
 *                 Any Other    Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t gpt_capture_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;

    uint32_t clkout_value = RESET_VALUE;

    timer_info_t info =
    { .clock_frequency = RESET_VALUE, .count_direction = RESET_VALUE, .period_counts = RESET_VALUE };

    /* Open GPT instance as input capture */
    err = R_GPT_Open (&g_input_capture_ctrl, &g_input_capture_cfg);
    APP_ERR_RET (FSP_SUCCESS != err,err, "\r\n ** R_GPT_Open API failed ** \r\n");

    /* Get the GPT information */
    err = R_GPT_InfoGet (&g_input_capture_ctrl, &info);
    APP_ERR_RET (FSP_SUCCESS != err,err, "\r\n ** R_GPT_InfoGet API failed ** \r\n");

    /* Get the GPT frequency */
    g_frequency = info.clock_frequency;

    /* Enable GPT input capture */
    err = R_GPT_Enable (&g_input_capture_ctrl);
    APP_ERR_RET (FSP_SUCCESS != err,err, "\r\n ** R_GPT_Enable API failed ** \r\n");

    /* Wait for GPT capture event */
    while (!g_start_measurement)
    {
        __NOP();
    }

    /* Reset the flag */
    g_start_measurement = false;

    /* Calculate the clock output value */
    clkout_value = ((g_frequency) / (g_capture_count));

    /* Print clock output value */
    APP_PRINT ("\r\nCLKOUT frequency measurement value is %d Hz\r\n", clkout_value);

    /* Reset the GPT capture count value */
    g_capture_count = RESET_VALUE;

    return err;
}
/***********************************************************************************************************************
* End of function gpt_capture_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: input_capture_user_callback
 * Description  : User defined callback
 * Arguments    : p_args
 * Return Value : None
 **********************************************************************************************************************/
void input_capture_user_callback(timer_callback_args_t *p_args)
{
    fsp_err_t err = FSP_SUCCESS;

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
                err = R_GPT_Disable (&g_input_capture_ctrl);
                APP_ERR_HANDLE (err, "\r\n ** R_GPT_Disable API failed ** \r\n");

                /* De-initailize GPT input capture */
                gpt_deinit ();
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
 * Function Name: gpt_deinit
 * Description  : De-initialize GPT module
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
static void gpt_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* De-initialize GPT module if already open */
    if (MODULE_CLOSE != g_input_capture_ctrl.open)
    {
        err = R_GPT_Close (&g_input_capture_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\n ** R_GPT_Close API failed ** \r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function gpt_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error
 * Description  : This function handles error if any error occurred, closes opened GPT module and traps the error.
 * Arguments    : err       error status
 *                err_str   error string
 * Return Value : None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, uint8_t * err_str)
{
    /* Print error information */
    APP_ERR_PRINT (err_str);

    /* De-initialize GPT module */
    gpt_deinit();

    /* Trap the error */
    APP_ERR_TRAP (err);
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
#endif /* GPT_MEASURE_CLKOUT */
