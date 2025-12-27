/***********************************************************************************************************************
 * File Name    : ospi_b.h
 * Description  : Contains data structures and functions used in ospi_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OSPI_B_H_
#define OSPI_B_H_

#include "ospi_b_commands.h"
#include "emWin_thread.h"


/* Flash device timming */
#define OSPI_B_TIME_RESET_SETUP             (2U)             //  Type 50ns
#define OSPI_B_TIME_RESET_PULSE             (1000U)          //  Type 500us
#define OSPI_B_TIME_UNIT                    (BSP_DELAY_UNITS_MICROSECONDS)
/* Flash device status bit */
#define OSPI_B_WEN_BIT_MASK                 (0x00000002)
#define OSPI_B_BUSY_BIT_MASK                (0x00000001)

#define APP_ERR_TRAP(err)        if(err) { __asm("BKPT #0\n");} /* trap upon the error  */

/* function declare */
fsp_err_t ospi_b_init(void);
fsp_err_t ospi_b_write_enable (void);

#endif /* OSPI_B_H_ */
