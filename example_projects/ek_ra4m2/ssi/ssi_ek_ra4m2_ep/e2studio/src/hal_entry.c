/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "common_utils.h"
#include "audio_data.h"

#define MAX_TIME    0xffff      //Max time value for time out

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* Private functions */
static void deinit_gpt(void);
static void deinit_ssi(void);

/*******************************************************************************************************************//**
 * @addtogroup SSI_EP
 * @{
 **********************************************************************************************************************/

/* Global variables */
volatile i2s_event_t g_i2s_event = I2S_EVENT_TX_EMPTY;  //an actual event updates in callback

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    volatile uint32_t time_out = MAX_TIME;                 // time_out value which is used to break the infinite loop

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch );
    APP_PRINT("\r\nThe example project illustrates a typical use of the SSI HAL module API. The project \r\n"
            "continuously transmits and receives audio sample data from the source buffer to the \r\n"
            "destination buffer by looping back connecting the Tx and Rx pins and comparing the \r\n"
            "transmitted data of the two buffers at 500ms each time apart. The comparison results \r\n"
            "will be displayed on the RTT viewer \r\n");

    /* Open SSI module */
    err = R_SSI_Open(&g_i2s_ctrl, &g_i2s_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n SSI open failed \r\n");
        /* Trap here */
        APP_ERR_TRAP(err);
    }

    /* Open GPT in periodic mode as internal clock for SSI bit clock */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n GPT open failed, Closing SSI \r\n");
        deinit_ssi();
        /* Trap here */
        APP_ERR_TRAP(err);
    }

    /* Start GPT in periodic mode */
    err = R_GPT_Start(&g_timer_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n GPT start failed, Closing SSI and GPT \r\n");
        deinit_ssi();
        deinit_gpt();
        /* Trap here */
        APP_ERR_TRAP(err);
    }

    while(true)
    {
        /* Transmit and receive the sample audio data buffer using WriteRead API from source buffer to destination buffer
         * through loop back connection from SSITXD to SSIRXD */
        err = R_SSI_WriteRead(&g_i2s_ctrl, g_src_buff, g_dest_buff, BUFF_SIZE);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n WriteRead API failed, Closing SSI and GPT \r\n");
            deinit_ssi();
            deinit_gpt();
            /* Trap here */
            APP_ERR_TRAP(err);
        }

        /*Wait for all transfer was complete of WriteRead operation using I2S_EVENT_RX_FULL event.*/
        while (I2S_EVENT_RX_FULL != g_i2s_event)
        {
            __NOP();
        }

        /*Set the timeout.*/
        time_out = MAX_TIME;

        /*Wait for completion of WriteRead operation using I2S_EVENT_IDLE event and time_out. Using these both ensures
         *that the DTC transfer will be over by the time transmit underflow occurs during R_SSI_WriteRead processing.
         *This is important so the receive buffer can be flushed in the transmit underflow error processing.
         *Without this, the last frame (two samples) could be lost during R_SSI_WriteRead. */
        while ((I2S_EVENT_IDLE != g_i2s_event) && --time_out)
        {
            __NOP();
        }

        /*Check if a timeout occurred.*/
        if (RESET_VALUE == time_out)
        {
            APP_PRINT("\r\nWriteRead operation not complete due to timeout");
            deinit_ssi();
            deinit_gpt();
            /* Trap here */
            APP_ERR_TRAP(FSP_ERR_TIMEOUT);
        }

        /* Compare the transmission of sample audio data from source buffer to destination buffer with WriteRead API */
        int cmp_result = memcmp(g_src_buff, g_dest_buff, sizeof(g_src_buff));
        if (RESET_VALUE == cmp_result)
        {
            APP_PRINT("\r\nCompared the transmitted sample audio data of SSI with received data is successful");
        }
        else
        {
            APP_PRINT("\r\nCompared the transmitted sample audio data of SSI with received data is failed");
        }
        /* Delay the WriteRead and compare operation for 500ms */
        R_BSP_SoftwareDelay(OPERATION_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    }
}

/*******************************************************************************************************************//**
 * @brief       User defined SSI callback
 * @param[in]   p_args
 * @return      None
 **********************************************************************************************************************/
void i2s_callback(i2s_callback_args_t *p_args)
{
    if( NULL != p_args)
    {
        /* capture callback event for validating the i2s transfer event*/
        g_i2s_event = p_args->event;
    }

}

/*******************************************************************************************************************//**
 * @brief       Close SSI module
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void deinit_ssi(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close SSI Module */
    err = R_SSI_Close(&g_i2s_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n R_SSI_Close API Failed \r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief       Close GPT module
 * @param[in]   None
 * @return      None
 **********************************************************************************************************************/
static void deinit_gpt(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close GPT module */
    err = R_GPT_Close(&g_timer_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n R_SSI_Close API Failed \r\n");
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
 * @} (end defgroup SSI_EP)
 **********************************************************************************************************************/
