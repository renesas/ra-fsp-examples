/***********************************************************************************************************************
 * File Name    : ospi_b_control.h
 * Description  : Contains data structures and functions used in ospi_b_control.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OSPI_B_CONTROL_H_
#define OSPI_B_CONTROL_H_

#include <ospi_flash_commands.h>
#include "Application.h"

/* Flash device sector size */
#define OSPI_B_SECTOR_SIZE_4K               (0x1000)

/* Flash device timing */
#define OSPI_B_TIME_UNIT                    (BSP_DELAY_UNITS_MICROSECONDS)
#define OSPI_B_TIME_RESET_SETUP             (2U)             /*  Type 50ns  */
#define OSPI_B_TIME_RESET_PULSE             (1000U)          /*  Type 500us */
#define OSPI_B_TIME_ERASE_4K                (80000U)         /*  Type 80ms  */
#define OSPI_B_TIME_WRITE                   (80U)            /*  Type 0.08ms*/

/* Flash device status bit */
#define OSPI_B_WEN_BIT_MASK                 (0x00000002)

/* return err  */
#define APP_ERR_RETURN(err)        if(err) {return err;}

/* Function declarations */
fsp_err_t ospi_b_init(void);
fsp_err_t ospi_b_set_protocol_to_spi(void);
fsp_err_t ospi_b_set_protocol_to_opi(void);
fsp_err_t xip_enter(void);
fsp_err_t xip_exit_res(void);

#endif /* OSPI_B_CONTROL_H_ */
