/***********************************************************************************************************************
 * File Name    : can_fd_change_baudrate.c
 * Description  : Contains data structures and functions used in can_fd_change_baudrate.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <math.h>
#include <stdlib.h>
#include "can_fd_change_baudrate.h"

/* Refer to 32.4.1 Initialize of CAN Clock, Bit Timing and Baud Rate (r01uh0891ej0140-ra6m5.pdf) */
static const can_timing_t s_can_timing_max =
{
    .sjw = NOMINAL_SJW_MAX,
    .phase_seg1 = NOMINAL_TSEG1_MAX,
    .phase_seg2 = NOMINAL_TSEG2_MAX,
    .prescaler = NOMINAL_PRESCALER_MAX,
    .baudrate = NOMINAL_BAUDRATE_MAX,
};

static const can_timing_t s_can_timing_min =
{
    .sjw = NOMINAL_SJW_MIN,
    .phase_seg1 = NOMINAL_TSEG1_MIN,
    .phase_seg2 = NOMINAL_TSEG2_MIN,
    .prescaler = NOMINAL_PRESCALER_MIN,
    .baudrate = NOMINAL_BAUDRATE_MIN,
};

static const can_timing_t s_can_data_timing_max =
{
    .sjw = DATA_SJW_MAX,
    .phase_seg1 = DATA_TSEG1_MAX,
    .phase_seg2 = DATA_TSEG2_MAX,
    .prescaler = DATA_PRESCALER_MAX,
    .baudrate = DATA_BAUDRATE_MAX,
};

static const can_timing_t s_can_data_timing_min =
{
    .sjw = DATA_SJW_MIN,
    .phase_seg1 = DATA_TSEG1_MIN,
    .phase_seg2 = DATA_TSEG2_MIN,
    .prescaler = DATA_PRESCALER_MIN,
    .baudrate = DATA_BAUDRATE_MIN,
};

/* Private functions */
static uint16_t clamp(uint16_t value, uint16_t min, uint16_t max);
static bool calculate_bit_timings(can_timing_t * result, const can_timing_t min, const can_timing_t max,
                                  uint32_t baudrate_req, uint16_t sample_pnt);

/***********************************************************************************************************************
 * Function Name: clamp.
 * Description  : This function clamps the value to the range from minimum to maximum.
 * Arguments    : value                     Input value.
 *                min                       Minimum of the range.
 *                max                       Maximum of the range.
 * Return Value : value                     The value after clamping.
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
 * Function Name: calculate_bit_timings.
 * Description  : This function calculates the bit timings of CANFD.
 * Arguments    : result                    Pointer to store calculated bit timings.
 *                min                       Minimum value of bit timings.
 *                max                       Maximum value of bit timings.
 *                baudrate_req              Requested baud rate.
 *                sample_pnt                Requested sample point.
 * Return Value : true                      Accept calculated bit timings.
 *                false                     Reject calculated bit timings.
 **********************************************************************************************************************/
static bool calculate_bit_timings(can_timing_t * result, const can_timing_t min, const can_timing_t max,
                                  uint32_t baudrate_req, uint16_t sample_pnt)
{
    can_timing_t temp = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};
    uint32_t bit_clock = (CANFD_CLOCK / result->prescaler);
    uint16_t total_tq = (uint16_t) ceil((double) bit_clock / (double) baudrate_req);
    temp.prescaler = result->prescaler;
    temp.phase_seg1 = (uint16_t) ceil((double) total_tq * ((double) sample_pnt / 100) - CAN_SYNC_SEG);
    temp.phase_seg2 = (uint16_t) (total_tq - temp.phase_seg1 - CAN_SYNC_SEG);

    /* Adjust time segments if segment 2 is too small */
    if (temp.phase_seg2 < min.phase_seg2)
    {
        uint16_t adjust = min.phase_seg2 - temp.phase_seg2;
        temp.phase_seg1 -= adjust;
        temp.phase_seg2 += adjust;
    }

    if (temp.phase_seg1 > max.phase_seg1 || temp.phase_seg2 > max.phase_seg2 ||
        total_tq < (min.phase_seg1 + min.phase_seg2 + CAN_SYNC_SEG))
    {
        return false;
    }

    /* Calculate default sjw as phase_seg2 / 2 and clamp the result */
    temp.sjw = temp.phase_seg1 > (temp.phase_seg2 / 2) ? (temp.phase_seg2 / 2) : temp.phase_seg1;
    temp.sjw = clamp(temp.sjw, min.sjw, max.sjw);
    temp.baudrate = (uint32_t) (bit_clock / total_tq);
    uint16_t delta = (uint16_t) abs((int32_t) (baudrate_req - temp.baudrate));

    if (delta <= (uint16_t) abs((int32_t) (result->baudrate - baudrate_req)))
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
 * Function Name: canfd_change_baudrate.
 * Description  : This function changes the CANFD baudrate.
 * Arguments    : nominal_rate_req          Requested nominal baudrate.
 *                data_rate_req             Requested data baudrate.
 *                sample_pnt_req            Requested sample point.
 *                p_nominal_actual          Pointer to store the actual nominal bit timings applied.
 *                p_data_actual             Pointer to store the actual data bit timings applied.
 * Return Value : FSP_SUCCESS               Changed successfully.
 *                Any other error codes     Failed to change.
 **********************************************************************************************************************/
fsp_err_t canfd_change_baudrate(uint32_t nominal_rate_req, uint32_t data_rate_req, uint16_t sample_pnt_req,
                                can_timing_t * p_nominal_actual, can_timing_t * p_data_actual)
{
    fsp_err_t err = FSP_SUCCESS;

    can_timing_t nominal_best = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};
    can_timing_t data_best = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};

    /* De-initialize CANFD module */
    can_fd_deinit();

    /* Initialize baud rate of nominal_best as requested nominal rate */
    nominal_best.baudrate = nominal_rate_req;

    /* Loop over all available pre-scalers */
    for (uint16_t prescaler = NOMINAL_PRESCALER_MIN; prescaler <= NOMINAL_PRESCALER_MAX; ++prescaler)
    {
        nominal_best.prescaler = prescaler;

        /* Calculate nominal bit timings */
        if (calculate_bit_timings(&nominal_best, s_can_timing_min, s_can_timing_max, nominal_rate_req,
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
        if (calculate_bit_timings(&data_best, s_can_data_timing_min, s_can_data_timing_max, data_rate_req,
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

    /* Report the actual bit timings applied back to the caller */
    *p_nominal_actual = nominal_best;
    *p_data_actual = data_best;

    /* Re-initialize CANFD module with updated baud rate */
    err = can_fd_init();

    return err;
}
/***********************************************************************************************************************
* End of function canfd_change_baudrate
***********************************************************************************************************************/
