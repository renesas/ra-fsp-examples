/*
 * ek_ra6m3_platform.c
 *
 *  Created on: May 26, 2020
 *      Author: Rajkumar.Thiagarajan
 */


#include <azure/core/az_platform.h>
#include <azure/core/internal/az_precondition_internal.h>
#include "hal_data.h"
#include <azure/core/_az_cfg.h>

az_result az_platform_clock_msec(int64_t* out_clock_msec) {

    _az_PRECONDITION_NOT_NULL(out_clock_msec);
    *out_clock_msec = ((int64_t)xTaskGetTickCount());
    return AZ_OK;
}

az_result az_platform_sleep_msec(int32_t milliseconds) {
    vTaskDelay((TickType_t)milliseconds + 1);
    return AZ_OK;
}
