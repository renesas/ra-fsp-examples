/*
 * ONE_common.c
 *
 *
 */

// Common utilities

#ifndef ONE_COMMON_C
#define ONE_COMMON_C

// System-specific
// - none -

// For all systems
#include "ONE_common.h"


// --------------------------------------------------
// Common interfaces to RTOS specific functions
// --------------------------------------------------

void ONE_DelayMsec(uint32_t msec)
{
    vTaskDelay(msec);
}


#endif  // #ifndef ONE_COMMON_C
