/***********************************************************************************************************************
 * File Name    : ethos_u55_face_detection_ep.h
 * Description  : Contains macros and functions used in ethos_u55_face_detection_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ETHOS_U55_FACE_DETECTION_EP_H_
#define ETHOS_U55_FACE_DETECTION_EP_H_

#include "hal_data.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Macro for the reset value */
#define RESET_VALUE     (0x00)

/* External function */
extern void MainLoop(void);

/* Function declarations */
#ifdef ARM_NPU
fsp_err_t ethos_u55_npu_open(void);
#endif /* ARM_NPU */

#if (INFERENCE_TIME_MEASUREMENT_ENABLED == 1)
fsp_err_t ethos_u55_timer_open(void);
fsp_err_t ethos_u55_timer_start(void);
fsp_err_t ethos_u55_timer_get_measure(uint32_t * p_time);
#endif /* INFERENCE_TIME_MEASUREMENT_ENABLED */

void ethos_u55_deinit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ETHOS_U55_FACE_DETECTION_EP_H_ */
