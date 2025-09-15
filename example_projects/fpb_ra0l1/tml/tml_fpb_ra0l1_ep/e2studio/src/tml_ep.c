/***********************************************************************************************************************
 * File Name    : tml_ep.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "tml_ep.h"

extern bsp_leds_t g_bsp_leds;

/* Timer callback flags */
volatile bool g_timer_flag [APP_TML_MAX_MODE] =
{
    [APP_TML_8_BIT]          = false,
    [APP_TML_16_BIT]         = false,
    [APP_TML_32_BIT]         = false,
    [APP_TML_16_BIT_CAPTURE] = false,
};

/* TML instances */
static timer_instance_t const * g_tml_instance [APP_TML_MAX_MODE] =
{
    [APP_TML_8_BIT]              = &g_timer_8bit,
    [APP_TML_16_BIT]             = &g_timer_16bit,
    [APP_TML_32_BIT]             = &g_timer_32bit,
    [APP_TML_16_BIT_CAPTURE]     = &g_timer_16bit_capture,
    [APP_TML_32_BIT_COUNTER_LPM] = &g_timer_32bit_counter_lpm,
};

/* TML instances control */
static tml_instance_ctrl_t * g_tml_instance_ctrl [APP_TML_MAX_MODE] =
{
    [APP_TML_8_BIT]              = &g_timer_8bit_ctrl,
    [APP_TML_16_BIT]             = &g_timer_16bit_ctrl,
    [APP_TML_32_BIT]             = &g_timer_32bit_ctrl,
    [APP_TML_16_BIT_CAPTURE]     = &g_timer_16bit_capture_ctrl,
    [APP_TML_32_BIT_COUNTER_LPM] = &g_timer_32bit_counter_lpm_ctrl,
};

/* TML modules name */
const uint8_t * g_tml_mode_name [APP_TML_MAX_MODE] =
{
    [APP_TML_8_BIT]          = (uint8_t *) NAME_8BIT_MODE,
    [APP_TML_16_BIT]         = (uint8_t *) NAME_16BIT_MODE,
    [APP_TML_32_BIT]         = (uint8_t *) NAME_32BIT_MODE,
    [APP_TML_16_BIT_CAPTURE] = (uint8_t *) NAME_16BIT_CAPTURE_MODE,
};

/* Function declarations */
static void tml_deinit(void);
static void elc_deinit(void);
static void lpm_deinit(void);
static void handle_error(fsp_err_t err, uint8_t * err_str);

/*******************************************************************************************************************//**
 * @brief      This function initializes the TML modules and starts TML operation.
 * @param[in]  None
 * @retval     None
 **********************************************************************************************************************/
void tml_entry(void)
{
    fsp_err_t          err     = FSP_SUCCESS;
    fsp_pack_version_t version = { RESET_VALUE };

    uint16_t duration_time = RESET_VALUE;
    uint8_t  user_input    = RESET_VALUE;

    /* Turn OFF all LEDs on board at start up */
    for (int i =0; i < g_bsp_leds.led_count; i++)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[i], (bsp_io_level_t)BSP_IO_LEVEL_LOW);
    }

#if (USE_VIRTUAL_COM == 1)
    /* Initialize UARTA module first to print log to serial terminal */
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Turn on all LEDs on board if uart_init fails */
        for (int i =0; i < g_bsp_leds.led_count; i++)
        {
            R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[i],\
                              (bsp_io_level_t) BSP_IO_LEVEL_HIGH);
        }

        /* Error trap */
        __asm("BKPT #0\n");
    }
#endif /* USE_VIRTUAL_COM */

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch );
    APP_PRINT(EP_INFO);

    while (true)
    {
        APP_PRINT(PRINT_MENU);
        user_input = get_user_input();
        switch (user_input)
        {
            case TML_COUNTER:
            {
                APP_PRINT("\r\nDemonstrate TML counter mode\r\n");
                /* Get duration time for each TML counter mode */
                duration_time = tml_get_count_duration();
                
                /* The TML operates in 8-bit counter mode */
                err = tml_counter_operation(duration_time, APP_TML_8_BIT);
                APP_ERR_HANDLE(err, "\r\n**tml_8bit_operation failed**\r\n");
                
                /* The TML operates in 16-bit counter mode */
                err = tml_counter_operation(duration_time, APP_TML_16_BIT);
                APP_ERR_HANDLE(err, "\r\n**tml_16bit_operation failed**\r\n");
                
                /* The TML operates in 32-bit counter mode */
                err = tml_counter_operation(duration_time, APP_TML_32_BIT);
                APP_ERR_HANDLE(err, "\r\n**tml_32bit_operation failed**\r\n");
            }
            break;
            case TML_CAPTURE:
            {
                APP_PRINT("\r\nDemonstrate TML capture mode\r\n");
                /* The TML operates in 16-bit capture mode */
                err = tml_capture_operation();
                APP_ERR_HANDLE(err, "\r\n**tml_16bit_capture_operation failed**\r\n");
            }
            break;
            case TML_COUNTER_IN_LPM:
            {
                APP_PRINT("\r\nDemonstrate TML counter mode operates in LPM (Sleep and Software Standby modes)\r\n");
                
                /* Initialize TML 32-bit counter operates in LPM modes */
                err = tml_open(APP_TML_32_BIT_COUNTER_LPM);
                APP_ERR_HANDLE(err, "\r\n**tml_open for 32-bit counter operates in LPM failed**\r\n");
                
                /* The 32-bit counter TML operates in LPM Sleep mode */
                err = tml_counter_operation_in_lpm(APP_LPM_SLEEP_STATE);
                APP_ERR_HANDLE(err, "\r\n**tml_counter_operates_in_LPM_Sleep_mode failed**\r\n");
                
                /* The 32-bit counter TML operates in LPM SW Standby mode */
                err = tml_counter_operation_in_lpm(APP_LPM_SW_STANDBY_STATE);
                APP_ERR_HANDLE(err, "\r\n**tml_counter_operates_in_LPM_SW_Standby_mode failed**\r\n");
                
                /* De-initialize TML 32-bit counter operates in LPM modes */
                err = tml_close(APP_TML_32_BIT_COUNTER_LPM);
                APP_ERR_HANDLE(err, "\r\n**tml_close for 32-bit counter operates in LPM failed**\r\n");
            }
            break;
            default:
            APP_PRINT("\r\nInvalid Input.\r\n");
        }
    }
}

/*********************************************************************************************************************
 * @brief       This function is used to get the user inputs.
 * @param[in]   None
 * @retval      input_value    Returns input buffer after receive enter key.
 *********************************************************************************************************************/
uint8_t get_user_input(void)
 {
	uint8_t input_value = RESET_VALUE;
	char user_input[TERM_BUFFER_SIZE + 1] = {RESET_VALUE};

    /* Clean buffer */
    memset(&user_input[0], NULL_CHAR, sizeof(user_input));

    /* Wait until there is any user input */
    while (!APP_CHECK_DATA)
    {
        __NOP();
    }

    /* Read user input from the terminal */
    APP_READ(&user_input[0], TERM_BUFFER_SIZE);

    /* Convert to integer value */
    input_value = (uint8_t)atoi((char*) &user_input[0]);

    return input_value;
}

/*******************************************************************************************************************//**
 * @brief       This function opens TML modules.
 * @param[IN]   tml_mode 		Timer mode to open.
 * @retval      FSP_SUCCESS 	Timer opened successfully.
 * @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tml_open(app_tml_mode_t tml_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open timer */
    err = R_TML_Open(g_tml_instance[tml_mode]->p_ctrl, g_tml_instance[tml_mode]->p_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_TML_Open API failed**\r\n");

#if (!TML_CFG_PARAM_CHECKING_ENABLE)
    g_tml_instance_ctrl[tml_mode]->open = TML_OPEN;
#endif /* !TML_CFG_PARAM_CHECKING_ENABLE */

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function closes TML modules.
 * @param[IN]   tml_mode 		Timer mode to close.
 * @retval      FSP_SUCCESS 	Timer closed successfully.
 * @retval       Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t tml_close(app_tml_mode_t tml_mode)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close timer */
    if (MODULE_CLOSED != g_tml_instance_ctrl[tml_mode]->open)
    {
        err = R_TML_Close(g_tml_instance[tml_mode]->p_ctrl);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_TML_Close API failed**\r\n");

#if (!TML_CFG_PARAM_CHECKING_ENABLE)
        g_tml_instance_ctrl[tml_mode]->open = MODULE_CLOSED;
#endif /* !TML_CFG_PARAM_CHECKING_ENABLE */
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function starts TML modules.
 * @param[IN]   tml_mode 		Timer mode to start.
 * @retval      FSP_SUCCESS 	Timer started successfully.
 * @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tml_start(app_tml_mode_t tml_mode)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Start timer */
    err = R_TML_Start(g_tml_instance[tml_mode]->p_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_TML_Start API failed**\r\n");

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function stops TML modules.
 * @param[IN]   tml_mode 		Timer mode to stop.
 * @retval      FSP_SUCCESS 	Timer stopped successfully.
 * @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tml_stop(app_tml_mode_t tml_mode)
{
    fsp_err_t err = FSP_SUCCESS;
    timer_status_t timer_status;

    /* Get status of count timer */
    err= R_TML_StatusGet(g_tml_instance[tml_mode]->p_ctrl, &timer_status);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_TML_StatusGet API failed**\r\n");

    if(TIMER_STATE_STOPPED != timer_status.state)
    {
        /* Stop timer */
        err = R_TML_Stop(g_tml_instance[tml_mode]->p_ctrl);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_TML_Stop API failed**\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened TML modules before the project ends up in an error trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void tml_deinit(void)
{
    if (FSP_SUCCESS != tml_close(APP_TML_8_BIT))
{
        APP_ERR_PRINT("tml_close for the 8-bit failed\r\n");
    }

    if (FSP_SUCCESS != tml_close(APP_TML_16_BIT))
    {
        APP_ERR_PRINT("tml_close for the 16-bit failed\r\n");
    }

    if (FSP_SUCCESS != tml_close(APP_TML_32_BIT))
    {
        APP_ERR_PRINT("tml_close for the 32-bit failed\r\n");
    }

    if (FSP_SUCCESS != tml_close(APP_TML_16_BIT_CAPTURE))
    {
        APP_ERR_PRINT("tml_close for the 16-bit capture failed\r\n");
    }

    if (FSP_SUCCESS != tml_close(APP_TML_32_BIT_COUNTER_LPM))
    {
        APP_ERR_PRINT("tml_close for the LPM 32-bit timer failed\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened ELC module before the project ends up in an error trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void elc_deinit(void)
{
    /* De-initialize ELC module if it opened */
    if (MODULE_CLOSE != g_elc_ctrl.open)
    {
        if(FSP_SUCCESS != R_ELC_Close(&g_elc_ctrl))
        {
            APP_ERR_PRINT("\r\n**R_ELC_Close API failed**\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function closes opened LPM modules before the project ends up in an error trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void lpm_deinit(void)
{
    /* De-initialize LPM Sleep mode module if it opened */
    if (MODULE_CLOSE != g_lpm_sleep_ctrl.lpm_open)
    {
        if(FSP_SUCCESS != R_LPM_Close(&g_lpm_sleep_ctrl))
        {
            APP_ERR_PRINT("\r\n**Close LPM Sleep mode failed**\r\n");
        }
    }

    /* De-initialize LPM SW Standby mode module if it opened */
    if (MODULE_CLOSE != g_lpm_sw_standby_ctrl.lpm_open)
    {
        if(FSP_SUCCESS != R_LPM_Close(&g_lpm_sw_standby_ctrl))
        {
            APP_ERR_PRINT("\r\n**Close LPM SW Standby mode failed**\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function handles error if error occurred, closes all opened modules, prints and traps error.
 *  @param[in]   err       Error status.
 *  @param[in]   err_str   Error string.
 *  @retval      None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, uint8_t * err_str)
{
    /* Print error information and error code */
    APP_ERR_PRINT(err_str);
    /* De-initialize all opened TML modules */
    tml_deinit();
    /* De-initialize opened ELC module */
    elc_deinit();
    /* De-initialize all opened LPM modules */
    lpm_deinit();
    /* Trap the error */
    APP_ERR_TRAP(err);
}

/*******************************************************************************************************************//**
 * @} (end addtogroup tml_ep)
 **********************************************************************************************************************/
