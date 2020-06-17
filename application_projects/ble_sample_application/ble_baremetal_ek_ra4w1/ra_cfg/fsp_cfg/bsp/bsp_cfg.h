/* generated configuration header file - do not edit */
#ifndef BSP_CFG_H_
#define BSP_CFG_H_
#include "bsp_clock_cfg.h"
#include "bsp_mcu_family_cfg.h"
#include "board_cfg.h"
#define RA_NOT_DEFINED 0
#ifndef BSP_CFG_RTOS
#if (RA_NOT_DEFINED) != (RA_NOT_DEFINED)
#define BSP_CFG_RTOS (2)
#elif (RA_NOT_DEFINED) != (RA_NOT_DEFINED)
#define BSP_CFG_RTOS (1)
#else
#define BSP_CFG_RTOS (0)
#endif
#endif
#undef RA_NOT_DEFINED
#define BSP_CFG_MCU_VCC_MV (3300)
#define BSP_CFG_STACK_MAIN_BYTES (0x2000)
#define BSP_CFG_HEAP_BYTES (0x2000)
#define BSP_CFG_PARAM_CHECKING_ENABLE (0)
#define BSP_CFG_ASSERT (0)
#define BSP_CFG_ERROR_LOG (0)

#define BSP_CFG_PFS_PROTECT ((1))

#define BSP_CFG_SOFT_RESET_SUPPORTED ((0))

#ifndef BSP_CLOCK_CFG_MAIN_OSC_POPULATED
#define BSP_CLOCK_CFG_MAIN_OSC_POPULATED (1)
#endif
#ifndef BSP_CLOCK_CFG_MAIN_OSC_WAIT
#define BSP_CLOCK_CFG_MAIN_OSC_WAIT (9)
#endif
#ifndef BSP_CLOCK_CFG_MAIN_OSC_CLOCK_SOURCE
#define BSP_CLOCK_CFG_MAIN_OSC_CLOCK_SOURCE (0)
#endif
#ifndef BSP_CLOCK_CFG_SUBCLOCK_DRIVE
#define BSP_CLOCK_CFG_SUBCLOCK_DRIVE (0)
#endif
#ifndef BSP_CLOCK_CFG_SUBCLOCK_POPULATED
#define BSP_CLOCK_CFG_SUBCLOCK_POPULATED (1)
#endif
#ifndef BSP_CLOCK_CFG_SUBCLOCK_STABILIZATION_MS
#define BSP_CLOCK_CFG_SUBCLOCK_STABILIZATION_MS 1000
#endif

/*
 ID Code
 Note: To permanently lock and disable the debug interface define the BSP_ID_CODE_PERMANENTLY_LOCKED in the compiler settings.
 WARNING: This will disable debug access to the part and cannot be reversed by a debug probe.
 */
#if defined(BSP_ID_CODE_PERMANENTLY_LOCKED)
#define BSP_CFG_ID_CODE_LONG_1 (0x00000000)
#define BSP_CFG_ID_CODE_LONG_2 (0x00000000)
#define BSP_CFG_ID_CODE_LONG_3 (0x00000000)
#define BSP_CFG_ID_CODE_LONG_4 (0x00000000)
#else
/* ID CODE: FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF */
#define BSP_CFG_ID_CODE_LONG_1 (0xFFFFFFFF)
#define BSP_CFG_ID_CODE_LONG_2 (0xFFFFFFFF)
#define BSP_CFG_ID_CODE_LONG_3 (0xFFFFFFFF)
#define BSP_CFG_ID_CODE_LONG_4 (0xffFFFFFF)
#endif
#endif /* BSP_CFG_H_ */
