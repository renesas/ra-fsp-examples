/*
 * ONE_common.h
 *
 *
 */

// Common utilities

#ifndef ONE_COMMON_H_
#define ONE_COMMON_H_

#ifdef EXTERN       // undef before re-defining
#undef EXTERN
#endif

#ifdef ONE_COMMON_C
    #define EXTERN
#else
    #define EXTERN extern
#endif

#include "common_utils.h"   // relies on other includes there




void     ONE_DelayMsec(uint32_t msec);


////----------------------------------------------------------------------
//// Define print functions based on local resources
////----------------------------------------------------------------------

extern fsp_err_t print_to_console(uint8_t * p_data);

EXTERN char g_print_buf[BUFFER_LINE_LENGTH];

#define ONE_PRINT(buf, ...)                         \
    {                                               \
        sprintf(g_print_buf, buf, ##__VA_ARGS__);   \
        print_to_console((uint8_t *)g_print_buf);   \
    }

#ifdef PRINT_AT_COMMANDS
#define ONE_PRINT_AT(buf, ...)                      \
    {                                               \
        sprintf(g_print_buf, buf, ##__VA_ARGS__);   \
        print_to_console((uint8_t *)g_print_buf);   \
    }
#else
#define ONE_PRINT_AT(buf, ...)                      \
    {                                               \
        print_to_console((uint8_t *)".");           \
    }

#endif


#define ONE_ERR_PRINT(buf, ...)                     \
    {                                               \
        sprintf(g_print_buf, buf, ##__VA_ARGS__);   \
        print_to_console((uint8_t *)g_print_buf);   \
    }


#endif /* ONE_COMMON_H_ */
