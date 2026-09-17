/***********************************************************************************************************************
 * File Name    : gptp_bridge_example_app.c
 * Description  : Bridge example — RA8T2 ESWM.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "SEGGER_RTT.h"
#include "gptp_bridge_ep.h"
#include "gptp_bridge_example_app.h"
#include "sync_8021_as.h"
#include "data_sets.h"
#include "hw_adapt.h"
#include "types.h"
#include "bmca.h"

extern st_domain_information_t           g_domains[];
extern st_master_selection_information_t g_master_selection[];

#define STATUS_INTERVAL (5000U)  /* Limits status print frequency. */

/* Private functions declarations */
static void handle_error(fsp_err_t err, const char * err_str);
static void print_gptp_status(void);
static void print_banner(void);

/***********************************************************************************************************************
 *  Function Name: gptp_example_app_run
 *  Description  : Single entry point of the Bridge application, called from hal_entry(). Brings up the console,
 *                 initializes the bridge role, then services the gPTP stack forever, printing a status
 *                 block every few seconds. Never returns.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void gptp_example_app_run(void)
{
    fsp_err_t err        = FSP_SUCCESS;
    uint32_t  status_cnt = 0;

    print_banner();

    err = gptp_bridge_init();
    handle_error(err, "gptp_bridge_init failed\r\n");
    SEGGER_RTT_printf(0, "Bridge: Port %u=SLAVE (upstream GM)  Port %u=MASTER (downstream)\r\n",
           GPTP_BRIDGE_SLAVE_PORT, GPTP_BRIDGE_MASTER_PORT);

    /* Service the gPTP stack forever, printing a status block every few seconds. */
    while (1)
    {
        gptp_service_tick();

        if (++status_cnt >= STATUS_INTERVAL)
        {
            status_cnt = 0;
            print_gptp_status();
        }
    }
}
/***********************************************************************************************************************
* End of function gptp_example_app_run.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: print_gptp_status
 *  Description  : Prints the bridge's per-port sync status and time offset to the console.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
static void print_gptp_status(void)
{
    uint32_t seconds     = 0;
    uint32_t nanoseconds = 0;
    int64_t  delay_p0    = 0;
    int64_t  delay_p1    = 0;

    /* Read the current gPTP time. */
    read_time_value(0, &seconds, &nanoseconds);

    /* Take the mean link delay on each port, if it has been measured yet. */
    if (NULL != g_domains[0].per_port[0].global_port_parameters)
    {
        delay_p0 = (int64_t) g_domains[0].per_port[0].global_port_parameters->mean_link_delay;
    }
    if (NULL != g_domains[0].per_port[1].global_port_parameters)
    {
        delay_p1 = (int64_t) g_domains[0].per_port[1].global_port_parameters->mean_link_delay;
    }

    uint8_t p0_ok = (IS_AS_CAPABLE == g_domains[0].per_port[GPTP_BRIDGE_SLAVE_PORT].as_capable);
    uint8_t p1_ok = (IS_AS_CAPABLE == g_domains[0].per_port[GPTP_BRIDGE_MASTER_PORT].as_capable);

    char p0_str[32] = {0};
    char p1_str[32] = {0};
    if (p0_ok)
    {
        (void) snprintf(p0_str, sizeof(p0_str), "Link Delay=%u ns", (uint32_t) delay_p0);
    }
    else
    {
        (void) snprintf(p0_str, sizeof(p0_str), "Link Down");
    }
    if (p1_ok)
    {
        (void) snprintf(p1_str, sizeof(p1_str), "Link Delay=%u ns", (uint32_t) delay_p1);
    }
    else
    {
        (void) snprintf(p1_str, sizeof(p1_str), "Link Down");
    }

    time_interval_t raw_offset = 0;
    read_offset_from_master(0, &raw_offset);
    int64_t offset_ns = raw_offset / 65536;

    st_clock_identity_t gm_clk_id;
    read_grandmaster_identity(0, &gm_clk_id);

    /* Emit the status block. */
    SEGGER_RTT_printf(0, GPTP_STATUS_INFO,
           gm_clk_id.element[0], gm_clk_id.element[1],
           gm_clk_id.element[2], gm_clk_id.element[3],
           gm_clk_id.element[4], gm_clk_id.element[5],
           gm_clk_id.element[6], gm_clk_id.element[7],
           seconds, nanoseconds / 1000000U,
           GPTP_BRIDGE_SLAVE_PORT,  p0_str,
           GPTP_BRIDGE_MASTER_PORT, p1_str,
           offset_ns < 0 ? "-" : "+",
           (uint32_t)(offset_ns < 0 ? -offset_ns : offset_ns));
}
/***********************************************************************************************************************
* End of function print_gptp_status.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: handle_error
 *  Description  : Prints the failing call's message and error code, then traps execution.
 *  Arguments    : err           The FSP error code to check.
 *                 err_str       Message identifying the failing call, printed on error.
 *  Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        SEGGER_RTT_printf(0, "%s", err_str);
        SEGGER_RTT_printf(0, "Returned Error Code: 0x%x\r\n", (unsigned int) err);
        __asm("BKPT #0\n");
    }
}
/***********************************************************************************************************************
* End of function handle_error.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: print_banner
 *  Description  : Prints a banner whose border width follows the longest heading line and wraps EP_INFO to that width.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
static void print_banner(void)
{
    SEGGER_RTT_printf(0, BANNER_INFO, EP_VERSION,
                    (unsigned int) FSP_VERSION_MAJOR,
                    (unsigned int) FSP_VERSION_MINOR,
                    (unsigned int) FSP_VERSION_PATCH);

    SEGGER_RTT_printf(0, EP_INFO);
}
/***********************************************************************************************************************
* End of function print_banner.
***********************************************************************************************************************/
