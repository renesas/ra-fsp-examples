/***********************************************************************************************************************
 * File Name    : apc_cpu0.c
 * Description  : This source file implements the main functions of application on CPU0.
 *                Process base on user request from CPU1.
 *                Handle real-time control and sensor data processing and Led control.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "apn_cpu0.h"
#include "tsn_control.h"
#include "rtc.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/***********************************************************************************************************************
 * Extern global variables
 **********************************************************************************************************************/
extern bsp_leds_t g_bsp_leds;
extern rtc_time_t g_present_time;
extern uint8_t g_periodic_irq_flag;
extern uint8_t g_alarm_irq_flag;
extern const bsp_ipc_semaphore_handle_t g_ipc_sem_input;
extern const bsp_ipc_semaphore_handle_t g_ipc_sem_smem;

/******************************************************************************
 * Global Variables
 *****************************************************************************/
share_mem_t share_memory BSP_PLACE_IN_SECTION(".shared_mem") = {.buf_out = {RESET_VALUE},
                                                                .length = RESET_VALUE,
                                                                .valid_sequent = RESET_VALUE,
                                                                .time_buf = {RESET_VALUE}};

/******************************************************************************
 * Private global variables
 *****************************************************************************/
volatile static uint8_t g_time_set_notify = RESET_FLAG;
volatile static uint8_t g_alarm_set_notify = RESET_FLAG;
volatile static uint8_t g_uart_ready = RESET_FLAG;

static tsn_request_t g_tsn_scan_rq = STOP_SCAN_TSN;
static uint8_t val_alarm = RESET_VALUE;

/******************************************************************************
 * Private Function
 *****************************************************************************/
static bsp_io_level_t led_control(bsp_io_level_t pin_level);
static void nmi_ipc_callback (void);
static void read_date_time(uint8_t *pBuffer);
static fsp_err_t ipc_init(const ipc_instance_t *ipc0, const ipc_instance_t *ipc1);

/*******************************************************************************************************************//**
 * @brief     This Function For toggle Led base on the current Led status.
 *
 * @param[in] bsp_io_level_t: The current Led status.
 * @return    bsp_io_level_t: The Led status after change state.
 **********************************************************************************************************************/
static bsp_io_level_t led_control(bsp_io_level_t pin_level)
{
    /* LED type structure */
   bsp_leds_t leds = g_bsp_leds;

   /* If this board has no LEDs then trap here */
   if (0 == leds.led_count)
   {
       while (1)
       {
           ;                          // There are no LEDs on this board
       }
   }

   /* Toggle level for next write */
   if (BSP_IO_LEVEL_LOW == pin_level)
   {
       pin_level = BSP_IO_LEVEL_HIGH;
   }
   else
   {
       pin_level = BSP_IO_LEVEL_LOW;
   }

   /* Enable access to the PFS registers. If using r_ioport module then register protection is automatically
    * handled. This code uses BSP IO functions to show how it is used.
    */
   R_BSP_PinAccessEnable();

   /* Update all board LEDs */
   for (uint32_t i = 0; i < leds.led_count; i++)
   {
       /* Get pin to toggle */
       uint32_t pin = leds.p_leds[i];

       /* Write to this pin */
       R_BSP_PinWrite((bsp_io_port_pin_t) pin, pin_level);
   }

   /* Protect PFS registers */
   R_BSP_PinAccessDisable();

   return pin_level;
}

/*******************************************************************************************************************//**
 * @brief     Read the Date and Time information at share memory.
 *
 * @param[in] uint8_t *: Buffer for stored Date and Time value.
 * @return    None.
 **********************************************************************************************************************/
static void read_date_time(uint8_t *pBuffer)
{
    if (NULL != (void *) pBuffer)
    {
        (void) 0;   // Do nothing
    }
    else
    {
        APP_ERR_PRINT("\r\n Invalid parameter");
        APP_ERR_TRAP(0);
    }
    memcpy(pBuffer, &share_memory.time_buf, BUFFER_SIZE_DOWN);
    R_IPC_EventGenerate(&g_ipc1_ctrl, IPC_GENERATE_EVENT_IRQ1);
}

/*******************************************************************************************************************//**
 * @brief     IPC0 callback
 *
 * @param[in] ipc_callback_args_t: The callback arguments.
 * @return    None
 **********************************************************************************************************************/
void ipc0_callback(ipc_callback_args_t *p_args)
{
    switch (p_args->event) {
        case IPC_EVENT_IRQ0:
            R_BSP_IpcSemaphoreGive(&g_ipc_sem_input);
            break;
        case IPC_EVENT_IRQ1:
        {
            g_time_set_notify = SET_FLAG;
            break;
        }
        case IPC_EVENT_IRQ2:
        {
            g_uart_ready = SET_FLAG;
            break;
        }
        case IPC_EVENT_IRQ3:
        {
            R_BSP_IpcSemaphoreGive(&g_ipc_sem_smem);
            break;
        }
        case IPC_EVENT_MESSAGE_RECEIVED:
        {
            val_alarm = (uint8_t)p_args->message;
            g_alarm_set_notify = SET_FLAG;
            break;
        }
        default:
            break;
    }
}

/*******************************************************************************************************************//**
 * @brief     IPC Non-Maskable-Interrupt
 *
 * @param[in] NONE
 * @return    NONE
 **********************************************************************************************************************/
static void nmi_ipc_callback (void)
{
    g_tsn_scan_rq = (g_tsn_scan_rq == SCAN_TSN) ? STOP_SCAN_TSN : SCAN_TSN;
}

/*******************************************************************************************************************//**
 * @brief     Initialize IPC
 *
 * @param[in] IPC instances
 * @return    fsp_err_t
 **********************************************************************************************************************/
static fsp_err_t ipc_init(const ipc_instance_t *ipc0, const ipc_instance_t *ipc1)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize IPC0 */
    err = R_IPC_Open(ipc0->p_ctrl, ipc0->p_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Initialize IPC1 */
    err = R_IPC_Open(ipc1->p_ctrl, ipc1->p_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Enable IPC Non mask-able interrupt and assign the callback */
    err = R_BSP_IpcNmiEnable(nmi_ipc_callback);

    return err;
}
/*******************************************************************************************************************//**
 * @brief     Main application entry
 *
 * @param[in] NONE
 * @return    NONE
 **********************************************************************************************************************/
void apn0_entry(void)
{
    /* Initialize variables */
    fsp_err_t err = FSP_SUCCESS;
    uint8_t ready_to_read = RESET_FLAG;
    uint8_t date_time_buf[BUFFER_SIZE_DOWN] = {'\0'};
    uint16_t adc_data = RESET_VALUE;
    bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW;

    memset(&share_memory,RESET_VALUE, sizeof(share_memory));

    /* initialize IPC */
    ipc_init(&g_ipc0, &g_ipc1);

    /* Start Second core */
    R_BSP_SecondaryCoreStart();

    /* Wait until the UART module has initialized on CPU1 */
    while(!g_uart_ready)
    {
        __WFI();
    }

    /* Initialize RTC module */
    rtc_init();

    /* Initialize ADC_TSN */
    init_adc_tsn();

    while(true)
    {
        /* Receive Set time request from CPU1 */
        if (g_time_set_notify)
        {
            /* Read Date and Time information in share memory region */
            read_date_time(date_time_buf);

            /* Set up Date and Time to RTC module */
            err = set_rtc_calendar_time(date_time_buf);
            if (FSP_SUCCESS != err)
            {
                if (FSP_ERR_INVALID_ARGUMENT == err)
                {
                    /* Input data not valid Print out the err message */
                    APP_PRINT_INFO(LOG_POS,"[ERR]: Invalid Date and Time\r\n");
                }
                else
                {
                    APP_ERR_PRINT("R_RTC_CalendarTimeSet failed.\r\n");
                    APP_ERR_TRAP(err);
                }
            }
            else
            {
                /* Notify to CPU1 RTC time set successfully */
                share_memory.valid_sequent = SET_FLAG;
                /* Release semaphore after write to share memory*/
                R_BSP_IpcSemaphoreGive(&g_ipc_sem_smem);
                APP_PRINT_INFO(LOG_POS,"[INFO]: Calendar Date and Time set successfully\r\n");
                /* set up periodic each second to send temperature value to terminal  */
                set_rtc_periodic_second();
            }
            g_time_set_notify = RESET_FLAG;
        }
        if (g_periodic_irq_flag)
        {
            /*******************************************************************************************************************//**
             * Update Date and Time and temperature value each seconds to terminal
             **********************************************************************************************************************/
            /* Get the current Calendar time */
            err = R_RTC_CalendarTimeGet(&g_rtc_ctrl, &g_present_time);

            /* Handle error */
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nCalendarTime Get failed.\r\n");
            }

            /* Modify the date in standard format to user readable format */
            rtc_date_readability_update(&g_present_time);
            APP_PRINT_INFO(DATE_POS,"Date %02d: %02d : %02d",g_present_time.tm_mday, g_present_time.tm_mon, g_present_time.tm_year);
            APP_PRINT_INFO(TIME_POS,"Time %02d: %02d : %02d",g_present_time.tm_hour, g_present_time.tm_min, g_present_time.tm_sec);

            /* Send temperature data base on request */
            if (SCAN_TSN == g_tsn_scan_rq)
            {
                if (!ready_to_read)
                {
                    /* Start Scan ADC-TSN */
                    tsn_scan_start();
                    ready_to_read = SET_FLAG;
                }
                /* Read ADC-TSN  data  each second */
                 tsn_read_data(&adc_data);

                 err = R_IPC_MessageSend(&g_ipc1_ctrl, adc_data);
                 if (FSP_SUCCESS != err)
                 {
                     /* IPC Failure Message */
                     APP_PRINT_INFO(LOG_POS,"\r\nR_IPC_MessageSend failed.\r\n");
                 }
            }
            g_periodic_irq_flag = RESET_FLAG;
        }

        /* Stop scan temperature if receive the request */
        if (STOP_SCAN_TSN == g_tsn_scan_rq && ready_to_read)
        {
            tsn_scan_stop();
            /* Clean log temperature value*/
            APP_PRINT_INFO(TEMP_POS,"\033[2K");
            APP_PRINT_INFO(TSN_STATUS_POS,"[INFO]: Stop Scan Temperature.");

            ready_to_read = RESET_FLAG;
        }
        else if (SCAN_TSN == g_tsn_scan_rq && !ready_to_read)
        {
            /* Start Scan ADC-TSN */
            tsn_scan_start();
            APP_PRINT_INFO(TSN_STATUS_POS,"[INFO]: Scanning Temperature...");
            ready_to_read = SET_FLAG;
        }

        /* set rtc alarm value base on user input */
        if (g_alarm_set_notify)
        {
            set_rtc_calendar_alarm(val_alarm);
            APP_PRINT_INFO(LOG_POS,"[INFO]: The LED will change its state when alarm occurs",val_alarm);
            g_alarm_set_notify = RESET_FLAG;
        }
        /* Toggle Led if rtc alarm occurs*/
        if (g_alarm_irq_flag)
        {
            pin_level = led_control(pin_level);
            g_alarm_irq_flag = RESET_FLAG;
        }
    }
}


