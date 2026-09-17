/***********************************************************************************************************************
 * File Name    : gptp_gm_example_app.c
 * Description  : Grand Master example — RA8T2 controller node.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <gptp_gm_example_app.h>
#include "gptp_gm_ep.h"
#include "sync_8021_as.h"
#include "data_sets.h"
#include "types.h"
#include "gptp_api.h"
#include "SEGGER_RTT.h"
#include "hw_adapt.h"

extern uint8_t                 g_sync_state[];
extern st_domain_information_t g_domains[];
extern uint8_t                 instance_using_hardware_clock;

#define STATUS_INTERVAL (5000U)  /* Limits status print frequency. */

/* Private functions declarations */
static void print_status(void);
static void print_banner(void);

/***********************************************************************************************************************
 *  Function Name: print_status
 *  Description  : Prints the Grandmaster's current role, timestamp and per-port link delay to the console.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
static void print_status(void)
{
    uint8_t  active_inst = instance_using_hardware_clock;
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

    /* Emit the status block. */
    SEGGER_RTT_printf(0, GPTP_STATUS_INFO,
           active_inst,
           seconds, nanoseconds / 1000000U,
           (SYNC_MASTER == g_sync_state[active_inst]) ? "MASTER" : "WAITING",
           0, p0_str,
           1, p1_str);
}
/***********************************************************************************************************************
* End of function print_status.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: gptp_example_app_run
 *  Description  : Single entry point of the Grandmaster application, called from hal_entry(). Brings up the console,
 *                 initializes the Grandmaster role, then services the gPTP stack forever, printing a status block
 *                 every few seconds. Never returns.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void gptp_example_app_run(void)
{
    fsp_err_t err        = FSP_SUCCESS;
    uint32_t  status_cnt = 0;

    print_banner();

    err = gptp_gm_init();
    if (FSP_SUCCESS != err)
    {
        SEGGER_RTT_printf(0,"gptp_gm_init failed, err=%d\r\n", err);
        __asm("BKPT #0\n");
    }
    SEGGER_RTT_printf(0,"GM: Grand Master (priority 50) - Start\r\n");

    /* Service the gPTP stack forever, printing a status block every few seconds. */
    while (1)
    {
        gptp_service_tick();

        if (++status_cnt >= STATUS_INTERVAL)
        {
            status_cnt = 0;
            print_status();
        }
    }
}
/***********************************************************************************************************************
* End of function gptp_example_app_run.
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
