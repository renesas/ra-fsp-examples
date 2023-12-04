/* websys.h
 *
 * Part of the Webio Open Source lightweight web server.
 *
 * Copyright (c) 2007 by John Bartas
 * Portions Copyright (C) 2010 Renesas Electronics Corporation.
 * All rights reserved.
 *
 * Use license: Modified from standard BSD license.
 * 
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation, advertising 
 * materials, Web server pages, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by John Bartas. The name "John Bartas" may not be used to 
 * endorse or promote products derived from this software without 
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef _WEBSYS_H_
#define _WEBSYS_H_    1

/* This file contains definitions intended for modification during porting */

/*********** Optional webio features - comment out to remove ***************/

#define WI_STDFILES  1     /* Use system "fopen" files */
#define WI_EMBFILES  1     /* Use embedded FS */
#define WI_THREAD    1     /* Drive webio with a thread rather than polling */

#if 0  // DEFAULTS

/*********** Webio sizes and limits ***************/
#define WI_RXBUFSIZE    1536  /* rxbuf[] total size */
#define WI_TXBUFSIZE    1400  /* txbuf[] section size */
#define WI_MAXURLSIZE   512   /* URL buffer size  */
#define WI_FSBUFSIZE    (1024 * 4) /* file read buffer size */
#define WI_PERSISTTMO   300   /* persistent connection timeout */
#define WI_LANG_BUFFER  64    /* Buffer for language string in get request */


#endif

/*********** Webio sizes and limits ***************/

#define WI_RXBUFSIZE    1536  /* rxbuf[] total size */
#define WI_TXBUFSIZE    1400  /* txbuf[] section size */
#define WI_MAXURLSIZE   512   /* URL buffer size  */
#define WI_FSBUFSIZE    (1024 * 4) /* file read buffer size */
#define WI_PERSISTTMO   300   /* persistent connection timeout */
#define WI_LANG_BUFFER  64    /* Buffer for language string in get request */

/*********** OS portability ***************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Map Webio heap routine to system's */
#define WI_MALLOC(size)     pvPortMalloc(size)
#define WI_FREE(mem)        vPortFree(mem)

/* Map Webio string routines to system's */
extern  int strnicmp(const char *pszS1, const char *pszS2, size_t stLength);
extern  int stricmp(const char *pszS1, const char *pszS2);
extern  int pathCompare(const char *pszS1, const char *pszS2);

/* The types used by Webio */
typedef unsigned char u_char;
typedef unsigned long u_long;

/* Variable cticks replaced with function call */
extern u_long cticks(void);

/* Define TPS (Ticks Per Second). If this is contained an another project with 
 * TPS defined (eg Buster) then use the external definition.
 */
#ifndef TPS
#define TPS 1000
#endif /* no TPS */

/*********** Network portability ***************/
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSconfig.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "semphr.h"
#include "queue.h"

typedef Socket_t socktype;

/* Map Webio socket routines to system's */
#define accept(a,b,c)           FreeRTOS_accept(a,b,c)
#define bind(a,b,c)             FreeRTOS_bind(a,b,c)
#define shutdown(a,b)           FreeRTOS_shutdown(a,b)
#define closesocket(s)          FreeRTOS_closesocket(s)
#define connect(a,b,c)          FreeRTOS_connect(a,b,c)

#define listen(a,b)             FreeRTOS_listen(a,b)
#define recv(a,b,c,d)           FreeRTOS_recv(a,b,c,d)
#define send(a,b,c,d)           FreeRTOS_send(a,b,c,d)
#define socket(a,b,c)           FreeRTOS_socket(a,b,c)

#define select(a,b)             FreeRTOS_select(a,b)

#define ioctlsocket(a,b,c)      FreeRTOS_ioctl(a,b,c)

/*********** File system mapping ***************/

#define  USE_EMFILES 1
#define  USE_SYSFILES 1

#include <stdio.h>

/*********** debug support **************/

#include <stdarg.h>
#include "Trace.h"
#ifndef _DEBUG_WEBIO_
#undef  TRACE
#define TRACE(_x_)
#endif
#define _ANSI_IO_
extern  void wsBreakPoint(void);
#define  dtrap()  wsBreakPoint()


#ifndef USE_ARG
#define USE_ARG(c) (c=c)
#endif  /* USE_ARG */

extern  void wsPanic(char *pszMessage);

#define panic wsPanic

extern u_long wi_totalblocks;

#endif   /* _WEBSYS_H_ */


