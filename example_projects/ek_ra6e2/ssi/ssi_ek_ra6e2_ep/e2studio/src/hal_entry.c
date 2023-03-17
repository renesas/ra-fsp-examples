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
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT("\r\nThe project demonstrates SSI module by transmitting and receiving \r\n"
            "the sample audio data in loop back connection and prints the status  \r\n"
            "by comparing the transmitted and received data buffers \r\n");

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

    /*Wait for completion of WriteRead operation using I2S_EVENT_IDLE event and time_out. Using these both ensures
     *that the DTC transfer will be over by the time transmit underflow occurs during R_SSI_WriteRead processing.
     *This is important so the receive buffer can be flushed in the transmit underflow error processing.
     *Without this, the last frame (two samples) could be lost during R_SSI_WriteRead. */
    while ((I2S_EVENT_IDLE != g_i2s_event) || ((time_out--) > 0));

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

    while(true)
    {
        ;
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
