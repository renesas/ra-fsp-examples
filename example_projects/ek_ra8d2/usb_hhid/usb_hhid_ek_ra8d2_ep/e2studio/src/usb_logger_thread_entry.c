/***********************************************************************************************************************
 * File Name    : usb_logger_thread_entry.c
 * Description  : Contains data structures and functions used in usb_logger_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usb_logger_thread.h"
#include "usb_hhid_appl.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_hhid_ep
 * @{
 **********************************************************************************************************************/

/* Global variables */
static uint8_t g_test_buf[DATASIZE];
BaseType_t g_err_data_queue;
extern uint16_t g_mxps;

/* USB keyboard keycode data */
uint8_t g_usbhidcode[INDEX][ROW_SIZE][COL_SIZE] =
{
 {
  "No key", "Keyboard Error Roll Over", "Keyboard POST Fail", "Keyboard Error Undefined",
  "Keyboard key a", "Keyboard key b", "Keyboard key c", "Keyboard key d", "Keyboard key e",
  "Keyboard key f", "Keyboard key g", "Keyboard key h", "Keyboard key i", "Keyboard key j",
  "Keyboard key k", "Keyboard key l", "Keyboard key m", "Keyboard key n", "Keyboard key o",
  "Keyboard key p", "Keyboard key q", "Keyboard key r", "Keyboard key s", "Keyboard key t",
  "Keyboard key u", "Keyboard key v", "Keyboard key w", "Keyboard key x", "Keyboard key y",
  "Keyboard key z", "Keyboard key 1", "Keyboard key 2", "Keyboard key 3", "Keyboard key 4",
  "Keyboard key 5", "Keyboard key 6", "Keyboard key 7", "Keyboard key 8", "Keyboard key 9",
  "Keyboard key 0"
 },

 {
  "No key", "Keyboard Error Roll Over", "Keyboard POST Fail", "Keyboard Error Undefined",
  "Keyboard key A", "Keyboard key B", "Keyboard key C", "Keyboard key D", "Keyboard key E",
  "Keyboard key F", "Keyboard key G", "Keyboard key H", "Keyboard key I", "Keyboard key J",
  "Keyboard key K", "Keyboard key L", "Keyboard key M", "Keyboard key N", "Keyboard key O",
  "Keyboard key P", "Keyboard key Q", "Keyboard key R", "Keyboard key S", "Keyboard key T",
  "Keyboard key U", "Keyboard key V", "Keyboard key W", "Keyboard key X", "Keyboard key Y",
  "Keyboard key Z", "Keyboard key !", "Keyboard key @", "Keyboard key #", "Keyboard key $",
  "Keyboard key %", "Keyboard key ^", "Keyboard key &", "Keyboard key *", "Keyboard key (",
  "Keyboard key )"
 }
};

/* Logger Thread entry function */
/* pvParameters contains TaskHandle_t */
void usb_logger_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    while (true)
    {
        char temBuff[BUFF_SIZE] = {NULL_CHAR};
        uint8_t index = RESET_VALUE;
        /* Receive data from queue */
        g_err_data_queue = xQueueReceive(g_data_queue, &g_test_buf[RESET_VALUE], portMAX_DELAY);
        if (pdTRUE == g_err_data_queue)
        {
            if ((KEY_MOD_LSHIFT == g_test_buf[RESET_VALUE]) || (KEY_MOD_RSHIFT == g_test_buf[RESET_VALUE] ))
            {
                index = ONE;
            }
            else
            {
                index = RESET_VALUE;
            }

            /* Decode the received keycode to recognize key pressed on the keyboard */
            for (uint8_t cnt = ONE; cnt < g_mxps; cnt++)
            {
                if (RESET_VALUE != g_test_buf[cnt])
                {
                    if (ROW_SIZE > g_test_buf[cnt])
                    {

                        memcpy(temBuff, &g_usbhidcode[index][g_test_buf[cnt]][RESET_VALUE], BUFF_SIZE);
                        APP_PRINT("%s is pressed\n", temBuff);
                    }
                    else
                    {
                        APP_PRINT("Pressed key is out of specified key range.\r\n");
                    }
                }
            }
        }
        else
        {
            APP_ERR_PRINT("\r\nError in receiving data through queue\r\n");
            /* Close the opened USB driver and trap error */
            deinit_usb();
            APP_ERR_TRAP(g_err_data_queue);
        }
        vTaskDelay(TASK_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usb_hhid_ep)
 **********************************************************************************************************************/
