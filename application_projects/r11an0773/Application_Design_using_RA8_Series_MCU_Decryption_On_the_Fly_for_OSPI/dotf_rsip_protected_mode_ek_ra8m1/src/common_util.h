/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/


#define APP_ERROR_TRAP(err)        if(err) { __asm("BKPT #0\n");} /* system execution breaks  */

#define CPU_CLOCK	R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_CPUCLK)
#define MULTIPICATION_FACTOR_TO_NANOSECOND	(1000)
#define SCALE_DWT_TO_MS	(CPU_CLOCK/CONVERT_SEC_TO_MILLISEC)

/** Common error codes */
typedef enum e_app_err
{
    APP_SUCCESS 				    = 0,

    APP_OSPI_OPEN                   = 1,                    ///< OSPI open API failed
    APP_OSPI_DIRECT_TRANSFER        = 2,                    ///< OSPI direct transfer API failed
    APP_OSPI_SPI_PROTOCOL_SET       = 3,                    ///< OSPI protocol set failed
    APP_FIBONACCI_CALCULATION       = 4,                    ///< fibonacci calculation failed
    APP_OSPI_OPERATION_TIMEOUT      = 5,                    ///< get OSPI status failed
    APP_OSPI_DOTF                   = 6,                    ///< OSIP DOTF operation failed
    APP_JLINK_CONSOLE_INIT          = 7,                    ///< jlink console init failed
	APP_JLINK_CONSOLE_OPEN		    = 8,					///< jlink uart open error
	APP_JLINK_SONSOLE_WRITE	        = 9,					///< JLink console write failed
	APP_JLINK_SONSOLE_READ		    = 10,					///< JLink console read failed

} app_err_t;


/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

extern char sprintf_buffer[];

/**********************************************************************************************************************
 * Global Function Prototypes
 **********************************************************************************************************************/

extern void handle_error (uint32_t err, app_err_t err_enum);

#endif /* COMMON_UTIL_H_ */
