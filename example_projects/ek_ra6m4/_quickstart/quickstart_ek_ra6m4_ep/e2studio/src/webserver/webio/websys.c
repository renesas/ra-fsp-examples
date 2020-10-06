/* websys.c
 *
 * Part of the Webio Open Source lightweight web server.
 *
 * Copyright (c) 2007 by John Bartas
 * Portions copyright Renesas Electronics Corporation
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

#include <fcntl.h>
#include <unistd.h>

#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "queue.h"

#include "websys.h"     /* port dependant system files */
#include "webio.h"
#include <ctype.h>
//#include "command.h"

/* This file contains the routines which change from OS to OS 
 * These are:
 *
 * wi_getdate - to format a string with the time and date
 * strnicmp - case insensitive string compare with length parameter
 * stricmp - case insensitive string compare
 * cticks - Function to return the tick count
 */

static char gpszDate[64] = "00";

/*****************************************************************************
Function Name: wi_getdate
Description:   Function to format a date string
Arguments:     IN  sess - Pointer to the web interface session data
Return value:  Pointer to a 
*****************************************************************************/
char * wi_getdate(wi_sess * sess)
{
    (void)sess;

    return gpszDate;
}
/*****************************************************************************
End of function  wi_getdate
******************************************************************************/

/*****************************************************************************
Function Name: pathCompare
Description:   Function to perform a slash and case insensitive string compare
Arguments:     IN  pszS1 - Pointer to the first string
               IN  pszS2 - Pointer to the second string
               IN  stLengthS2 - The length of S1
Return value:  If the two strings are equivalent the return is zero.
*****************************************************************************/
int pathCompare(const char   *pszS1, const char   *pszS2)
{
    /* Drop the first slash at the front of either string */
    if ((*pszS1 == '\\')
    ||  (*pszS1 == '/'))
    {
        pszS1++;
    }
    if ((*pszS2 == '\\')
    ||  (*pszS2 == '/'))
    {
        pszS2++;
    }

    /* Until the end of either sting */
    while ((*pszS1) && (*pszS2))
    {
        /* Do a case insensitive compare */
        if (((*pszS1) | 0x20) != ((*pszS2) | 0x20))
        {
            if ((((*pszS1) == '\\') || ((*pszS1) == '/'))
            &&  (((*pszS2) == '\\') || ((*pszS2) == '/')))
            {
                /* Only different by the slash is OK */
            }
            else
            {
                break;
            }
        }

        /* Bump the pointers */
        pszS1++;
        pszS2++;

        /* Both strings must be the same length */
        if ((!*pszS2)
        &&  (!*pszS2))
        {
            return 0;
        }
    }
    return 1;
}
/*****************************************************************************
End of function  pathCompare
******************************************************************************/

/*****************************************************************************
Function Name: cticks
Description:   Function to get the current system "tick"
Arguments:     none
Return value:  The number of mS since the timer was opened
*****************************************************************************/
uint32_t cticks(void)
{
    uint32_t    ulClockTicks = 0UL;
    int         iClockTicks = 0;


    iClockTicks = xTaskGetTickCount();

    if (iClockTicks >= 0 )
    {
    	ulClockTicks = (uint32_t)iClockTicks;
    }

    return ulClockTicks;
}
/*****************************************************************************
End of function  cticks
******************************************************************************/

