/***********************************************************************************************************************
 * File Name    : ethos_u55_face_detection_ruhmi_example_app.h
 * Description  : Public interface for the Ethos-U55 Face Detection RUHMI example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ETHOS_U55_FACE_DETECTION_RUHMI_EXAMPLE_APP_H_
#define ETHOS_U55_FACE_DETECTION_RUHMI_EXAMPLE_APP_H_

#include "ethos_u55_face_detection_ruhmi_ep.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define EP_VERSION      "1.0"

#define MODULE_NAME     "ethosu"

#define BANNER_INFO     "\r\n*********************************************************************"\
                        "\r\n*   Renesas FSP Example Project for " MODULE_NAME " Module                   *"\
                        "\r\n*   Example Project Version %s                                     *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                *"\
                        "\r\n*********************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about " MODULE_NAME " module\r\n"

#define EP_INFO         "\r\nThis project showcases the AI inference capabilities of the Arm Ethos-U"\
                        "\r\nNeural Processing Unit (NPU) integrated into Renesas RA microcontrollers. It"\
                        "\r\nimplements a face detection application using a pre-trained model sourced from"\
                        "\r\nthe Arm ML-Zoo, optimized for the Ethos-U NPU via the Vela compiler."\
                        "\r\nInference is executed using TensorFlow Lite for Microcontrollers (TFLM). The"\
                        "\r\ninput image is supplied by the user as a statically defined array in C and"\
                        "\r\nthe application outputs bounding box coordinates of detected faces to the"\
                        "\r\nterminal.\r\n\r\n"

/* Single entry-point called in hal_entry.cpp */
void ethos_u55_ruhmi_example_app_run(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ETHOS_U55_FACE_DETECTION_RUHMI_EXAMPLE_APP_H_ */
