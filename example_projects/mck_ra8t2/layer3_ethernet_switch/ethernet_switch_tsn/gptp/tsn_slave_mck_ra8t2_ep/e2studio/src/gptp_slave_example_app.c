/***********************************************************************************************************************
 * File Name    : gptp_slave_example_app.c
 * Description  : Slave example application layer for the gPTP EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <gptp_slave_ep.h>
#include "hw_adapt.h"
#include <gptp_slave_example_app.h>
#include "SEGGER_RTT.h"
#include "sync_8021_as.h"
#include "data_sets.h"
#include "bmca.h"

extern void get_clock_identity(uint8_t instance, st_clock_identity_t * p_clock_identity);

extern uint8_t                           g_sync_state[];
extern st_domain_information_t           g_domains[];
extern uint8_t                           instance_using_hardware_clock;
extern st_master_selection_information_t g_master_selection[];

#define STATUS_INTERVAL (5000U)  /* Limits status print frequency. */

/* Private functions declarations */
static void handle_error(fsp_err_t err, const char * err_str);
static void print_gptp_status(void);
static void print_banner(void);

/***********************************************************************************************************************
 *  Function Name: gptp_example_app_run
 *  Description  : Single entry point of the Slave application, called from hal_entry(). Brings up the console,
 *                 initializes the Slave role, then services the gPTP stack forever, printing a status block
 *                 every few seconds. Never returns.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void gptp_example_app_run(void)
{
    fsp_err_t err        = FSP_SUCCESS;
    uint32_t  status_cnt = 0;

    print_banner();

    err = gptp_init();
    handle_error(err, "gptp_init failed\r\n");

    SEGGER_RTT_printf(0, "Start gPTP Example\r\n");

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
 *  Description  : Prints the Slave's clock identity, role, time offset and link delay to the console.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
static void print_gptp_status(void)
{
    time_interval_t offset      = 0;
    int64_t         offset_ns   = 0;
    int64_t         delay_p0    = 0;
    int64_t         delay_p1    = 0;
    uint32_t        seconds     = 0;
    uint32_t        nanoseconds = 0;
    /* instance_using_hardware_clock always points to the instance driving the HW clock */
    uint8_t active_inst = instance_using_hardware_clock;

    read_time_value(0, &seconds, &nanoseconds);

    if (NULL != g_domains[0].per_port[0].global_port_parameters)
    {
        delay_p0 = (int64_t) g_domains[0].per_port[0].global_port_parameters->mean_link_delay;
    }
    if (NULL != g_domains[0].per_port[1].global_port_parameters)
    {
        delay_p1 = (int64_t) g_domains[0].per_port[1].global_port_parameters->mean_link_delay;
    }

    uint8_t p0_ok = (IS_AS_CAPABLE == g_domains[0].per_port[0].as_capable);
    uint8_t p1_ok = (IS_AS_CAPABLE == g_domains[0].per_port[1].as_capable);

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

    st_clock_identity_t gm_clk_id;
    read_grandmaster_identity(0, &gm_clk_id);

    char role_str[8]    = {0};
    char offset_str[16] = {0};
    char hops_str[16]   = {0};

    switch (g_sync_state[active_inst])
    {
        case SYNC_SLAVE:
        {
            read_offset_from_master(active_inst, &offset);
            offset_ns = offset / 65536;
            (void) snprintf(role_str, sizeof(role_str), "SLAVE");
            (void) snprintf(offset_str, sizeof(offset_str), "%s%u ns",
                             (offset_ns < 0) ? "-" : "+",
                             (uint32_t) ((offset_ns < 0) ? -offset_ns : offset_ns));
            (void) snprintf(hops_str, sizeof(hops_str), "%u", g_master_selection[active_inst].master_steps_removed);
            break;
        }

        default:
        {
            (void) snprintf(role_str, sizeof(role_str), "WAITING");
            (void) snprintf(offset_str, sizeof(offset_str), "--");
            (void) snprintf(hops_str, sizeof(hops_str), "--");
            break;
        }
    }

    SEGGER_RTT_printf(0, GPTP_STATUS_INFO,
           active_inst,
           gm_clk_id.element[0], gm_clk_id.element[1],
           gm_clk_id.element[2], gm_clk_id.element[3],
           gm_clk_id.element[4], gm_clk_id.element[5],
           gm_clk_id.element[6], gm_clk_id.element[7],
           seconds, nanoseconds / 1000000U,
           role_str,
           0, p0_str,
           1, p1_str,
           hops_str, offset_str);
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
 *  Description  : Prints the fixed EP banner and info text over SEGGER RTT.
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
