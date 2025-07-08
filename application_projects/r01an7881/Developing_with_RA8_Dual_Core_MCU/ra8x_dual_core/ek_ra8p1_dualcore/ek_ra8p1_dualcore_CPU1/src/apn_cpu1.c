/***********************************************************************************************************************
 * File Name    : apn_cpu1.c
 * Description  : This source file implements the main functions of application on CPU1.
 *                Managing the user interface and logging data to the terminal.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <apn_cpu1.h>
#include "hal_data.h"

/***********************************************************************************************************************
 * Extern variables
 **********************************************************************************************************************/
extern volatile uint8_t g_rx_complete_flag;
extern char     g_rx_buffer[BUFFER_SIZE_DOWN];
extern          uint8_t g_cpu0_rq_print;
extern          uint8_t temp_val_get;
extern          uint32_t temp_val;
extern          share_mem_t *share_mem;
extern const    bsp_ipc_semaphore_handle_t g_ipc_sem_smem;
extern const    bsp_ipc_semaphore_handle_t g_ipc_sem_input;

/******************************************************************************
 * Private Function
 *****************************************************************************/
static int32_t check_alarm_input(void);
static float   cal_temp(uint32_t adc_val);

/*******************************************************************************************************************//**
 * @brief      This function check if alarm input by user is valid
 *
 * @param[in]  None
 * @retval     int32_t   Alarm value if the input is valid;
 *                       negative error code if the input is invalid.
 **********************************************************************************************************************/
static int32_t check_alarm_input(void)
{
    char *endptr;
    uint32_t alarm_val = RESET_VALUE;

    alarm_val = (uint32_t) strtol(g_rx_buffer, &endptr, 10);

    /* check if input contains char return error */
    if (*endptr != '\0')
    {
        /* Cleaning receive buffer */
        memset (g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));
        return ERR_INVALID_USER_INPUT;
    }
    /* or invalid second return error */
    if (alarm_val < 0 || alarm_val > 59)
    {

        /* Cleaning receive buffer */
        memset (g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));

        return ERR_INVALID_USER_INPUT;
    }

    /* Cleaning receive buffer */
    memset (g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));
    return (int32_t)alarm_val;
}

/*******************************************************************************************************************//**
 * @brief      Converts a digital ADC value to temperature in degrees Celsius.
 *
 * @param[in]  adc_val   Raw ADC value to be converted.
 *
 * @retval     float     Calculated temperature in degrees Celsius.
 **********************************************************************************************************************/
float cal_temp(uint32_t adc_val)
{
    float degC_temp = RESET_VALUE;
    uint32_t cal_95 = R_TSN_CAL->TSCDR;
    /* Calculate the temperature from ADC TSN value
     * Refer to section 56.3.1 of the RA8P1 Hardware User Manual for further info
     */
    float v1 = 3.3f * (float)cal_95 / 4096.0f;
    float vs = 3.3f * (float)adc_val / 4096.0f;

    degC_temp = ((vs - v1) / (2.7f/1000)) + 95.0f;

    return degC_temp;
}

/*******************************************************************************************************************//**
 * @brief     Main application entry
 *
 * @param[in] NONE
 * @return    NONE
 **********************************************************************************************************************/
void apn1_entry(void)
{
    /* Initialize variables */
    fsp_err_t err = FSP_SUCCESS;
    uint8_t user_input = RESET_VALUE;
    int32_t alarm_sec = RESET_VALUE;
    float degC_temp = 0.00f;
    statte_screen_t current_screen =  MAIN_SCREEN;

    err = uart_init ();
    if (FSP_SUCCESS != err)
    {
        /* Error trap */
        __asm("BKPT #0\n");
    }

    /* initialize inter-processor communication */
    err = ipc_init(&g_ipc0, &g_ipc1);
    if (FSP_SUCCESS != err)
    {
        /* Error trap */
        __asm("BKPT #0\n");
    }

    R_IPC_EventGenerate (&g_ipc0_ctrl, IPC_GENERATE_EVENT_IRQ2);

    /* Clear entire screen before print banner menu */
    APP_CLEAR_SCREEN;

    /* Print out banner menu */
    APP_PRINT(BANNER_POS,MENU_INFO);

    while (1)
    {
        /* Check if receive any messages from CPU0*/
        if (g_cpu0_rq_print)
        {
            /* Write the messages request from CPU0 to terminal */
            uart_write_message(CORE_0_MESSAGE);

            g_cpu0_rq_print = RESET_FLAG;
        }

        /* if receive the temperature value each Second */
        if (SET_FLAG == temp_val_get)
        {
            /* Convert the value to temperature in degrees Celsius.*/
            degC_temp = cal_temp(temp_val);
            /* Print out the temperature value to terminal */
            APP_PRINT_INFO(TEMP_POS,"Temperature: %.2f deg C", degC_temp);
            temp_val_get = RESET_FLAG;
        }

        /* Check any ing_rx_complete_flagput from user */
        if (g_rx_complete_flag)
        {
            switch (current_screen)
            {
                case MAIN_SCREEN:
                    /* Convert data in g_rx_buffer to integer */
                    user_input = (uint8_t) atoi (g_rx_buffer);

                    /* Cleaning buffer */
                    memset (g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));

                    if (SET_DATE_TIME == user_input)
                    {
                        current_screen = RTC_INPUT_SCREEN;
                        SHOW_SCREEN(current_screen);
                    }
                    else if (SET_ALARM == user_input)
                    {
                        err = smem_output_busy();
                        if (err != FSP_SUCCESS)
                        {
                            APP_ERR_TRAP(err);
                        }

                        /* Check for Date and Time already set up */
                        if (SET_FLAG == share_mem->valid_sequent)
                        {
                            current_screen = SET_ALARM_SCREEN;
                            SHOW_SCREEN(current_screen);
                        }
                        else
                        {
                            SHOW_SCREEN(current_screen);
                            APP_PRINT_INFO(LOG_POS,"[ERR]: Invalid sequence. Please set RTC calendar Date and Time First\r\n");
                        }
                        R_BSP_IpcSemaphoreGive(&g_ipc_sem_smem);
                    }
                    else if (START_STOP_ADC == user_input)
                    {

                        err = smem_output_busy();
                        if (err != FSP_SUCCESS)
                        {
                            APP_ERR_TRAP(err);
                        }

                        /* Check for Date and Time already set up */
                        if (SET_FLAG == share_mem->valid_sequent)
                        {
                            /* IPC NMI to CPU0 for stop ADC_TSN */
                            err = R_BSP_IpcNmiRequestSet ();
                            if (FSP_SUCCESS != err)
                            {
                                /* IPC-NMI Failure Message */
                                APP_PRINT_INFO(LOG_POS,"\nRequest interrupt fail");
                            }
                        }
                        else
                        {
                            APP_PRINT_INFO(LOG_POS,"[ERR]: Invalid sequence. Please set RTC calendar Date and Time First\r\n");
                        }
                        SHOW_SCREEN(current_screen);
                        R_BSP_IpcSemaphoreGive(&g_ipc_sem_smem);
                    }
                    /* Other Input than 1 -> 3 */
                    else
                    {
                        SHOW_SCREEN(MAIN_SCREEN);
                        /* IPC-NMI Failure Message */
                        APP_PRINT_INFO(LOG_POS,"[ERR]: Invalid input.");
                    }
                    g_rx_complete_flag = RESET_FLAG;
                    break;
                case RTC_INPUT_SCREEN:
                    err = smem_input_busy();
                    if (FSP_SUCCESS != err)
                    {
                        APP_ERR_TRAP(err);
                    }
                    /* Copy the Date and Time value from input to shared memory for set up at CPU0 */
                    memcpy(share_mem->time_buf, &g_rx_buffer, BUFFER_SIZE_DOWN);

                    /* Use IPC-MI trigger CPU0 read from share memory and set up date and time */
                    R_IPC_EventGenerate(&g_ipc0_ctrl, IPC_GENERATE_EVENT_IRQ1);

                    /* Finish write alarm data in shared memory, release semaphore*/
                    R_BSP_IpcSemaphoreGive(&g_ipc_sem_input);

                    current_screen = MAIN_SCREEN;
                    SHOW_SCREEN(current_screen);
                    /* Cleaning receive buffer */
                    memset(g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));
                    g_rx_complete_flag = RESET_FLAG;
                    break;
                case SET_ALARM_SCREEN:
                    /* Wait until an invalid data input to terminal */
                    alarm_sec = check_alarm_input();
                    if (ERR_INVALID_USER_INPUT !=  alarm_sec)
                    {
                        err = R_IPC_MessageSend (&g_ipc0_ctrl, (uint32_t) alarm_sec);

                        current_screen = MAIN_SCREEN;
                        SHOW_SCREEN(current_screen);

                        /* Cleaning receive buffer */
                        memset (g_rx_buffer, NULL_CHAR, sizeof(g_rx_buffer));
                    }
                    else
                    {
                        current_screen = MAIN_SCREEN;
                        SHOW_SCREEN(current_screen);
                        APP_PRINT_INFO(LOG_POS,"[ERR]: Invalid Alarm Input.");
                    }
                    g_rx_complete_flag = RESET_FLAG;
                    break;
                default:
                {
                    break;
                }

            }
        }

    }
}
