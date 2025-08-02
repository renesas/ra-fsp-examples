/***********************************************************************************************************************
 * File Name    : ethos_u55.c
 * Description  : Contains data structures and functions for Ethos-U55 face detection application.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "ethos_u55.h"
#include "common_utils.h"

extern void MainLoop(void);
#if (INFERENCE_TIME_MEASUREMENT_ENABLED == 1)
static fsp_err_t timer_start (void);
static fsp_err_t timer_init(void);
static fsp_err_t timer_deinit(void);
#endif /* INFERENCE_TIME_MEASUREMENT_ENABLED */
static void handle_error(fsp_err_t err, char *err_str);

/***********************************************************************************************************************
 * @brief       Entry point for face detection application. Initialize hardware accelerator, starts timer measurement.
 * @param       None
 * @retval      None
 **********************************************************************************************************************/
void face_detection_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };

#if (USE_VIRTUAL_COM == 1)
    err = TERM_INIT;
    if (FSP_SUCCESS != err)
    {
        ERROR_TRAP;
    }
#endif /* USE_VIRTUAL_COM */

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet (&version);

    /* Print the EP banner on the RTT viewer */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);

    APP_PRINT(EP_INFO);

#ifdef ARM_NPU
    /* Initialize Ethos-U NPU */
    err = RM_ETHOSU_Open (&g_rm_ethosu0_ctrl, &g_rm_ethosu0_cfg);
    handle_error (err, "ERROR: RM_ETHOSU_Open() failed.\r\n");
#endif /* ARM_NPU */

#if (INFERENCE_TIME_MEASUREMENT_ENABLED == 1)

    /* Initialize the timer */
    err = timer_init ();
    handle_error (err, "ERROR: timer_init() failed.\r\n");

    /* Start the timer */
    err = timer_start ();
    handle_error (err, "ERROR: timer_start() failed.\r\n");

    /* Deinitialize the timer */

#endif /* INFERENCE_TIME_MEASUREMENT_ENABLED */

    /* Enter main face detection operation */
    MainLoop ();

#ifdef ARM_NPU
    /* Deinitialize the Ethos-U NPU */
    err = RM_ETHOSU_Close(&g_rm_ethosu0_ctrl);
    handle_error(err, "ERROR: RM_ETHOSU_Close() failed.\r\n");
#endif /* ARM_NPU */

#if (INFERENCE_TIME_MEASUREMENT_ENABLED == 1)

    /* De-initialize the timer */
    err = timer_deinit ();
    handle_error (err, "ERROR: timer_deinit() failed.\r\n");

#endif /* INFERENCE_TIME_MEASUREMENT_ENABLED */

#ifdef ARM_NPU
    /* Deinitialize the Ethos-U NPU */
    err = RM_ETHOSU_Close (&g_rm_ethosu0_ctrl);
    handle_error (err, "ERROR: RM_ETHOSU_Close() failed.\r\n");
#endif /* ARM_NPU */

}

#if (INFERENCE_TIME_MEASUREMENT_ENABLED == 1)
/***********************************************************************************************************************
 * @brief       This function starts GPT module.
 * @param       None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_start (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Clear timer counter */
    err = R_GPT_Reset (&g_timer_ctrl);
    APP_ERR_RET(err,err, "ERROR: R_GPT_Reset() failed.\r\n");

    /* Start timer */
    err = R_GPT_Start (&g_timer_ctrl);
    APP_ERR_RET(err,err, "ERROR: R_GPT_Start() failed.\r\n");

    return err;
}

/***********************************************************************************************************************
 * @brief       This function gets the elapsed time by reading the timer counter.
 * @param[out]  p_time     Pointer to store the measured time in microseconds
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
fsp_err_t timer_get_measure(uint32_t *p_time)
{
    fsp_err_t       err = FSP_SUCCESS;
    timer_status_t  timer_status = {0};
    timer_info_t    timer_info = {0};

    /* Get status of timer */
    err = R_GPT_StatusGet(&g_timer_ctrl, &timer_status);
    APP_ERR_RET(err, err, "ERROR: R_GPT_StatusGet() failed.\r\n");

    /* Get info of timer */
    err = R_GPT_InfoGet(&g_timer_ctrl, &timer_info);
    APP_ERR_RET(err, err, "ERROR: R_GPT_InfoGet() failed.\r\n");

    /* Convert count value to microseconds */
    *p_time = (uint32_t)(((uint64_t)timer_status.counter * 1000000U) / timer_info.clock_frequency);

    return err;
}

/***********************************************************************************************************************
 * @brief       This function initializes the GPT module.
 * @param       None
 * @retval      FSP_SUCCESS                Upon successful initialization
 * @retval      Any other error code       Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open the GPT timer module */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    APP_ERR_RET(err, err, "ERROR: R_GPT_Open() failed.\r\n");

    return err;
}

/***********************************************************************************************************************
 * @brief       This function deinitializes the GPT module.
 * @param       None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t timer_deinit (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Stop the GPT timer */
    err = R_GPT_Stop(&g_timer_ctrl);
    APP_ERR_RET(err, err, "ERROR: R_GPT_Stop() failed.\r\n");

    /* Close the GPT timer module */
    err = R_GPT_Close(&g_timer_ctrl);
    APP_ERR_RET(err, err, "ERROR: R_GPT_Close() failed.\r\n");

    return err;
}

#endif /* INFERENCE_TIME_MEASUREMENT_ENABLED */

/***********************************************************************************************************************
 *  @brief       This function handles errors by printing the error message, closing opened modules, and 
 *               trapping the error.
 *  @param[in]   err      Error status
 *  @param[in]   err_str  Error string
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

#ifdef ARM_NPU
        /* Close opened ETHOSU module if open */
        if (0U != g_rm_ethosu0_ctrl.open)
        {
            if (FSP_SUCCESS != RM_ETHOSU_Close(&g_rm_ethosu0_ctrl))
            {
                APP_ERR_PRINT("ERROR: RM_ETHOSU_Close() failed.\r\n");
            }
        }
#endif

#if (INFERENCE_TIME_MEASUREMENT_ENABLED == 1)
        /* Close opened GPT module if open */
        if (0U != g_timer_ctrl.open)
        {
            if (FSP_SUCCESS != R_GPT_Close(&g_timer_ctrl))
            {
                APP_ERR_PRINT("ERROR: R_GPT_Close() failed.\r\n");
            }
        }
#endif

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
