/***********************************************************************************************************************
 * File Name    : ethos_u55.c
 * Description  : Contains data structures and functions for Ethos-U55 face detection application.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ETHOS_U55_H_
#define ETHOS_U55_H_

#include <stdint.h>
#include "hal_data.h"

#define RESET_VALUE     (0x00)

#define EP_VERSION      ("1.0")
#define MODULE_NAME     "ethosu"
#define BANNER_INFO     "\r\n*********************************************************************"\
                        "\r\n*   Renesas FSP Example Project for " MODULE_NAME " Module                   *"\
                        "\r\n*   Example Project Version %s                                     *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                               *"\
                        "\r\n*********************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and" \
                        "\r\nFSP User's Manual for more information about " MODULE_NAME " module\r\n"

#define EP_INFO         "\r\nThis project showcases the AI inference capabilities of the Arm® Ethos™-U" \
                        "\r\nNeural Processing Unit (NPU) integrated into Renesas RA microcontrollers. It" \
                        "\r\nimplements a face detection application using a pre-trained model sourced from" \
                        "\r\nthe Arm ML-Zoo, optimized for the Ethos-U NPU via the Vela compiler." \
                        "\r\nInference is executed using TensorFlow Lite for Microcontrollers (TFLM). The" \
                        "\r\ninput image is supplied by the user as a statically defined array in C and" \
                        "\r\nthe application outputs bounding box coordinates of detected faces to the" \
                        "\r\nterminal.\r\n\r\n"

#ifdef __cplusplus
extern "C" {
#endif

void face_detection_entry(void);
#if (INFERENCE_TIME_MEASUREMENT_ENABLED == 1)
fsp_err_t timer_get_measure (uint32_t * p_time);
#endif/* INFERENCE_TIME_MEASUREMENT_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* ETHOS_U55_H_ */
