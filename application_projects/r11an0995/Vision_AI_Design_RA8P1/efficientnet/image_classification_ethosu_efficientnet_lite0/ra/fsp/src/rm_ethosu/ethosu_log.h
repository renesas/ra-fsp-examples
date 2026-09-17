/* ${REA_DISCLAIMER_PLACEHOLDER} */

#ifndef RM_ETHOSU_LOG_H
#define RM_ETHOSU_LOG_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>

/******************************************************************************
 * Defines
 ******************************************************************************/

// Log severity levels
#define RM_ETHOSU_LOG_DISABLED    0
#define RM_ETHOSU_LOG_ERR         1
#define RM_ETHOSU_LOG_WARN        2
#define RM_ETHOSU_LOG_INFO        3
#define RM_ETHOSU_LOG_DEBUG       4

#include "rm_ethosu_cfg.h"

// Define default log severity
#ifndef RM_ETHOSU_LOG_SEVERITY
 #define RM_ETHOSU_LOG_SEVERITY    RM_ETHOSU_LOG_DISABLED
#endif

#if RM_ETHOSU_LOG_SEVERITY >= RM_ETHOSU_LOG_ERR
 #define LOG(f, ...)    (void) printf(f, ## __VA_ARGS__)
#else
 #define LOG(f, ...)
#endif

#if RM_ETHOSU_LOG_SEVERITY >= RM_ETHOSU_LOG_ERR
 #define LOG_ERR(f, ...)    (void) printf("Err: " f "\n", ## __VA_ARGS__)
#else
 #define LOG_ERR(f, ...)
#endif

#if RM_ETHOSU_LOG_SEVERITY >= RM_ETHOSU_LOG_WARN
 #define LOG_WARN(f, ...)    (void) printf("Warn: " f "\n", ## __VA_ARGS__)
#else
 #define LOG_WARN(f, ...)
#endif

#if RM_ETHOSU_LOG_SEVERITY >= RM_ETHOSU_LOG_INFO
 #define LOG_INFO(f, ...)    (void) printf("Info: " f "\n", ## __VA_ARGS__)
#else
 #define LOG_INFO(f, ...)
#endif

#if RM_ETHOSU_LOG_SEVERITY >= RM_ETHOSU_LOG_DEBUG
 #define LOG_DEBUG(f, ...)    (void) printf("Debug: " f "\n", ## __VA_ARGS__)
#else
 #define LOG_DEBUG(f, ...)
#endif

#endif                                 // RM_ETHOSU_LOG_H
