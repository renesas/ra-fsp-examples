/***********************************************************************************************************************
 * File Name    : terminal.c
 * Description  : Contains data structures and functions used in terminal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "terminal.h"
#include "usb_hhid_ep.h"

/* Private variable */
static uint8_t s_test_buf[KEYBOARD_REPORT_SIZE];

/* USB keyboard keycode data */
static const uint8_t s_usb_hhid_code[INDEX][ROW_SIZE][COL_SIZE] =
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

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Terminal thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void terminal_thread_entry_run(void)
{
    while (true)
    {
        char tem_buff[BUFF_SIZE] = {NULL_CHAR};
        uint8_t index = RESET_VALUE;
        BaseType_t err_queue;

        /* Receive data from queue */
        err_queue = xQueueReceive(g_data_queue, &s_test_buf[RESET_VALUE], portMAX_DELAY);
        if (pdTRUE != err_queue)
        {
            /* Unexpected queue receive failure */
            continue;
        }

        if (RESET_VALUE != (s_test_buf[RESET_VALUE] & (KEY_MOD_LSHIFT | KEY_MOD_RSHIFT)))
        {
            index = 1U;
        }
        else
        {
            index = RESET_VALUE;
        }

        /* Decode the received keycode to recognize key pressed on the keyboard */
        for (uint32_t cnt = 2U; cnt < sizeof(s_test_buf); cnt++)
        {
            if (RESET_VALUE != s_test_buf[cnt])
            {
                if (ROW_SIZE > s_test_buf[cnt])
                {

                    memcpy(tem_buff, &s_usb_hhid_code[index][s_test_buf[cnt]][RESET_VALUE], BUFF_SIZE);
                    printf("\r\n%s is pressed\r\n", tem_buff);
                }
                else
                {
                    printf("\r\nPressed key is out of specified key range\r\n");
                }
            }
        }

        vTaskDelay(TASK_DELAY);
    }
}
