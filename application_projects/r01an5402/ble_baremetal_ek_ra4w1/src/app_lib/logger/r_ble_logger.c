/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include <string.h>
#include "r_ble_logger.h"
#include "r_ble_api.h"

#if (BLE_DEFAULT_LOG_LEVEL != 0)

char *BLE_BD_ADDR_STR(uint8_t *p_addr, uint8_t addr_type)
{
    static char addr_str[28];

    memset(addr_str, '\0', 28);

    if (NULL != p_addr)
    {
        sprintf(addr_str, "%02X:%02X:%02X:%02X:%02X:%02X %s",
                p_addr[5], p_addr[4], p_addr[3], p_addr[2], p_addr[1], p_addr[0],
                (BLE_GAP_ADDR_PUBLIC == addr_type) ? "pub" : 
                    ((BLE_GAP_ADDR_RAND == addr_type) ? "rnd" : 
                        ((BLE_GAP_ADDR_RPA_ID_PUBLIC == addr_type) ? "rpa_pub" : "rpa_rnd")));
    }

    return addr_str;
}

char *BLE_UUID_STR(uint8_t *p_uuid, uint8_t uuid_type)
{
    static char uuid_str[37];

    memset(uuid_str, '\0', 37);

    if (NULL != p_uuid)
    {
        if (uuid_type == BLE_GATT_16_BIT_UUID_FORMAT)
        {
            /* For 16bit, ex: 0xE29B */
            sprintf(uuid_str, "0x%02X%02X", p_uuid[1], p_uuid[0]);
        }
        else if (uuid_type == BLE_GATT_128_BIT_UUID_FORMAT)
        {
            /* For 128bit, ex: 550E8400-E29B-41D4-A716-446655440000 */
            sprintf(uuid_str, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                    p_uuid[15], p_uuid[14], p_uuid[13], p_uuid[12],
                    p_uuid[11], p_uuid[10], p_uuid[ 9], p_uuid[ 8],
                    p_uuid[ 7], p_uuid[ 6], p_uuid[ 5], p_uuid[ 4],
                    p_uuid[ 3], p_uuid[ 2], p_uuid[ 1], p_uuid[ 0]);
        }
        else
        {
            /* Do nothing */
        }
    }

    return uuid_str;
}

#else /* (BLE_DEFAULT_LOG_LEVEL != 0) */
static char gs_dummy_str[] = "-";
char *BLE_BD_ADDR_STR(uint8_t *p_addr, uint8_t addr_type)
{
    (void)p_addr;
    (void)addr_type;
    return &gs_dummy_str[0];
}

char *BLE_UUID_STR(uint8_t *p_uuid, uint8_t uuid_type)
{
    (void)p_uuid;
    (void)uuid_type;
    return &gs_dummy_str[0];
}

#endif /* (BLE_DEFAULT_LOG_LEVEL != 0) */
