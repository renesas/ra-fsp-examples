/***********************************************************************************************************************
 * File Name    : i3c_master_example_app.c
 * Description  : Example application layer for the I3C Master EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: i3c_master_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "i3c_master_example_app.h"
#include "console/uart_console.h"

/* Private function declarations */
static void i3c_master_operation(void);
static fsp_err_t check_disp_i3c_slave_info(void);
static void handle_error(fsp_err_t err, const char *err_str);

/***********************************************************************************************************************
 * Function Name: i3c_master_example_app_run.
 * Description  : Single entry-point for the I3C Master example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void i3c_master_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint32_t event_flag = RESET_VALUE;

    /* Initialize UART console */
    err = (fsp_err_t)uart_console_init();
    if (FSP_SUCCESS != err)
    {
        return;
    }

    /* Get FLEX version information */
    R_FSP_VersionGet(&version);
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize AGT timer */
    err = agt_init();
    handle_error(err, "agt_init failed.\r\n");

    /* Initialize I3C in Master mode */
    err = i3c_master_init();
    handle_error(err, "i3c_master_init failed.\r\n");
    printf("\r\nINFO: I3C initialized successfully in Master mode.");

    if (g_b_bus_initialized && g_num_device_on_bus)
    {
        printf("\r\nINFO: Bus configuration is completed successfully.\r\n");
        printf("INFO: Number of I3C device on bus: %d **\r\n", (unsigned int)g_num_device_on_bus);
    }
    else
    {
        printf("\r\nINFO: Sending CCC broadcast signal for Dynamic Address Assignment successfully.\r\n");
    }

    /* Print menu option */
    printf(EP_MENU);

    while (true)
    {
        /* Check for user input */
        if (uart_console_has_char())
        {
            i3c_master_operation();
        }

        /* Wait for I3C events */
        event_flag = RESET_VALUE;
        err = i3c_master_process_event(&event_flag);
        handle_error(err, "i3c_master_process_event failed.\r\n");

        /* Check if Hot-Join was processed */
        if (g_b_hot_join_done)
        {
            printf("\r\nINFO: A Hot-Join event is received, initiate DAA using CCC transmission.");
            printf("\r\nINFO: DAA using CCC transmission completed, please check by pressing user"
                   "\r\ninput 1 (available at menu option) for Slave information\r\n");
            g_b_hot_join_done = false;
        }

        /* Check if IBI transfer was received */
        if ((event_flag & I3C_EVENT_FLAG_IBI_READ_COMPLETE) && g_b_ibi_transfer_received)
        {
            printf("\r\nINFO: A Slave IBI transfer is received.");
            printf("\r\nINFO: Target address:0x%02x, IBI Payload size: 0x%x\r\n",
                   g_ibi_target_address, (unsigned int)g_data_transfer_size);
            g_b_ibi_transfer_received = false;
        }
    }
}
/***********************************************************************************************************************
 * End of function i3c_master_example_app_run
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: i3c_master_operation.
 * Description  : This function reads user input and executes the corresponding I3C Master operation.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void i3c_master_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t user_option = (uint8_t)(getchar() - '0');

    switch (user_option)
    {
        case DISPLAY_I3C_SLAVE_INFO:
        {
            /* Display Slave information */
            err = check_disp_i3c_slave_info();
            handle_error(err, "check_disp_i3c_slave_info failed.\r\n");
            break;
        }

        case MASTER_WRITE_READ:
        {
            /* Perform Master write read operation */
            err = master_write_read_verify();
            handle_error(err, "master_write_read_verify failed.\r\n");
            printf("\r\nINFO: Data written to I3C Slave is read back and matching - SUCCESS\r\n");
            printf("INFO: Data Transfer size 0x%x\r\n", (unsigned int)g_data_transfer_size);
            break;
        }

#ifdef I3C_HDR_DDR_SUPPORT
        case MASTER_WRITE_READ_HDR:
        {
            if (I3C_HDR_DDR_MODE == g_device_slave_info.slave_info.bcr_b.hdr_capable)
            {
                /* Perform Master write read operation in HDR-DDR */
                err = master_write_read_hdr_verify();
                handle_error(err, "master_write_read_hdr_verify failed.\r\n");
                printf("\r\nINFO: [HDR-DDR] Data written to I3C Slave is read back and matching - SUCCESS\r\n");
                printf("INFO: [HDR-DDR] Data Transfer size 0x%x\r\n", (unsigned int)g_data_transfer_size);
            }
            else
            {
                printf("\r\nThis Slave has not been enabled or not supported I3C HDR-DDR mode\r\n");
            }
            break;
        }
#endif /* I3C_HDR_DDR_SUPPORT */

        default:
        {
            printf("\r\nInvalid input\r\n");
            break;
        }
    }

    printf(EP_MENU);
}
/***********************************************************************************************************************
 * End of function i3c_master_operation
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: check_disp_i3c_slave_info.
 * Description  : This function checks the Slave device is present on bus and displays Slave device information.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Operated successfully.
 *                Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t check_disp_i3c_slave_info(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Check for Slave device existence on the bus */
    if ((RESET_VALUE == g_num_device_on_bus) && (RESET_VALUE == g_device_slave_info.dynamic_address))
    {
        printf("\r\nNo Slave device exists on I3C bus,\r\n"
               "Sending broadcast Common Command Code to check for Slave Hot-Join requests\r\n"
               "Please re-check again\r\n");

        /* Send broadcast to reset dynamic address for all Slave devices */
        err = i3c_ccc_reset_address_send();
        if (FSP_SUCCESS != err)
        {
            printf("\r\nNo Slave device exists on I3C bus, please confirm on physical connection on bus.\r\n");
            return err;
        }
        printf("\r\nINFO: CCC Reset Dynamic Address Assignment transfer completed successfully.");
        printf("\r\nINFO: Please re-check with menu option 1 to view Dynamic Address assigned "
               "to Slave before using menu option 2\r\n");
    }
    else
    {
        printf("\r\nINFO: Number of I3C device on bus: %d **\r\n\n", (unsigned int)g_num_device_on_bus);
        printf("***********************************************\r\n");
        printf("*                 I3C Slave Info              *\r\n");
        printf("***********************************************\r\n");
        printf("- Dynamic Address:   0x%02x\r\n", g_device_slave_info.dynamic_address);
        printf("- BCR:               0x%02x\r\n", g_device_slave_info.slave_info.bcr);
        printf("- DCR:               0x%02x\r\n", g_device_slave_info.slave_info.dcr);
        printf("- PID:               0x%02x%02x%02x%02x%02x%02x\r\n",
               g_device_slave_info.slave_info.pid[0],
               g_device_slave_info.slave_info.pid[1],
               g_device_slave_info.slave_info.pid[2],
               g_device_slave_info.slave_info.pid[3],
               g_device_slave_info.slave_info.pid[4],
               g_device_slave_info.slave_info.pid[5]);
    }

    return err;
}
/***********************************************************************************************************************
 * End of function check_disp_i3c_slave_info
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error.
 * Description  : This function handles an error by printing error message and code, de-initializing the related
 *                modules, and entering a trap state.
 * Arguments    : err                       Error code.
 *                err_str                   Error message.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        i3c_deinit();
        agt_deinit();
        __BKPT(0);
    }
}
/***********************************************************************************************************************
 * End of function handle_error
 **********************************************************************************************************************/
