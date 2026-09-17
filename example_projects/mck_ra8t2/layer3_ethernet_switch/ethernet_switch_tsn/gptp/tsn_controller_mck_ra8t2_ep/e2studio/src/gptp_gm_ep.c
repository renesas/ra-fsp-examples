/***********************************************************************************************************************
 * File Name    : gptp_gm_ep.c
 * Description  : Grand Master ESWM initialization for RA8T2.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "gptp_gm_ep.h"

/***********************************************************************************************************************
 *  Function Name: gptp_gm_init
 *  Description  : Initializes the gPTP ESWM instance and claims the Grandmaster role by setting priority1 so this
 *                 node wins BMCA against every downstream node.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS   ESWM initialized and Grandmaster priority applied.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t gptp_gm_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Bring up the gPTP ESWM instance. */
    err = rm_gptp_eswm_init(&g_gptp_eswm0_ctrl, &g_gptp_eswm0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set priority1 for this board. */
    for (uint8_t i = 1; i <= NUMBER_OF_INSTANCES; i++)
    {
        set_priority_1(i, GPTP_ESWM_EXAMPLE_MASTER_PRIORITY);
    }

    return err;
}
/***********************************************************************************************************************
* End of function gptp_gm_init.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: gptp_service_tick
 *  Description  : Services the gPTP stack for one tick: keeps the link state current, processes one RX/TX/timer
 *                 cycle, and blocks until the next tick is due. Call this in a tight loop; it never returns early.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void gptp_service_tick(void)
{
    /* Poll link status. */
    gptp_link_process();

    gptp_read();
    gptp_timer_check();
    gptp_send();

    while (!(gptp_detect_count()))
    {
        ;
    }
}
/***********************************************************************************************************************
* End of function gptp_service_tick.
***********************************************************************************************************************/
