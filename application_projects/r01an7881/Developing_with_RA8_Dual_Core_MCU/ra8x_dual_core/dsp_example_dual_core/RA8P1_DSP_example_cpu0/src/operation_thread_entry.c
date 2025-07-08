/***********************************************************************************************************************
 * File Name    : operation_thread_entry.c
 * Description  : This file contains the main operation of application by call arm fft example.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ipc_squeue.h"
#include "operation_thread.h"
#include "common_utils.h"

/* Extern variables */
extern uint8_t g_rq_fft_calc;
extern float32_t testInput_f32_10khz[2048];

float32_t testInput_f32_10khz_temp[2048] = {RESET_VALUE};

int32_t main_arm_fft_bin_example_f32(void);

/*******************************************************************************************************************//**
 * @brief     Sends formatted data to the print/output interface on CPU1 (e.g., UART terminal).
 *
 * @param[in] format  Format string (similar to printf).
 * @param[in] ...     Variable arguments corresponding to the format specifiers in the format string.
 *
 * @return    fsp_err_t  Status of the print operation (e.g., success or error code).
 **********************************************************************************************************************/
fsp_err_t send_to_print(char *format, ...)
{
    fsp_err_t   err  = FSP_SUCCESS;
    size_t    len    = RESET_VALUE;
    int     ret      = RESET_VALUE;
    int32_t transmitted_bytes = RESET_VALUE;
    unsigned char g_tx_buffer[MAX_MESSAGE_LENGTH] = {RESET_VALUE};
    va_list aptr;

    va_start(aptr, format);
    ret = vsprintf((char *)g_tx_buffer, format, aptr);
    va_end(aptr);

    /* Check VSPRINTF function return code */
    if (RESET_VALUE > ret)
    {
        return (fsp_err_t)FSP_ERR_INVALID_ARGUMENT;
    }

    /* Calculate length of message */
    len = strlen((char *)g_tx_buffer);
    if (RESET_VALUE == len)
    {
        return (fsp_err_t)FSP_ERR_INVALID_SIZE;
    }

    /* Write message to share memory and trigger second core to print out terminal */
    transmitted_bytes = send_to_queue(g_tx_buffer, len, 0);
    if (0 == transmitted_bytes )
    {
        APP_ERR_TRAP(err);
    }
    return err;
}

/* Operation Thread entry function */
/* pvParameters contains TaskHandle_t */
void operation_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* Copy original input data to temporary buffer for repeated runs */
    memcpy(testInput_f32_10khz_temp, testInput_f32_10khz, sizeof(testInput_f32_10khz));

    while (1)
    {
        if(g_rq_fft_calc)
        {
           /* Load testInput_f32_10khz */
           memcpy(testInput_f32_10khz, testInput_f32_10khz_temp, sizeof(testInput_f32_10khz));
           /* Execute arm_fft_bin_example_f32 example */
           main_arm_fft_bin_example_f32();
           g_rq_fft_calc = RESET_FLAG;
           APP_PRINT("\r\nThe DSP application has finished execution. \r\nPress s1 button to start application ...\r\n");
        }
        vTaskDelay (1);
    }
}
