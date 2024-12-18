/*
 * FreeRTOS
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file cellular_setup.c
 * @brief Setup cellular connectivity for board with cellular module.
 */

/* FreeRTOS include. */
#include <FreeRTOS.h>
#include "task.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/* FreeRTOS Cellular Library include. */
#include "cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_types.h"
#include "cellular_api.h"
#include "cellular_comm_interface.h"
#include "cellular_platform.h"

#include "common_utils.h"
/*-----------------------------------------------------------*/
#define CELLULAR_APN "iot.truphone.com"  /* APN : Truphone SIM Card */
//#define CELLULAR_APN "broadband"
//#define CELLULAR_APN "VZWINTERNET"

#ifndef CELLULAR_APN
    #error "CELLULAR_APN is not defined in cellular_config.h"
#endif

#define CELLULAR_PDN_CONNECT_TIMEOUT             ( 100000UL )
#define CELLULAR_SIM_CARD_WAIT_INTERVAL_MS       ( 500UL )
#define CELLULAR_MAX_SIM_RETRY                   ( 5U )

#define CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS    ( 1000UL )

#define CELLULAR_PDN_CONTEXT_NUM                 ( CELLULAR_PDN_CONTEXT_ID_MAX - CELLULAR_PDN_CONTEXT_ID_MIN + 1U )

#define MODEM_EVENT_BOOTUP_OR_REBOOT_BIT        ( 0x00000001U )
#define MODEM_EVENT_POWERED_DOWN_BIT            ( 0x00000002U )
#define MODEM_EVENT_PSM_ENTER_BIT               ( 0x00000004U )

/*-----------------------------------------------------------*/

/* the default Cellular comm interface in system. */
//extern CellularCommInterface_t g_cellular_comm_interface_on_uart;

static void prvNetworkRegistrationCallback( CellularUrcEvent_t urcEvent,
                                            const CellularServiceStatus_t * pServiceStatus,
                                            void * pCallbackContext );
static void modemEventCallback( CellularModemEvent_t modemEvent,
                                           void * pCallbackContext );

void get_time();
char* getDNS(char *host);
bool setupCellular( void );
/*-----------------------------------------------------------*/

/* Secure socket needs application to provide the cellular handle and pdn context id. */
/* User of secure sockets cellular should provide this variable. */
extern CellularHandle_t CellularHandle ;

/* User of secure sockets cellular should provide this variable. */
extern uint8_t CellularSocketPdnContextId ;

/*-----------------------------------------------------------*/

PlatformEventGroupHandle_t  _modemEventGroup = NULL;
CellularError_t xCellularStatus = CELLULAR_SUCCESS;
CellularTime_t networkTime = { 0 };

bool setupCellular( void )
{
    bool cellularRet = true;
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularSimCardStatus_t simStatus = { 0 };
    CellularServiceStatus_t serviceStatus = { 0 };
    CellularSignalInfo_t  signalInfo = {0};
    CellularCommInterface_t * pCommIntf = &g_cellular_comm_interface_on_uart;  //g_rm_cellular_comm_uart0
    uint8_t tries = 0;
    CellularPdnConfig_t pdnConfig = { CELLULAR_PDN_CONTEXT_IPV4V6, CELLULAR_PDN_AUTH_NONE, CELLULAR_APN, "", "" };
    CellularPdnStatus_t PdnStatusBuffers[ CELLULAR_PDN_CONTEXT_NUM ] = { 0 };
    char localIP[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { '\0' };
    uint32_t timeoutCountLimit = ( CELLULAR_PDN_CONNECT_TIMEOUT / CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS ) + 1U;
    uint32_t timeoutCount = 0;
    uint8_t NumStatus = 0;
    bool pdnStatus = false;
    uint32_t i = 0U;
    CellularError_t err;

    _modemEventGroup = xEventGroupCreate();

    xEventGroupClearBits( _modemEventGroup,
                            MODEM_EVENT_BOOTUP_OR_REBOOT_BIT | MODEM_EVENT_POWERED_DOWN_BIT | MODEM_EVENT_PSM_ENTER_BIT );

    /* Initialize Cellular Comm Interface. */
    cellularStatus = Cellular_Init( &CellularHandle, pCommIntf );

    if( cellularStatus != CELLULAR_SUCCESS )
    {
        APP_ERR_PRINT(">>>  Cellular_Init failure %d  <<<\r\n", (uint32_t)cellularStatus);
    }
    else
    {
        /* wait until SIM is ready */
        for( tries = 0; tries < CELLULAR_MAX_SIM_RETRY; tries++ )
        {
            cellularStatus = Cellular_GetSimCardStatus( CellularHandle, &simStatus );

            if( ( cellularStatus == CELLULAR_SUCCESS ) &&
                ( ( simStatus.simCardState == CELLULAR_SIM_CARD_INSERTED ) &&
                  ( simStatus.simCardLockState == CELLULAR_SIM_CARD_READY ) ) )
            {
                APP_PRINT( ( "**** Cellular SIM okay  ****\r\n" ) );
                break;
            }
            else
            {
                APP_PRINT(">>>  Cellular SIM card state %d, Lock State %d <<<\r\n",
                        (uint32_t)simStatus.simCardState,
                        (uint32_t)simStatus.simCardLockState);
            }

            vTaskDelay( pdMS_TO_TICKS( CELLULAR_SIM_CARD_WAIT_INTERVAL_MS ) );
        }

        if( cellularStatus != CELLULAR_SUCCESS )
        {
            APP_ERR_PRINT(">>>  Cellular SIM failure  <<<\r\n");
        }
    }

    err = Cellular_RegisterUrcNetworkRegistrationEventCallback(CellularHandle, prvNetworkRegistrationCallback, NULL);
    if(CELLULAR_SUCCESS != err )
    {
        APP_ERR_PRINT("Network Registration failed\r\n");
    }


    err = Cellular_RegisterModemEventCallback(CellularHandle, modemEventCallback, NULL);
    if(CELLULAR_SUCCESS != err )
    {
        APP_ERR_PRINT("Modem Event Registration failed\r\n");
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_RfOff( CellularHandle );

        if( cellularStatus != CELLULAR_SUCCESS )
        {
            APP_ERR_PRINT("****  Cellular_RfOff failure %d  ****\r\n", (uint32_t)cellularStatus);
        }
    }


    /* Setup the PDN config. */
    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_SetPdnConfig( CellularHandle, CellularSocketPdnContextId, &pdnConfig );

        if( cellularStatus != CELLULAR_SUCCESS )
        {
            APP_ERR_PRINT(">>>  Cellular_SetPdnConfig failure %d  <<<\r\n", (uint32_t)cellularStatus);
        }
    }


    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_RfOn( CellularHandle );

        if( cellularStatus != CELLULAR_SUCCESS )
        {
            APP_ERR_PRINT(">>>  Cellular_RfOn failure %d  <<<\r\n", (uint32_t)cellularStatus);
        }
    }

    vTaskDelay( pdMS_TO_TICKS( CELLULAR_SIM_CARD_WAIT_INTERVAL_MS * 10 ) );

    cellularStatus =  Cellular_GetSignalInfo(CellularHandle, &signalInfo);


    /* Get service status. */
    if( cellularStatus == CELLULAR_SUCCESS )
    {
        while( timeoutCount < timeoutCountLimit )
        {
            cellularStatus = Cellular_GetServiceStatus( CellularHandle, &serviceStatus );

            if( ( cellularStatus == CELLULAR_SUCCESS ) &&
                ( ( serviceStatus.psRegistrationStatus == REGISTRATION_STATUS_REGISTERED_HOME ) ||
                  ( serviceStatus.psRegistrationStatus == REGISTRATION_STATUS_ROAMING_REGISTERED ) ) )
            {
                APP_PRINT( ( ">>>  Cellular module registered  <<<\r\n" ) );
                break;
            }
            else
            {
                APP_ERR_PRINT(">>>  Cellular GetServiceStatus failed %d, ps registration status %d  <<<\r\n",
                        (uint32_t)cellularStatus, (uint32_t)serviceStatus.psRegistrationStatus);
            }

            timeoutCount++;

            if( timeoutCount >= timeoutCountLimit )
            {
                APP_ERR_PRINT( ( ">>>  Cellular module can't be registered  <<<\r\n" ) );
            }

            vTaskDelay( pdMS_TO_TICKS( CELLULAR_PDN_CONNECT_WAIT_INTERVAL_MS ) );
        }
    }


    cellularStatus =  Cellular_GetSignalInfo(CellularHandle, &signalInfo);

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_ActivatePdn( CellularHandle, CellularSocketPdnContextId );

        if( cellularStatus != CELLULAR_SUCCESS )
        {
            APP_ERR_PRINT(">>>  Cellular_ActivatePdn failure %d  <<<\r\n", (uint32_t)cellularStatus);
        }
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_GetIPAddress( CellularHandle, CellularSocketPdnContextId, localIP, sizeof( localIP ) );

        if( cellularStatus != CELLULAR_SUCCESS )
        {
            APP_ERR_PRINT(">>>  Cellular_GetIPAddress failure %d  <<<\r\n", (uint32_t)cellularStatus);
        }
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        cellularStatus = Cellular_GetPdnStatus( CellularHandle, PdnStatusBuffers, CELLULAR_PDN_CONTEXT_NUM, &NumStatus );

        if( cellularStatus != CELLULAR_SUCCESS )
        {
            APP_ERR_PRINT(">>>  Cellular_GetPdnStatus failure %d  <<<\r\n", (uint32_t)cellularStatus);
        }
    }

    if( cellularStatus == CELLULAR_SUCCESS )
    {
        for( i = 0U; i < NumStatus; i++ )
        {
            if( ( PdnStatusBuffers[ i ].contextId == CellularSocketPdnContextId ) && ( PdnStatusBuffers[ i ].state == 1 ) )
            {
                pdnStatus = true;
                break;
            }
        }

        if( pdnStatus == false )
        {
            APP_PRINT(">>>  Cellular PDN is not activated <<< \r\n");
        }
    }

    if( ( cellularStatus == CELLULAR_SUCCESS ) && ( pdnStatus == true ) )
    {
        APP_PRINT(">>>  Cellular module registered, IP address %s  <<<\r\n",localIP);
        cellularRet = true;
    }
    else
    {
        cellularRet = false;
    }

    return cellularRet;
}


/* Modem event callback function. */
static void modemEventCallback( CellularModemEvent_t modemEvent,
                                void * pCallbackContext )
{
    ( void ) pCallbackContext;

    if( _modemEventGroup != NULL )
    {
        switch( modemEvent )
        {
            case CELLULAR_MODEM_EVENT_BOOTUP_OR_REBOOT:
                ( void ) PlatformEventGroup_SetBits( _modemEventGroup, MODEM_EVENT_BOOTUP_OR_REBOOT_BIT );
                break;

            case CELLULAR_MODEM_EVENT_POWERED_DOWN:
                ( void ) PlatformEventGroup_SetBits( _modemEventGroup, MODEM_EVENT_POWERED_DOWN_BIT );
                break;

            case CELLULAR_MODEM_EVENT_PSM_ENTER:
                ( void ) PlatformEventGroup_SetBits( _modemEventGroup, MODEM_EVENT_PSM_ENTER_BIT );
                break;

            default:
                break;
        }
    }
}

/* Network registration callback function. */
static void prvNetworkRegistrationCallback( CellularUrcEvent_t urcEvent,
                                            const CellularServiceStatus_t * pServiceStatus,
                                            void * pCallbackContext )
{
    FSP_PARAMETER_NOT_USED (pCallbackContext);

    if( pServiceStatus != NULL )
    {
        if( ( urcEvent == CELLULAR_URC_EVENT_NETWORK_CS_REGISTRATION ) ||
            ( urcEvent == CELLULAR_URC_EVENT_NETWORK_PS_REGISTRATION ) )
        {
            APP_PRINT("Network CS registration status received: %d. \r\n", (uint32_t)pServiceStatus->csRegistrationStatus);
            APP_PRINT("Network PS registration status received: %d. \r\n", (uint32_t)pServiceStatus->psRegistrationStatus);
        }
    }
}

void get_time()
{
    xCellularStatus = Cellular_GetNetworkTime( CellularHandle, &networkTime );
}


char pIpAddress[ CELLULAR_IP_ADDRESS_MAX_SIZE ] = { '\0' };

char* getDNS(char *host)
{
    xCellularStatus = Cellular_GetHostByName(CellularHandle, CellularSocketPdnContextId, host, pIpAddress );

    if(xCellularStatus != CELLULAR_SUCCESS)
    {
        return pIpAddress;
    }
    return pIpAddress;
}

/*-----------------------------------------------------------*/
