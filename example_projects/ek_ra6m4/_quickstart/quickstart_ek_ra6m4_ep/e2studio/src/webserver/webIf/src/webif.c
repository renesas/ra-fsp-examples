/***********************************************************************************************************************
* Copyright (c) 2012 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
 /******************************************************************************
 * File Name    : Webif.c
 * Version      : 1.00
 * Description  : Interface functions for John Bartas's Webio
 *******************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 04.02.2010 1.00    First Release
 ******************************************************************************/

/******************************************************************************
 WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
 OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
 SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
 ******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "websys.h"
#include "webio.h"
#include "webfs.h"
#include "webif.h"

#include "board_cfg.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/* Comment this line out to turn ON module trace in this file */
#undef _TRACE_ON_

#ifndef _TRACE_ON_
    #undef TRACE
    #define TRACE(x)
#endif

/*****************************************************************************
 Enumerated Types
 ******************************************************************************/

/*****************************************************************************
 Typedefs
 ******************************************************************************/

/*****************************************************************************
 Constant Data
 ******************************************************************************/

/*****************************************************************************
 Function Prototypes
 ******************************************************************************/

static int wsAuthenticate (void *fd, char *name, char *password, wi_sess *sess);
static void wsMain( void *pvParameters );

/*****************************************************************************
 External Variables
 ******************************************************************************/

extern socktype wi_listen;

/*****************************************************************************
 Global Variables
 ******************************************************************************/
extern _Bool cmdCheckUserNameAndPassword(char *pszUserName, char *pszPassword);

static TaskHandle_t *gs_pgui_webio_task_id = NULL;

/*****************************************************************************
 Public Functions
 ******************************************************************************/

/*****************************************************************************
 Function Name: wsStart
 Description:   Function to start the Webio service
 Arguments:     IN  usPortNumber - The port number to use
 Return value:  true if the service was started
 *****************************************************************************/
_Bool wsStart (uint16_t usPortNumber)
{
    if (NULL == gs_pgui_webio_task_id)
    {
        int iError;

        /* Set the port number for the Webio server */
        httpport = usPortNumber;

        /* Initialise Webio */
        iError = wi_init();


        if ( !iError)
        {
            /* Install our port-local authentication routine */
            emfs.wfs_fauth = wsAuthenticate;

            /* Create the task to run the Webio server */

            /* Create the task to run the Webio server */
            xTaskCreate(wsMain,
                       (const char*) "Webio wi_thread",
                       configMINIMAL_STACK_SIZE * 32, NULL,
                       10, // close to idle
                       &gs_pgui_webio_task_id);

            if (NULL != gs_pgui_webio_task_id)
            {
                return true;
            }
            else
            {
                /* No function to uninit Webio */
                /* NOTE: Sockets will not be closed! */
            	;
            }
        }
        else
        {
            // TRACE(("wsStart: **Error: wi_init error %d\r\n", iError));
            ;
        }
    }
    return false;
}
/*****************************************************************************
 End of function  wsStart
 ******************************************************************************/

/*****************************************************************************
 Function Name: wsStop
 Description:   Function to stop the Webio service
 Arguments:     none
 Return value:  none
 *****************************************************************************/
void wsStop (void)
{
    if (NULL != gs_pgui_webio_task_id)
    {
        wi_sess *pSess;
        wi_sess *pNext;

        /* Kill the Webio task */
        R_OS_DeleteTask(gs_pgui_webio_task_id);
        gs_pgui_webio_task_id = NULL;

        /* No function to uninit Webio */
        pSess = wi_sessions;
        while (pSess)
        {
            pNext = pSess->ws_next;
            wi_delsess(pSess);
            pSess = pNext;
        }

        /* Close the listening socket */
        closesocket(wi_listen);
    }
}
/*****************************************************************************
 End of function  wsStop
 ******************************************************************************/

/*****************************************************************************
 Function Name: wsBreakPoint
 Description:   Webio break point function
 Arguments:     none
 Return value:  none
 *****************************************************************************/
void wsBreakPoint (void)
{
    uint32_t ctf;
    uint32_t state = OFF;

    TRACE(("wsBreakPoint\r\n"));
    TURN_RED_ON;

    while(1)
    {
        ctf = 200000;
        while(ctf--)
        {
          __NOP();
        }
    }
}
/*****************************************************************************
 End of function  wsBreakPoint
 ******************************************************************************/

/*****************************************************************************
 Function Name: wsPanic
 Description:   The Webio stuck up a gum tree function
 Arguments:     none
 Return value:  none
 *****************************************************************************/
void wsPanic (char *pszMessage)
{
    (void) pszMessage;
    TRACE(("wsPanic: %s\r\n", pszMessage));
    wsBreakPoint();

    /* This is similar to John's test code but it is not suitable for an
     embedded web server! */
#if 0
    gs_pgui_webio_task_id = NULL;
    taskExit();
#endif
}
/*****************************************************************************
 End of function  wsPanic
 ******************************************************************************/

/*****************************************************************************
 Private Functions
 ******************************************************************************/

/*****************************************************************************
 Function Name: wsAuthenticate
 Description:   Function to check a file for authentication
 Arguments:     IN  fd - Pointer to the Webio file object
 IN  name - Pointer to the login name string
 IN  password - Pointer to the password
 IN  sess - Pointer to the session
 Return value:  0 if the user is not allowed access to the file
 *****************************************************************************/
static int wsAuthenticate (void *fd, char *name, char *password, wi_sess *sess)
{
    /* See if this file requires authentication. */
    EOFILE *pEoFile = (EOFILE *) fd;
    (void) sess;
    if (pEoFile->eo_authenticate)
    {
#if 0
        /* Always check the user name and password */
        if ( !cmdCheckUserNameAndPassword(name, password))
        {
            return 0;
        }
#endif
    }
    return 1;
}
/*****************************************************************************
 End of function  wsAuthenticate
 ******************************************************************************/

/*****************************************************************************
 Function Name: wsMain
 Description:   The main webserver task
 Arguments:     none
 Return value:  none
 *****************************************************************************/
static void wsMain( void *pvParameters )
{
//    R_OS_TaskUsesFloatingPoint();

    while (true)
    {
        /* Run the Webio main task */
        wi_thread();
    }
}
/*****************************************************************************
 End of function  wsMain
 ******************************************************************************/

/******************************************************************************
 End  Of File
 ******************************************************************************/

