#include "new_thread0.h"
#include "FreeRTOS_IP.h"


#if( ipconfigUSE_DHCP != 0 )
 static uint8_t ucIPAddress[4]        = { 0x00 };
 static uint8_t ucNetMask[4]          = { 0x00 };
 static uint8_t ucGatewayAddress[4]   = { 0x00 };
 static uint8_t ucDNSServerAddress[4] = { 0x00 };
#else
 static uint8_t ucIPAddress[4]        = { 192, 168,   1, 100 };
 static uint8_t ucNetMask[4]          = { 255, 255, 255,   0 };
 static uint8_t ucGatewayAddress[4]   = { 192, 168,   1,   101 };
 static uint8_t ucDNSServerAddress[4] = {  10,  60,   1,   2 };
#endif

/* New Thread entry function */
/* pvParameters contains TaskHandle_t */
void new_thread0_entry(void *pvParameters)
{

    FSP_PARAMETER_NOT_USED (pvParameters);

    FreeRTOS_IPInit (ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, g_ether0.p_cfg->p_mac_address);

    R_MODBUS_TCP_SERVER_Open(&g_modbus_tcp_server0_ctrl, &g_modbus_tcp_server0_cfg);

    if (MODBUS_TCP_SERVER_HOST_IP_LIST_STATUS_ENABLE == g_modbus_tcp_server0_cfg.host_ip_list_status)
    {
        R_MODBUS_TCP_SERVER_RegisterIpList(&g_modbus_tcp_server0_ctrl, g_modbus_tcp_server_ip_address_list[0]);
    }

    R_MODBUS_TCP_SERVER_Start(&g_modbus_tcp_server0_ctrl);

    /* TODO: add your own code here */
    while (1)
    {
        vTaskDelay (1);
    }
}

/*******************************************************************************************************************//**
 * @brief      DHCP Hook function to populate the user defined Host name for the Kit.
 * @param[in]  None
 * @retval     Hostname
 **********************************************************************************************************************/
#if( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
const char* pcApplicationHostnameHook(void)
{
    return "MbusTCPSrv";
}
#endif
