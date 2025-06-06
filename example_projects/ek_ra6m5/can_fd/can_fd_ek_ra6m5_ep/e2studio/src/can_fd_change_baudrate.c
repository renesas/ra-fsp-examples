/***********************************************************************************************************************
 * File Name    : can_fd_change_baudrate.c
 * Description  : Contains data structures and functions used in can_fd_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "can_fd_change_baudrate.h"

/* Refer to 32.4.1 Initialize of CAN Clock, Bit Timing and Baud Rate (r01uh0891ej0140-ra6m5.pdf) */
const can_timing_t g_can_timming_max =
{
    .sjw = NOMINAL_SJW_MAX,
    .phase_seg1 = NOMINAL_TSEG1_MAX,
    .phase_seg2 = NOMINAL_TSEG2_MAX,
    .prescaler = NOMINAL_PRESCALER_MAX,
    .baudrate = NOMINAL_BAUDRATE_MAX,
};

const can_timing_t g_can_timming_min =
{
    .sjw = NOMINAL_SJW_MIN,
    .phase_seg1 = NOMINAL_TSEG1_MIN,
    .phase_seg2 = NOMINAL_TSEG2_MIN,
    .prescaler = NOMINAL_PRESCALER_MIN,
    .baudrate = NOMINAL_BAUDRATE_MIN,
};

const can_timing_t g_can_data_timming_max =
{
    .sjw = DATA_SJW_MAX,
    .phase_seg1 = DATA_TSEG1_MAX,
    .phase_seg2 = DATA_TSEG2_MAX,
    .prescaler = DATA_PRESCALER_MAX,
    .baudrate = DATA_BAUDRATE_MAX,
};

const can_timing_t g_can_data_timming_min =
{
    .sjw = DATA_SJW_MIN,
    .phase_seg1 = DATA_TSEG1_MIN,
    .phase_seg2 = DATA_TSEG2_MIN,
    .prescaler = DATA_PRESCALER_MIN,
    .baudrate = DATA_BAUDRATE_MIN,
};

/* External variables */
extern canfd_extended_cfg_t g_canfd0_extended_cfg;
extern bool g_transmission_start;

/* Global variable */
baudrate_set_status_t g_baudrate_set_status;

/* User defined functions */
static uint16_t clamp(uint16_t value, uint16_t min, uint16_t max);
static bool calculate_bit_timings(can_timing_t *result, const can_timing_t min, const can_timing_t max,
                                     uint32_t baudrate_req, uint16_t sample_pnt);
static fsp_err_t canfd_change_baudrate(uint32_t nominal_rate_req, uint32_t data_rate_req, uint16_t sample_pnt_req);
static baudrate_set_status_t canfd_baudrate_get_from_input(menu_t menu_option, uint32_t *nominal_rate_req,
                                                           uint32_t *data_rate_req, uint16_t *sample_pnt_req);

/**********************************************************************************************************************
 *  Function Name: canfd_baudrate_set
 *  Description  : This function sets the requested baudrate for CANFD module.
 *  Arguments    : None
 *  Return Value : None
 *********************************************************************************************************************/
void canfd_baudrate_set(void)
{
    menu_t menu_option = RESET_VALUE;
    static uint32_t nominal_rate_req = NOMINAL_BAUDRATE_DEFAULT;
    static uint32_t data_rate_req = DATA_BAUDRATE_DEFAULT;
    static uint16_t sample_pnt_req = SAMPLE_POINT_DEFAULT;

    while (true != g_transmission_start)
    {
        /* Reset baudrate set flag */
        g_baudrate_set_status &= RESET_VALUE;

        /* Print requested CANFD baudrate information */
        APP_PRINT("\r\nThe requested CANFD baud rate:");
        APP_PRINT(CANFD_BAUDRATE_INFO, nominal_rate_req, data_rate_req, sample_pnt_req);

        /* Print Menu option */
        APP_PRINT(MENU_OPTION);

        /* Get selected menu */
        menu_option = (menu_t) get_user_input();
        switch (menu_option)
        {
            case NOMINAL_BAUDRATE_SET:
            {
                while (RESET_VALUE == (g_baudrate_set_status & NOMINAL_SET_SUCCESS))
                {
                    g_baudrate_set_status = canfd_baudrate_get_from_input(menu_option, &nominal_rate_req,
                                                                           &data_rate_req, &sample_pnt_req);
                }

                break;
            }
            case DATA_BAUDRATE_SET:
            {
                while (RESET_VALUE == (g_baudrate_set_status & DATA_SET_SUCCESS))
                {
                    g_baudrate_set_status = canfd_baudrate_get_from_input(menu_option, &nominal_rate_req,
                                                                           &data_rate_req, &sample_pnt_req);
                }
                break;
            }
            case SAMPLE_POINT_SET:
            {
                while (RESET_VALUE == (g_baudrate_set_status & SAMPLE_POINT_SET_SUCCESS))
                {
                    APP_PRINT("\r\nEnter sample point (%%): Valid value from %d to %d", SAMPLE_POINT_MIN,
                              SAMPLE_POINT_MAX);
                    g_baudrate_set_status = canfd_baudrate_get_from_input(menu_option, &nominal_rate_req,
                                                                           &data_rate_req, &sample_pnt_req);
                }
                break;
            }
            case START_TRANSMISSION:
            {
                canfd_change_baudrate(nominal_rate_req, data_rate_req, sample_pnt_req);
                g_transmission_start = true;
                APP_PRINT(PERFORM_TRANSMISSION);
                break;
            }

            default:
            {
                APP_PRINT("Invalid input");
            }
            break;
    }

    }
}
/***********************************************************************************************************************
* End of function canfd_baudrate_set
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: clamp
 *  Description  : This function clamps the value to the range from min to max.
 *  Arguments    : value        input value
 *                 min          minimum of the range
 *                 max          maximum of the range
 *  Return Value : value        The value after clamping
 **********************************************************************************************************************/
static uint16_t clamp(uint16_t value, uint16_t min, uint16_t max)
{
    if (value < min)
    {
        value = min;
    }
    else if (value > max)
    {
        value = max;
    }
    else
    {
        /* Do nothing */
    }
    return value;
}
/***********************************************************************************************************************
* End of function clamp
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: calculate_bit_timings
 *  Description  : This function calculates the bit timings of CANFD.
 *  Arguments    : *result          Store calculated bit timings
 *                 min              Minimum value of bit timings
 *                 max              Maximum value of bit timings
 *                 baudrate_req     Requested baud rate
 *                 sample_pnt       Requested sample point
 *  Return Value : true             Accept calculated bit timings
 *                 false            Reject calculated bit timings
 **********************************************************************************************************************/
static bool calculate_bit_timings(can_timing_t *result, const can_timing_t min, const can_timing_t max,
                                     uint32_t baudrate_req, uint16_t sample_pnt)
{
    can_timing_t temp = {RESET_VALUE};

    uint32_t bit_clock = (CANFD_CLOCK / result->prescaler);
    uint16_t total_tq = (uint16_t)ceil((double)bit_clock / (double)baudrate_req);
    temp.prescaler = result->prescaler;
    temp.phase_seg1 = (uint16_t)ceil((double)total_tq * ((double)sample_pnt / 100) - CAN_SYNC_SEG);
    temp.phase_seg2 = (uint16_t)(total_tq - temp.phase_seg1 - CAN_SYNC_SEG);

    /* Adjust time segments if segment 2 is too small */
    if (temp.phase_seg2 < min.phase_seg2)
    {
        uint16_t adjust = min.phase_seg2 - temp.phase_seg2;
        temp.phase_seg1 -= adjust;
        temp.phase_seg2 += adjust;
    }

    if (temp.phase_seg1 > max.phase_seg1 || temp.phase_seg2 > max.phase_seg2
            || total_tq < (min.phase_seg1 + min.phase_seg2 + CAN_SYNC_SEG))
    {
        return false;
    }


    /* Calculate default sjw as phase_seg2 / 2 and clamp the result */
    temp.sjw = temp.phase_seg1 > (temp.phase_seg2 /2) ? (temp.phase_seg2 /2) : temp.phase_seg1 ;
    temp.sjw = clamp(temp.sjw, min.sjw, max.sjw);

    temp.baudrate = (uint32_t) (bit_clock / total_tq);
    uint16_t delta = (uint16_t)abs((int32_t)(baudrate_req - temp.baudrate));

    if (delta <= (uint16_t)abs((int32_t)(result->baudrate - baudrate_req)))
    {
        *result = temp;
        return true;
    }
    *result = temp;
    return false;
}
/***********************************************************************************************************************
* End of function calculate_bit_timings
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: canfd_change_baudrate
 *  Description  : This function changes CANFD baudrate.
 *  Arguments    : nominal_rate_req     Requested nominal baudrate
 *                 data_rate_req        Requested data baudrate
 *                 sample_pnt_req       Requested sample point
 *  Return Value : FSP_SUCCESS          Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS Unsuccessful operation
 **********************************************************************************************************************/
static fsp_err_t canfd_change_baudrate(uint32_t nominal_rate_req, uint32_t data_rate_req, uint16_t sample_pnt_req)
{
    fsp_err_t err = FSP_SUCCESS;

    can_timing_t nominal_best = {RESET_VALUE};
    can_timing_t data_best = {RESET_VALUE};

    /* De-initialize CANFD module */
    err = R_CANFD_Close(&g_canfd0_ctrl);
    APP_ERR_RETURN(err, "\r\n**R_CANFD_Close API failed**");

    /* Initialize baud rate of nominal_best as requested nominal rate */
    nominal_best.baudrate = nominal_rate_req;

    /* Loop over all available pre-scalers */
    for (uint16_t prescaler = NOMINAL_PRESCALER_MIN; prescaler <= NOMINAL_PRESCALER_MAX; ++prescaler)
    {
        nominal_best.prescaler = prescaler;
        /* Calculate nominal bit timings */
        if (calculate_bit_timings (&nominal_best, g_can_timming_min, g_can_timming_max, nominal_rate_req,
                                   sample_pnt_req))
        {
            break;
        }
    }

    /* Initialize baud rate of data_best as requested data rate */
    data_best.baudrate = data_rate_req;

    /* Loop over all available pre-scalers */
    for (uint16_t prescaler = DATA_PRESCALER_MIN; prescaler <= DATA_PRESCALER_MAX; ++prescaler)
    {
        data_best.prescaler = prescaler;
        /* Calculate data bit timings */
        if (calculate_bit_timings (&data_best, g_can_data_timming_min, g_can_data_timming_max, data_rate_req,
                                   sample_pnt_req))
        {
            break;
        }
    }

    /* Update CANFD nominal bit timings */
    g_canfd0_cfg.p_bit_timing->baud_rate_prescaler = nominal_best.prescaler;
    g_canfd0_cfg.p_bit_timing->synchronization_jump_width = nominal_best.sjw;
    g_canfd0_cfg.p_bit_timing->time_segment_1 = nominal_best.phase_seg1;
    g_canfd0_cfg.p_bit_timing->time_segment_2 = nominal_best.phase_seg2;

    /* Update CANFD data bit timings */
    g_canfd0_extended_cfg.p_data_timing->baud_rate_prescaler = data_best.prescaler;
    g_canfd0_extended_cfg.p_data_timing->synchronization_jump_width = data_best.sjw;
    g_canfd0_extended_cfg.p_data_timing->time_segment_1 = data_best.phase_seg1;
    g_canfd0_extended_cfg.p_data_timing->time_segment_2 = data_best.phase_seg2;

    /* Print actual CANFD baud rate */
    APP_PRINT("\r\nThe actual CANFD baud rate:");
    APP_PRINT(CANFD_BAUDRATE_INFO, nominal_best.baudrate, data_best.baudrate, sample_pnt_req);

    /* Re-initialize CANFD module with updated baud rate */
    err = R_CANFD_Open(&g_canfd0_ctrl, &g_canfd0_cfg);
    APP_ERR_RETURN(err, "\r\n**R_CANFD_Open API failed**");

    return err;
}
/***********************************************************************************************************************
* End of function canfd_change_baudrate
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: canfd_baudrate_get_from_input
 *  Description  : This function gets the requested CANFD baudrate from user input.
 *  Arguments    : menu_option                  Selected option
 *                 *nominal_rate_req            Nominal baudrate get from input
 *                 *data_rate_req               Data baudrate get from input
 *                 *sample_pnt_req              Sample point get from input
 *  Return Value : g_baudrate_set_status        Status of get baudrate from user input
 **********************************************************************************************************************/
static baudrate_set_status_t canfd_baudrate_get_from_input(menu_t menu_option, uint32_t *nominal_rate_req,
                                                           uint32_t *data_rate_req, uint16_t *sample_pnt_req)
{
    uint32_t custom_input = RESET_VALUE;

    switch (menu_option)
    {
        case NOMINAL_BAUDRATE_SET:
        {
            g_baudrate_set_status |= NOMINAL_SET_SUCCESS;

            APP_PRINT(NOMINAL_RATE_OPTION);

            nominal_rate_opt_t nominal_opt = (nominal_rate_opt_t) get_user_input();
            switch (nominal_opt)
            {
                case KBPS_250:
                {
                    *nominal_rate_req = KBPS_250_VAL;
                }
                break;
                case KBPS_500:
                {
                    *nominal_rate_req = KBPS_500_VAL;
                }
                break;
                case KBPS_1000:
                {
                    *nominal_rate_req = MBPS_1_VAL;
                }
                break;
                case NOMINAL_CUSTOM:
                {
                    APP_PRINT("\r\nEnter nominal rate(bits/s): Valid value from %d to %d", NOMINAL_BAUDRATE_MIN,
                              NOMINAL_BAUDRATE_MAX);
                    custom_input = get_user_input();
                    if (NOMINAL_BAUDRATE_MIN <= custom_input && NOMINAL_BAUDRATE_MAX >= custom_input)
                    {
                        *nominal_rate_req = custom_input;
                    }
                    else
                    {
                        APP_PRINT("The input is out of range\r\n");
                        g_baudrate_set_status &= !NOMINAL_SET_SUCCESS;
                    }
                }
                break;
                default:
                    APP_PRINT("Invalid option\r\n");
                    g_baudrate_set_status &= !NOMINAL_SET_SUCCESS;
                break;
            }
        }
        break;
        case DATA_BAUDRATE_SET:
        {
            g_baudrate_set_status |= DATA_SET_SUCCESS;

            APP_PRINT(DATA_RATE_OPTION);

            data_rate_opt_t data_opt = (data_rate_opt_t) get_user_input();
            switch (data_opt)
            {
                case MBPS_1:
                {
                    *data_rate_req = MBPS_1_VAL;
                }
                break;
                case MBPS_2:
                {
                    *data_rate_req = MBPS_2_VAL;
                }
                break;
                case MBPS_5:
                {
                    *data_rate_req = MBPS_5_VAL;
                }
                break;
                case DATA_CUSTOM:
                {
                    APP_PRINT("\r\nEnter data rate(bits/s): Valid value from %d to %d", DATA_BAUDRATE_MIN,
                              DATA_BAUDRATE_MAX);
                    custom_input = get_user_input();
                    if (DATA_BAUDRATE_MIN <= custom_input && DATA_BAUDRATE_MAX >= custom_input)
                    {
                        *data_rate_req = custom_input;
                        g_baudrate_set_status |= DATA_SET_SUCCESS;
                    }
                    else
                    {
                        APP_PRINT("The input is out of range\r\n");
                    }
                }
                break;
                default:
                    APP_PRINT("Invalid option\r\n");
                    g_baudrate_set_status &= !DATA_SET_SUCCESS;
                break;
            }
            break;
        }
        case SAMPLE_POINT_SET:
        {
            custom_input = get_user_input();
            if (SAMPLE_POINT_MIN <= custom_input && SAMPLE_POINT_MAX >= custom_input)
            {
                *sample_pnt_req = (uint16_t) custom_input;
                g_baudrate_set_status |= SAMPLE_POINT_SET_SUCCESS;
            }
            else
            {
                APP_PRINT("The input is out of range\r\n");
            }
            break;
        }
        case START_TRANSMISSION:
        {
            break;
        }
        default:
            APP_PRINT("Invalid option\r\n");
            break;
    }

    return g_baudrate_set_status;
}
/***********************************************************************************************************************
* End of function canfd_baudrate_get_from_input
***********************************************************************************************************************/
