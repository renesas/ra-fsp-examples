/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
#include <string.h>

#include "r_ble_serv_common.h"

ble_status_t decode_8bit(uint8_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    if ((NULL == p_app_value) || (NULL == p_gatt_value))
    {
        return BLE_ERR_INVALID_PTR;
    }

    if (p_gatt_value->value_len != sizeof(uint8_t))
    {
        return BLE_ERR_INVALID_DATA;
    }

    *p_app_value = p_gatt_value->p_value[0];

    return BLE_SUCCESS;
}

ble_status_t encode_8bit(const uint8_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    if ((NULL == p_app_value) || (NULL == p_gatt_value))
    {
        return BLE_ERR_INVALID_PTR;
    }

    if (p_gatt_value->value_len < sizeof(uint8_t))
    {
        return BLE_ERR_INVALID_DATA;
    }

    p_gatt_value->p_value[0] = *p_app_value;

    return BLE_SUCCESS;
}

ble_status_t decode_16bit(uint16_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    if ((NULL == p_app_value) || (NULL == p_gatt_value))
    {
        return BLE_ERR_INVALID_PTR;
    }

    if (p_gatt_value->value_len != sizeof(uint16_t))
    {
        return BLE_ERR_INVALID_DATA;
    }

    *p_app_value = (uint16_t)((p_gatt_value->p_value[0]) | (p_gatt_value->p_value[1] << 8));

    return BLE_SUCCESS;
}

ble_status_t encode_16bit(const uint16_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    if ((NULL == p_app_value) || (NULL == p_gatt_value))
    {
        return BLE_ERR_INVALID_PTR;
    }

    if (p_gatt_value->value_len < sizeof(uint16_t))
    {
        return BLE_ERR_INVALID_DATA;
    }

    p_gatt_value->p_value[0] = (uint8_t)((*p_app_value) & 0xFF);
    p_gatt_value->p_value[1] = (uint8_t)(((*p_app_value) >> 8) & 0xFF);

    return BLE_SUCCESS;
}

ble_status_t decode_24bit(uint32_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    if ((NULL == p_app_value) || (NULL == p_gatt_value))
    {
        return BLE_ERR_INVALID_PTR;
    }

    if (p_gatt_value->value_len != 3)
    {
        return BLE_ERR_INVALID_DATA;
    }

    *p_app_value = (uint32_t)((p_gatt_value->p_value[0]) | (p_gatt_value->p_value[1] << 8) |
                   (p_gatt_value->p_value[2] << 16));

    return BLE_SUCCESS;
}

ble_status_t decode_32bit(uint32_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    if ((NULL == p_app_value) || (NULL == p_gatt_value))
    {
        return BLE_ERR_INVALID_PTR;
    }

    if (p_gatt_value->value_len != sizeof(uint32_t))
    {
        return BLE_ERR_INVALID_DATA;
    }

    *p_app_value = (uint32_t)((p_gatt_value->p_value[0]) | (p_gatt_value->p_value[1] << 8) |
                   (p_gatt_value->p_value[2] << 16) | (p_gatt_value->p_value[3] << 24));

    return BLE_SUCCESS;
}

ble_status_t encode_24bit(const uint32_t * p_app_value, st_ble_gatt_value_t * p_gatt_value)
{
    if ((NULL == p_app_value) || (NULL == p_gatt_value))
    {
        return BLE_ERR_INVALID_PTR;
    }

    if (p_gatt_value->value_len < 3)
    {
        return BLE_ERR_INVALID_DATA;
    }

    p_gatt_value->p_value[0] = *p_app_value & 0xFF;
    p_gatt_value->p_value[1] = (*p_app_value >> 8) & 0xFF;
    p_gatt_value->p_value[2] = (*p_app_value >> 16) & 0xFF;
    p_gatt_value->value_len  = 3;

    return BLE_SUCCESS;
}

ble_status_t encode_32bit(const uint32_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    if ((NULL == p_app_value) || (NULL == p_gatt_value))
    {
        return BLE_ERR_INVALID_PTR;
    }

    if (p_gatt_value->value_len < sizeof(uint32_t))
    {
        return BLE_ERR_INVALID_DATA;
    }

    p_gatt_value->p_value[0] = (uint8_t)(*p_app_value & 0xFF);
    p_gatt_value->p_value[1] = (uint8_t)((*p_app_value >> 8) & 0xFF);
    p_gatt_value->p_value[2] = (uint8_t)((*p_app_value >> 16) & 0xFF);
    p_gatt_value->p_value[3] = (uint8_t)((*p_app_value >> 24) & 0xFF);

    return BLE_SUCCESS;
}

ble_status_t decode_allcopy(uint8_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    if ((NULL == p_app_value) || (NULL == p_gatt_value))
    {
        return BLE_ERR_INVALID_PTR;
    }

    memcpy(p_app_value, p_gatt_value->p_value, p_gatt_value->value_len);

    return BLE_SUCCESS;
}

ble_status_t encode_allcopy(const uint8_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    if ((NULL == p_app_value) || (NULL == p_gatt_value))
    {
        return BLE_ERR_INVALID_PTR;
    }

    memcpy(p_gatt_value->p_value, p_app_value, p_gatt_value->value_len);

    return BLE_SUCCESS;

}

ble_status_t decode_st_ble_seq_data_t(st_ble_seq_data_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value)
{
    p_app_value->data = p_gatt_value->p_value;
    p_app_value->len  = p_gatt_value->value_len;
    return BLE_SUCCESS;
}

ble_status_t encode_st_ble_seq_data_t(const st_ble_seq_data_t *p_app_value, st_ble_gatt_value_t *p_gatt_value)
{
    if (p_app_value->len > p_gatt_value->value_len)
    {
        return BLE_ERR_INVALID_DATA;
    }

    memcpy(p_gatt_value->p_value, p_app_value->data, p_app_value->len);

    /* for characteristic size */
    p_gatt_value->value_len = p_app_value->len;

    return BLE_SUCCESS;
}

uint8_t pack_st_ble_ieee11073_sfloat_t(uint8_t *p_dst, const st_ble_ieee11073_sfloat_t *p_src)
{
    uint8_t pos = 0;
    uint16_t sfloat = (uint16_t)((p_src->mantissa & 0x0FFF) | (p_src->exponent << 12));

    p_dst[pos++] = (uint8_t)(sfloat & 0xFF);
    p_dst[pos++] = (uint8_t)((sfloat >> 8) & 0xFF);

    return pos;
}

uint8_t unpack_st_ble_ieee11073_sfloat_t(st_ble_ieee11073_sfloat_t *p_dst, const uint8_t *p_src)
{
    p_dst->mantissa = (int16_t)(p_src[0] | ((p_src[1] & 0x0F) << 8));
    p_dst->exponent = (int8_t)(p_src[1] >> 4);

    if (p_dst->exponent & 0x08)
    {
        p_dst->exponent  = (int8_t)(p_dst->exponent | 0xF0);
    }

    return 2;
}

uint8_t pack_st_ble_date_time_t(uint8_t *p_dst, const st_ble_date_time_t *p_src)
{
    uint32_t pos = 0;

    BT_PACK_LE_2_BYTE(&p_dst[pos], &p_src->year);
    pos += 2;
    p_dst[pos++] = p_src->month;
    p_dst[pos++] = p_src->day;
    p_dst[pos++] = p_src->hours;
    p_dst[pos++] = p_src->minutes;
    p_dst[pos++] = p_src->seconds;

    return (uint8_t)pos;
}

uint8_t unpack_st_ble_date_time_t(st_ble_date_time_t *p_dst, const uint8_t *p_src)
{
    uint32_t pos = 0;

    BT_UNPACK_LE_2_BYTE(&p_dst->year, &p_src[pos]);
    pos += 2;
    p_dst->month   = p_src[pos++];
    p_dst->day     = p_src[pos++];
    p_dst->hours   = p_src[pos++];
    p_dst->minutes = p_src[pos++];
    p_dst->seconds = p_src[pos++];

    return (uint8_t)pos;
}
