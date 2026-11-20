/***********************************************************************************************************************
 * File Name    : i3c_slave_example_app.c
 * Description  : Example application layer for the I3C Slave EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: i3c_slave_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "i3c_slave_example_app.h"
#include "console/uart_console.h"

/* Private function declarations */
static void i3c_slave_print_event_info(uint32_t p_event_flag);
static void handle_error(fsp_err_t err, const char * err_str);

/***********************************************************************************************************************
 * Function Name: i3c_slave_example_app_run.
 * Description  : Single entry-point for the I3C Slave example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void i3c_slave_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint32_t event_flag = RESET_VALUE;

    /* Initialize UART console */
    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize AGT timer */
    err = agt_init();
    handle_error(err, "agt_init failed.\r\n");

    /* Initialize ICU module */
    err = icu_init();
    handle_error(err, "icu_init failed.\r\n");

    /* Initialize I3C Slave */
    err = i3c_slave_init();
    handle_error(err, "i3c_slave_init failed.\r\n");

    printf("\r\nINFO: I3C initialized successfully in Slave mode.\r\n");
    printf("I3C device information:\r\n");
    printf(" - Static Address: 0x%02x\r\n", g_slave_device_cfg.static_address);
    printf(" - BCR:            0x%02x\r\n", g_slave_device_cfg.slave_info.bcr);
    printf(" - DCR:            0x%02x\r\n", g_slave_device_cfg.slave_info.dcr);
    printf(" - PID:            0x%02x%02x%02x%02x%02x%02x\r\n",
           g_slave_device_cfg.slave_info.pid[0],
           g_slave_device_cfg.slave_info.pid[1],
           g_slave_device_cfg.slave_info.pid[2],
           g_slave_device_cfg.slave_info.pid[3],
           g_slave_device_cfg.slave_info.pid[4],
           g_slave_device_cfg.slave_info.pid[5]);

    printf("\r\nINFO: I3C device is ready and waiting for DAA.");

    /* Wait for the bus initialization (DAA) */
    err = i3c_device_daa_participation();
    handle_error(err, "i3c_device_daa_participation failed.\r\n");

    printf("\r\nINFO: Address assignment completed, Dynamic Address: 0x%02x\r\n",
           (unsigned int)g_slave_dynamic_address);

    while (true)
    {
        event_flag = RESET_VALUE;

        /* Perform I3C Slave operation */
        err = i3c_slave_process_event(&event_flag);
        handle_error(err, "i3c_slave_process_event failed.\r\n");

        i3c_slave_print_event_info(event_flag);
    }
}
/***********************************************************************************************************************
 * End of function i3c_slave_example_app_run
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_slave_print_event_info.
 * Description  : This function prints information related to I3C Slave events and transfer status.
 * Arguments    : p_event_flag              Pointer to store detected I3C event flags.
 * Return Value : None.
 **********************************************************************************************************************/
static void i3c_slave_print_event_info(uint32_t p_event_flag)
{
    /* Snapshot variables for UART prints */
    uint8_t ccc_code = g_slave_received_ccc_code;
    uint32_t transfer_size = g_data_transfer_size;

    if (p_event_flag & I3C_EVENT_FLAG_COMMAND_COMPLETE)
    {
        printf("\r\nINFO: Received CCC: 0x%x\r\n", (unsigned int)ccc_code);
        if (transfer_size)
        {
            printf("INFO: Received CCC payload size: 0x%x", (unsigned int)transfer_size);
        }

        if (I3C_CCC_BROADCAST_RSTDAA == ccc_code)
        {
            printf("\r\nINFO: Slave Dynamic Address is reset.\r\n");
            printf("\r\nINFO: Slave Dynamic Address is assigned.\r\n");
        }

#ifdef I3C_HDR_DDR_SUPPORT
        if (I3C_HDR_COMMAND_CODE_WRITE == ccc_code)
        {
            printf("\r\nINFO: [HDR-DDR] Write complete, transfer size: 0x%x\r\n", (unsigned int)transfer_size);
        }
        else if (I3C_HDR_COMMAND_CODE_READ == ccc_code)
        {
            printf("\r\nINFO: [HDR-DDR] Read complete, transfer size: 0x%x\r\n", (unsigned int)transfer_size);
        }
        else
        {
            /* Do nothing */
        }
#endif /* I3C_HDR_DDR_SUPPORT */
    }

    if (p_event_flag & I3C_EVENT_FLAG_WRITE_COMPLETE)
    {
        printf("\r\nINFO: Write complete, transfer size: 0x%x\r\n", (unsigned int)transfer_size);
    }

    if (p_event_flag & I3C_EVENT_FLAG_READ_COMPLETE)
    {
        printf("\r\nINFO: Read complete, transfer size: 0x%x", (unsigned int)transfer_size);
    }

    if (p_event_flag & I3C_EVENT_FLAG_IBI_WRITE_COMPLETE)
    {
        printf("\r\nINFO: IBI write complete, transfer size: 0x%x\r\n", (unsigned int)transfer_size);
    }

    if (g_b_ibi_write_sent)
    {
        printf("\r\nINFO: User push button pressed.");
        printf("\r\nINFO: IBI transfer request initiated successfully.");
        g_b_ibi_write_sent = false;
    }
}
/***********************************************************************************************************************
 * End of function i3c_slave_print_event_info
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error.
 * Description  : This function handles an error by printing error message and code, de-initializing the related
 *                modules, and entering a trap state.
 * Arguments    : err                       Error code.
 *                err_str                   Error message.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        i3c_deinit();
        agt_deinit();
        icu_deinit();
        __BKPT(0);
    }
}
/***********************************************************************************************************************
 * End of function handle_error
 **********************************************************************************************************************/
