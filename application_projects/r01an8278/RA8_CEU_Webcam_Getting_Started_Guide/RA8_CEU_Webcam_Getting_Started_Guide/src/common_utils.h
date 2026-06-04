/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures, and functions commonly used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for the terminal interface */
#if (USE_VIRTUAL_COM == 1)
  #include "SERIAL_TERM/serial.h"
  #define TERM_BUFFER_SIZE              (SERIAL_RX_MAX_SIZE)
  #define TERM_INIT()                   (serial_init())
  #define TERM_PRINTF(fmt, ...)         (serial_printf((fmt), ##__VA_ARGS__))
  #define TERM_READ(buf, len)           (serial_read((buf), (len)))
  #define TERM_HAS_DATA()               (serial_has_data())
  #define TERM_HAS_KEY()                (serial_has_key())
  #define TERM_DEINIT()                 (serial_deinit())
#else
  #include "SEGGER_RTT/SEGGER_RTT.h"
  #define SEGGER_INDEX                  (0)
  #define TERM_BUFFER_SIZE              (BUFFER_SIZE_DOWN)
  #define TERM_INIT()                   (0)
  #define TERM_PRINTF(fmt, ...)         (SEGGER_RTT_printf(SEGGER_INDEX, (fmt), ##__VA_ARGS__))
  #define TERM_READ(buf, len)           (SEGGER_RTT_Read(SEGGER_INDEX, (buf), (len)))
  #define TERM_HAS_DATA()               (SEGGER_RTT_HasData(SEGGER_INDEX))
  #define TERM_HAS_KEY()                (SEGGER_RTT_HasKey())
  #define TERM_DEINIT()                 /* No de-initialization needed for SEGGER RTT */
#endif /* USE_VIRTUAL_COM */

/* Macros for terminal functionality in the RTOS project */
#if (BSP_CFG_RTOS != 0U)
  #if (BSP_CFG_RTOS == 1U)
    #define TERM_BYTE_POOL_SIZE         (4096U)
  #endif /* BSP_CFG_RTOS == 1U */
  #define TERM_OUTPUT_QUEUE_SIZE        (100U)
  #define TERM_INPUT_QUEUE_SIZE         (100U)
#endif /* BSP_CFG_RTOS != 0U */

/* Macros commonly used */
#define LVL_ERR                         (1U)       /* Error conditions */
#define RESET_VALUE                     (0x00)
#define NULL_CHAR                       ('\0')
#define MODULE_CLOSE                    (0U)
#define TERMINAL_BUFFER                 (64)

#define APP_PRINT(fn_, ...)             (TERM_PRINTF((fn_), ##__VA_ARGS__))

#if LVL_ERR
  #define APP_ERR_PRINT(fn_, ...)       (APP_PRINT("\r\n[ERR] In Function: %s(), %s", __FUNCTION__, \
                                                   (fn_), ##__VA_ARGS__))
#else
  #define APP_ERR_PRINT(fn_, ...)
#endif /* LVL_ERR */

#define APP_ERR_RET(err, fn_, ...)      ({\
		                                if (err)\
                                        {\
                                        APP_ERR_PRINT((fn_), ##__VA_ARGS__); \
                                        return (err); \
                                        }\
                                        })

#define ERROR_TRAP                      ({ \
                                        __asm("BKPT #0\n"); \
                                        })

#define APP_ERR_TRAP(err)               ({\
                                        if(err)\
                                        {\
                                        APP_PRINT("\r\nReturned Error Code: 0x%x  \r\n", (err));\
                                        TERM_DEINIT(); \
                                        /* Trap upon the error */ \
                                        ERROR_TRAP; \
                                        }\
                                        })

#define APP_READ(buf)                   (TERM_READ(buf, sizeof(buf)))

#define APP_CHECK_DATA                  (TERM_HAS_DATA())

#define APP_CHECK_KEY                   (TERM_HAS_KEY())


/* Network Events */
#define EVT_NET_READY                   (1U << 0)   /* Network initialized and IP obtained */

/* HTTP Server Events */
#define EVT_HTTP_READY                  (1U << 1)   /* HTTP server successfully initialized */

/* Camera Events */
#define EVT_CAM_READY                   (1U << 2)   /* Camera configured and ready to capture */
#define EVT_CAM_START                   (1U << 3)   /* Start camera capture */
#define EVT_CAM_STOP                    (1U << 4)   /* Stop camera capture */
#define EVT_DEMO_END                    (1U << 5)   /* Request to terminate the demo */

#endif /* COMMON_UTILS_H_ */
