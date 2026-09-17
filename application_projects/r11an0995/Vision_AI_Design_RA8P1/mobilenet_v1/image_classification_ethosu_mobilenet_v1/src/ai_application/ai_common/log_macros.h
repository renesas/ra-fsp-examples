#ifndef ML_EMBEDDED_CORE_LOG_H
#define ML_EMBEDDED_CORE_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

extern int e_printf(const char *format, ...);
extern void log_caller(unsigned short index, unsigned short  x, unsigned short  y, unsigned short  w, unsigned short  h);

#define LOG_LEVEL_TRACE       0
#define LOG_LEVEL_DEBUG       1
#define LOG_LEVEL_INFO        2
#define LOG_LEVEL_WARN        3
#define LOG_LEVEL_ERROR       4

#ifndef LOG_LEVEL
#define LOG_LEVEL             LOG_LEVEL_ERROR
#endif /*LOG_LEVEL*/

#define UNUSED(x)       ((void)(x))

#if (LOG_LEVEL == LOG_LEVEL_TRACE)
    #define trace(...)        {e_printf("TRACE - "); e_printf(__VA_ARGS__); e_printf("\r\n");}
#else
    #define trace(...)
#endif  /* LOG_LEVEL == LOG_LEVEL_TRACE */

#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
    #define debug(...)        {e_printf("DEBUG - "); e_printf(__VA_ARGS__); e_printf("\r\n");}
#else
    #define debug(...)
#endif  /* LOG_LEVEL > LOG_LEVEL_TRACE */

#if (LOG_LEVEL <= LOG_LEVEL_INFO)
    #define info(...)         {e_printf("INFO - "); e_printf(__VA_ARGS__); e_printf("\r\n");}
#else
    #define info(...)
#endif  /* LOG_LEVEL > LOG_LEVEL_DEBUG */

#if (LOG_LEVEL <= LOG_LEVEL_WARN)
    #define warn(...)         {e_printf("WARN - "); e_printf(__VA_ARGS__); e_printf("\r\n");}
#else
    #define warn(...)
#endif  /* LOG_LEVEL > LOG_LEVEL_INFO */

#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
    #define error(...)        {e_printf("ERROR - "); e_printf(__VA_ARGS__); e_printf("\r\n");}
#else
    #define error(...)
#endif  /* LOG_LEVEL > LOG_LEVEL_INFO */

#ifdef __cplusplus
}
#endif

#endif /* ML_EMBEDDED_CORE_LOG_H */
