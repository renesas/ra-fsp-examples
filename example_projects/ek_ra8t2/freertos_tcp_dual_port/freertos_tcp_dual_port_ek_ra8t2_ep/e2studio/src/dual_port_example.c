/***********************************************************************************************************************
 * File: multiport_example.c
 * Description: FreeRTOS+TCP dual-interface (ETH0 / ETH1) TCP server with DNS + Ping test
 **********************************************************************************************************************/

#include <dual_port_example.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_Routing.h"
#include "common_utils.h"

/***********************************************************************************************************************
 * Macros
 **********************************************************************************************************************/

#define PING_COUNT          20U
#define PING_DATA_SIZE      32U
#define PING_TIMEOUT_MS     1000U
#define PING_INTERVAL_MS    100U

#define RECV_BUF_SIZE       256U
#define ACCEPT_TIMEOUT_MS   5000U

/***********************************************************************************************************************
 * Network interface instances - defined externally
 **********************************************************************************************************************/
extern NetworkInterface_t g_freertos_plus_tcp0_xInterface;
extern NetworkInterface_t g_freertos_plus_tcp1_xInterface;

/***********************************************************************************************************************
 * Endpoint configurations
 **********************************************************************************************************************/
static NetworkEndPoint_t xEndPoint_Eth0;
static NetworkEndPoint_t xEndPoint_Eth1;

/* MAC addresses */
static const uint8_t ucMACAddress_Interface0[ipMAC_ADDRESS_LENGTH_BYTES] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
static const uint8_t ucMACAddress_Interface1[ipMAC_ADDRESS_LENGTH_BYTES] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x66 };

/* ETH0 - IPv4 configuration */
static const uint8_t ucIPAddress_Eth0[ipIP_ADDRESS_LENGTH_BYTES]        = { 192, 168, 0, 101 };
static const uint8_t ucNetMask_Eth0[ipIP_ADDRESS_LENGTH_BYTES]          = { 255, 255, 255, 0 };
static const uint8_t ucGatewayAddress_Eth0[ipIP_ADDRESS_LENGTH_BYTES]   = { 192, 168, 0, 1 };
static const uint8_t ucDNSServerAddress_Eth0[ipIP_ADDRESS_LENGTH_BYTES] = { 192, 168, 0, 1 };

/* ETH1 - IPv4 configuration */
static const uint8_t ucIPAddress_Eth1[ipIP_ADDRESS_LENGTH_BYTES]        = { 192, 168, 1, 102 };
static const uint8_t ucNetMask_Eth1[ipIP_ADDRESS_LENGTH_BYTES]          = { 255, 255, 255, 0 };
static const uint8_t ucGatewayAddress_Eth1[ipIP_ADDRESS_LENGTH_BYTES]   = { 192, 168, 1, 1 };
static const uint8_t ucDNSServerAddress_Eth1[ipIP_ADDRESS_LENGTH_BYTES] = { 192, 168, 1, 1 };

/***********************************************************************************************************************
 * Global ping counter - updated in vApplicationPingReplyHook()
 **********************************************************************************************************************/
volatile uint8_t ucPingReceived = 0;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void prvTcpServerTask( void * pvParameters );

/***********************************************************************************************************************
 * @brief   TCP server task parameters
 **********************************************************************************************************************/
typedef struct
{
    char     pcIP[16];
    uint16_t usPort;
} TcpServerParams_t;

static TcpServerParams_t xServerParamsEth0 = { "192.168.0.52", SERVER_PORT_ETH0 };
static TcpServerParams_t xServerParamsEth1 = { "192.168.0.55", SERVER_PORT_ETH1 };

/***********************************************************************************************************************
 * @brief   Generic TCP server task — binds to a specific IP:port, accepts one client at a time, echoes data back.
 * @param   pvParameters  Pointer to TcpServerParams_t
 **********************************************************************************************************************/
static void prvTcpServerTask( void * pvParameters )
{
    const TcpServerParams_t    * pxParams       = ( TcpServerParams_t * ) pvParameters;
    Socket_t                     xListenSocket  = FREERTOS_INVALID_SOCKET;
    Socket_t                     xClientSocket  = FREERTOS_INVALID_SOCKET;
    struct freertos_sockaddr     xBindAddr      = { 0 };
    struct freertos_sockaddr     xClientAddr    = { 0 };
    socklen_t                    xClientAddrLen = sizeof( xClientAddr );
    BaseType_t                   xRet;
    char                         pcRecvBuf[ RECV_BUF_SIZE ];
    BaseType_t                   xBytesReceived;
    TickType_t                   xAcceptTimeout = pdMS_TO_TICKS( ACCEPT_TIMEOUT_MS );

    /*-----------------------------------------------------------------
     * Create TCP listen socket
     *-----------------------------------------------------------------*/
    xListenSocket = FreeRTOS_socket( FREERTOS_AF_INET,
                                     FREERTOS_SOCK_STREAM,
                                     FREERTOS_IPPROTO_TCP );
    configASSERT( xListenSocket != FREERTOS_INVALID_SOCKET );

    /* Set accept timeout so we don't need vTaskDelay in the accept loop */
    FreeRTOS_setsockopt( xListenSocket, 0, FREERTOS_SO_RCVTIMEO,
                         &xAcceptTimeout, sizeof( xAcceptTimeout ) );

    /*-----------------------------------------------------------------
     * Bind to local endpoint
     *-----------------------------------------------------------------*/
    xBindAddr.sin_family            = FREERTOS_AF_INET4;
    xBindAddr.sin_address.ulIP_IPv4 = FreeRTOS_inet_addr( pxParams->pcIP );
    xBindAddr.sin_port              = FreeRTOS_htons( pxParams->usPort );

    xRet = FreeRTOS_bind( xListenSocket, &xBindAddr, sizeof( xBindAddr ) );
    if( xRet != 0 )
    {
        APP_PRINT( "\r\n[TCP Server] FreeRTOS_bind() failed (ret=%ld, ip=%s, port=%u)\r\n",
                   xRet, pxParams->pcIP, pxParams->usPort );
        FreeRTOS_closesocket( xListenSocket );
        vTaskDelete( NULL );
        return;
    }

    /*-----------------------------------------------------------------
     * Start listening
     *-----------------------------------------------------------------*/
    xRet = FreeRTOS_listen( xListenSocket, 1 );
    if( xRet != 0 )
    {
        APP_PRINT( "\r\n[TCP Server] FreeRTOS_listen() failed (ret=%ld)\r\n", xRet );
        FreeRTOS_closesocket( xListenSocket );
        vTaskDelete( NULL );
        return;
    }

    APP_PRINT( "\r\n[TCP Server] Listening on %s:%u", pxParams->pcIP, pxParams->usPort );

    /*-----------------------------------------------------------------
     * Accept loop — one client at a time
     *-----------------------------------------------------------------*/
    for( ;; )
    {
        APP_PRINT( "\r\n[TCP Server] [%s:%u] Waiting for client...",
                   pxParams->pcIP, pxParams->usPort );

        xClientSocket = FreeRTOS_accept( xListenSocket, &xClientAddr, &xClientAddrLen );

        if( ( xClientSocket == FREERTOS_INVALID_SOCKET ) || ( xClientSocket == NULL ) )
        {
            /* Accept timed out — retry without blocking */
            continue;
        }

        APP_PRINT( "\r\n[TCP Server] [%s:%u] Client connected",
                   pxParams->pcIP, pxParams->usPort );

        /*-------------------------------------------------------------
         * Receive loop — echo until client disconnects
         *-------------------------------------------------------------*/
        for( ;; )
        {
            memset( pcRecvBuf, 0, sizeof( pcRecvBuf ) );

            xBytesReceived = FreeRTOS_recv( xClientSocket,
                                            pcRecvBuf,
                                            sizeof( pcRecvBuf ) - 1U,
                                            0 );

            if( xBytesReceived > 0 )
            {
                pcRecvBuf[ xBytesReceived ] = '\0';
                APP_PRINT( "\r\n[TCP Server] [%s:%u] Rx %ld byte(s): \"%s\"",
                           pxParams->pcIP, pxParams->usPort,
                           xBytesReceived, pcRecvBuf );

                /* Echo back */
                FreeRTOS_send( xClientSocket, pcRecvBuf, ( size_t ) xBytesReceived, 0 );
            }
            else if( xBytesReceived == 0 )
            {
                APP_PRINT( "\r\n[TCP Server] [%s:%u] Client disconnected\r\n",
                           pxParams->pcIP, pxParams->usPort );
                break;
            }
            else
            {
                break;
            }
        }

        /*-------------------------------------------------------------
         * Close client socket, ready for next connection
         *-------------------------------------------------------------*/
        FreeRTOS_closesocket( xClientSocket );
        xClientSocket = FREERTOS_INVALID_SOCKET;
        APP_PRINT( "\r\n[TCP Server] [%s:%u] Ready for next connection\r\n",
                   pxParams->pcIP, pxParams->usPort );
    }
}

/***********************************************************************************************************************
 * @brief   DNS lookup + ping test to google.com
 **********************************************************************************************************************/
static void prvPingTask( void )
{
    uint32_t   ulIPAddress;
    BaseType_t xResult;
    uint8_t    ucCount;
    uint8_t    ucSent   = 0;
    uint8_t    ucLost;
    float      fLossPct;

    /*-----------------------------------------------------------------
     * DNS Lookup
     *-----------------------------------------------------------------*/
    APP_PRINT( "[DNS] Resolving google.com..." );

    ulIPAddress = FreeRTOS_gethostbyname( "google.com" );
    if( ulIPAddress == 0 )
    {
        APP_PRINT( "FAILED\r\n" );
        APP_PRINT( "[DNS] Could not resolve google.com\r\n" );
        return;
    }

    APP_PRINT( "OK\r\n" );
    APP_PRINT( "[DNS] google.com -> %u.%u.%u.%u\r\n\r\n", IP_BYTES( ulIPAddress ) );

    /*-----------------------------------------------------------------
     * Ping
     *-----------------------------------------------------------------*/
    ucPingReceived = 0;

    APP_PRINT( "[PING] Target  : google.com (%u.%u.%u.%u)\r\n", IP_BYTES( ulIPAddress ) );
    APP_PRINT( "[PING] Count   : %u packets\r\n",  PING_COUNT );
    APP_PRINT( "[PING] Timeout : %u ms\r\n\r\n",   PING_TIMEOUT_MS );
    APP_PRINT( "[PING] Sending : " );

    for( ucCount = 0; ucCount < PING_COUNT; ucCount++ )
    {
        xResult = FreeRTOS_SendPingRequest( ulIPAddress, PING_DATA_SIZE,
                                            pdMS_TO_TICKS( PING_TIMEOUT_MS ) );
        if( xResult != pdFAIL )
        {
            ucSent++;
        }

        vTaskDelay( pdMS_TO_TICKS( PING_INTERVAL_MS ) );
    }

    ucLost   = ucSent - ucPingReceived;
    fLossPct = ( ucSent > 0 ) ? ( ( float ) ucLost / ucSent * 100.0f ) : 0.0f;

    APP_PRINT( "\r\n\r\n[PING] ---- Statistics ----\r\n" );
    APP_PRINT( "[PING] Sent     : %u\r\n",              ucSent );
    APP_PRINT( "[PING] Received : %u\r\n",              ucPingReceived );
    APP_PRINT( "[PING] Lost     : %u (%.0f%% loss)\r\n", ucLost, fLossPct );
}

/***********************************************************************************************************************
 * @brief   Ping reply callback — called by FreeRTOS+TCP on ICMP reply
 **********************************************************************************************************************/
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier )
{
    ( void ) usIdentifier;

    if( eStatus == eSuccess )
    {
        ucPingReceived++;
        APP_PRINT( "!" );
    }
    else
    {
        APP_PRINT( "." );
    }
}

/***********************************************************************************************************************
 * @brief   Network event hook — logs IP info when an interface comes up
 **********************************************************************************************************************/
void vApplicationIPNetworkEventHook_Multi(
    eIPCallbackEvent_t          eNetworkEvent,
    struct xNetworkEndPoint   * pxEndPoint )
{
    const char    * p_if_name = "UNKNOWN";
    static uint32_t ulLastIPEth0 = 0;
    static uint32_t ulLastIPEth1 = 0;

    /* Only handle network-up events with a valid endpoint */
    if( ( eNetworkEvent != eNetworkUp ) || ( pxEndPoint == NULL ) )
    {
        return;
    }

    /* Identify interface and deduplicate log output */
    if( pxEndPoint == &xEndPoint_Eth0 )
    {
        p_if_name = "ETH0";
        if( pxEndPoint->ipv4_settings.ulIPAddress == ulLastIPEth0 ) return;
        ulLastIPEth0 = pxEndPoint->ipv4_settings.ulIPAddress;
        FreeRTOS_inet_ntoa(pxEndPoint->ipv4_settings.ulIPAddress, xServerParamsEth0.pcIP);
    }
    else if( pxEndPoint == &xEndPoint_Eth1 )
    {
        p_if_name = "ETH1";
        if( pxEndPoint->ipv4_settings.ulIPAddress == ulLastIPEth1 ) return;
        ulLastIPEth1 = pxEndPoint->ipv4_settings.ulIPAddress;
        FreeRTOS_inet_ntoa(pxEndPoint->ipv4_settings.ulIPAddress, xServerParamsEth1.pcIP);
    }

    const uint32_t ip      = pxEndPoint->ipv4_settings.ulIPAddress;
    const uint32_t netmask = pxEndPoint->ipv4_settings.ulNetMask;
    const uint32_t gateway = pxEndPoint->ipv4_settings.ulGatewayAddress;
    const uint32_t dns     = pxEndPoint->ipv4_settings.ulDNSServerAddresses[0];

    APP_PRINT( "\r\n[Network UP] %s\r\n"
               "IP      : %u.%u.%u.%u\r\n"
               "Netmask : %u.%u.%u.%u\r\n"
               "Gateway : %u.%u.%u.%u\r\n"
               "DNS     : %u.%u.%u.%u\r\n",
               p_if_name,
               IP_BYTES( ip ),
               IP_BYTES( netmask ),
               IP_BYTES( gateway ),
               IP_BYTES( dns ) );
}

/***********************************************************************************************************************
 * @brief   Initialize dual network interfaces, run DNS+ping test, then start TCP servers
 **********************************************************************************************************************/
void vInitializeNetworkInterfaces( void )
{
    BaseType_t xResult;

    /*-----------------------------------------------------------------
     * Configure endpoints
     *-----------------------------------------------------------------*/
    memset( &xEndPoint_Eth0, 0, sizeof( NetworkEndPoint_t ) );
    memset( &xEndPoint_Eth1, 0, sizeof( NetworkEndPoint_t ) );

    pxFSP_Eth_FillInterfaceDescriptor( 0, &g_freertos_plus_tcp0_xInterface );
    FreeRTOS_FillEndPoint( &g_freertos_plus_tcp0_xInterface,
                           &xEndPoint_Eth0,
                           ucIPAddress_Eth0, ucNetMask_Eth0,
                           ucGatewayAddress_Eth0, ucDNSServerAddress_Eth0,
                           ucMACAddress_Interface0 );

    pxFSP_Eth_FillInterfaceDescriptor( 1, &g_freertos_plus_tcp1_xInterface );
    FreeRTOS_FillEndPoint( &g_freertos_plus_tcp1_xInterface,
                           &xEndPoint_Eth1,
                           ucIPAddress_Eth1, ucNetMask_Eth1,
                           ucGatewayAddress_Eth1, ucDNSServerAddress_Eth1,
                           ucMACAddress_Interface1 );

#if ( ipconfigUSE_DHCP != 0 )
    xEndPoint_Eth0.bits.bWantDHCP = pdTRUE;
    xEndPoint_Eth1.bits.bWantDHCP = pdTRUE;
#endif

    /*-----------------------------------------------------------------
     * Start IP stack
     *-----------------------------------------------------------------*/
    xResult = FreeRTOS_IPInit_Multi();

    if( xResult == pdPASS )
    {
        APP_PRINT( "\r\nMulti-endpoint network stack initialized successfully\r\n" );
    }
    else
    {
        APP_PRINT( "[ERROR] Failed to initialize multi-endpoint network stack\r\n" );
        return;
    }

    /*-----------------------------------------------------------------
     * Wait for both interfaces to come up
     *-----------------------------------------------------------------*/
    APP_PRINT( "\r\n[NET] Waiting for ETH0 and ETH1..." );
    while( ( xEndPoint_Eth0.bits.bEndPointUp == pdFALSE ) ||
           ( xEndPoint_Eth1.bits.bEndPointUp == pdFALSE ) )
    {
        APP_PRINT( "." );
        vTaskDelay( pdMS_TO_TICKS( 100 ) );
    }

    /*-----------------------------------------------------------------
     * DNS + Ping test
     *-----------------------------------------------------------------*/
    APP_PRINT( "\r\n========================================\r\n" );
    APP_PRINT( "       DNS Lookup and Ping Test         \r\n" );
    APP_PRINT( "========================================\r\n\r\n" );

    prvPingTask();

    /*-----------------------------------------------------------------
     * Start TCP servers
     *-----------------------------------------------------------------*/
    APP_PRINT( "\r\n========================================\r\n" );
    APP_PRINT( "         TCP SERVER STARTING...         \r\n" );
    APP_PRINT( "========================================\r\n" );
    APP_PRINT( "  Run TCP client on PC:\r\n" );
    APP_PRINT( "    > python tcp_client.py %u.%u.%u.%u:%u\r\n",
               IP_BYTES( xEndPoint_Eth0.ipv4_settings.ulIPAddress ), SERVER_PORT_ETH0 );
    APP_PRINT( "    > python tcp_client.py %u.%u.%u.%u:%u\r\n",
               IP_BYTES( xEndPoint_Eth1.ipv4_settings.ulIPAddress ), SERVER_PORT_ETH1 );
    APP_PRINT( "========================================\r\n\r\n" );

    xTaskCreate( prvTcpServerTask, "TCP_ETH0", 1024,
                 ( void * ) &xServerParamsEth0, tskIDLE_PRIORITY + 1, NULL );

    vTaskDelay( pdMS_TO_TICKS( 1000 ) );

    xTaskCreate( prvTcpServerTask, "TCP_ETH1", 1024,
                 ( void * ) &xServerParamsEth1, tskIDLE_PRIORITY + 2, NULL );
}

#if (ipconfigDHCP_REGISTER_HOSTNAME == 1)
/*******************************************************************************************************************//**
 * @brief      DHCP Hook function to populate the user defined Host name for the Kit.
 * @param[in]  None
 * @retval     Host name.
 **********************************************************************************************************************/
const char *pcApplicationHostnameHook(void)
{
    return KIT_NAME;
}
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

