/***********************************************************************************************************************
 * File Name    : ethos_u55_face_detection_example_app.c
 * Description  : Example application layer for the Ethos-U55 Face Detection EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.cpp calls a single function: ethos_u55_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "ethos_u55_face_detection_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Ethos-U55 Face Detection example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ethos_u55_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Print the EP banner on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

#ifdef ARM_NPU
    /* Initialize Ethos-U NPU */
    err = ethos_u55_npu_open();
    handle_error(err, "ethos_u55_npu_open failed\r\n");
#endif /* ARM_NPU */

#if (INFERENCE_TIME_MEASUREMENT_ENABLED == 1)
    /* Initialize the timer */
    err = ethos_u55_timer_open();
    handle_error(err, "ethos_u55_timer_open failed\r\n");

    /* Start the timer */
    err = ethos_u55_timer_start();
    handle_error(err, "ethos_u55_timer_start failed\r\n");
#endif /* INFERENCE_TIME_MEASUREMENT_ENABLED */

    /* Enter main face detection operation */
    MainLoop();

    /* De-initialize all the opened modules */
    ethos_u55_deinit();
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int) err);
        uart_console_flush();
        ethos_u55_deinit();
        __BKPT(0);
    }
}
