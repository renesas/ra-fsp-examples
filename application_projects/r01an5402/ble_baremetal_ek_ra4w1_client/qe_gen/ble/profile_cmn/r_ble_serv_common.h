/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @file
 * @defgroup profile_cmn Profile Common Library
 * @{
 * @ingroup app_lib
 * @brief Profile Common Library
 * @details This library provides APIs to encode/decode default type and data types.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 23.08.2019 1.00    First Release
*         : 31.10.2019 1.01    Add doxygen comments.
***********************************************************************************************************************/

#ifndef R_BLE_SERV_COMMON_H
#define R_BLE_SERV_COMMON_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__ICCRX__)
/*RX23W*/
#include "r_ble_rx23w_if.h"
#else
/*RA4W*/
#include "r_ble_api.h"
#include "rm_ble_abs.h"
#endif

#include "r_ble_profile_cmn.h"

/** @defgroup profile_cmn_macro Macros 
 *  @{
 *  @brief Macro definition
 */
/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/** 
 * @def BLE_PRF_MAX_NUM_OF_SERVS
 * @brief Maximum Number of Services.
 */
#define BLE_PRF_MAX_NUM_OF_SERVS (10)

/** 
 * @def BLE_SERV_CLI_CNFG_LEN
 * @brief Length of Client Characteristic Configuration descriptor.
 */
#define BLE_SERV_CLI_CNFG_LEN (2)

/** 
 * @def BLE_SERV_SER_CNFG_LEN
 * @brief Length of Server Characteristic Configuration descriptor.
 */
#define BLE_SERV_SER_CNFG_LEN (2)

/** 
 * @def BLE_SERV_CLI_CNFG_UUID
 * @brief UUID of Client Characteristic Configuration descriptor.
 */
#define BLE_SERV_CLI_CNFG_UUID (0x2902)

/** 
 * @def BLE_SERV_SER_CNFG_UUID
 * @brief UUID of Server Characteristic Configuration descriptor.
 */
#define BLE_SERV_SER_CNFG_UUID (0x2903)

#if defined(__CCRX__) || defined(__ICCRX__)
/*RX23W*/
#define BLE_PRF_MTU_SIZE (BLE_CFG_GATT_MTU_SIZE)
#define BLE_PRF_CONN_MAX (BLE_CFG_RF_CONN_MAX)
#else
/*RA4W*/
#define BLE_PRF_MTU_SIZE (BLE_ABS_CFG_GATT_MTU_SIZE)
#define BLE_PRF_CONN_MAX (BLE_ABS_CFG_RF_CONNECTION_MAXIMUM)
#endif
/*@}*/

/** @defgroup profile_cmn_struct Structures
 *  @{
 *  @brief Structure definition
 */
/******************************************************************************************************************//**
 * @struct st_ble_seq_data_t
 * @brief  st_ble_seq_data_t is value field for variable length.
 **********************************************************************************************************************/
typedef struct {
    /**
     * @brief Data value.
     */
    uint8_t *data;
    /**
     * @brief Data length.
     */
    uint16_t len;
} st_ble_seq_data_t;
/*@}*/

/** @defgroup profile_cmn_func Functions
 *  @{
 *  @brief Function definition
 */
/******************************************************************************************************************//**
 * @brief   Decode data value for 8bit. 
 * @param[out]  p_app_value Application data value of characteristic or descriptor.
 * @param[in]   p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t decode_8bit(uint8_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Encode data value for 8bit. 
 * @param[in]   p_app_value Application data value of characteristic or descriptor.
 * @param[out]  p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t encode_8bit(const uint8_t *p_app_value, st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Decode data value for 16bit. 
 * @param[out]  p_app_value Application data value of characteristic or descriptor.
 * @param[in]   p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t decode_16bit(uint16_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Encode data value for 16bit. 
 * @param[in]   p_app_value Application data value of characteristic or descriptor.
 * @param[out]  p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t encode_16bit(const uint16_t *p_app_value, st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Decode data value for 32bit. 
 * @param[out]  p_app_value Application data value of characteristic or descriptor.
 * @param[in]   p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t decode_32bit(uint32_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Encode data value for 32bit. 
 * @param[in]   p_app_value Application data value of characteristic or descriptor.
 * @param[out]  p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t encode_32bit(const uint32_t *p_app_value, st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Decode data value for 24bit. 
 * @param[out]  p_app_value Application data value of characteristic or descriptor.
 * @param[in]   p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t decode_24bit(uint32_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Encode data value for 24bit. 
 * @param[in]   p_app_value Application data value of characteristic or descriptor.
 * @param[out]  p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t encode_24bit(const uint32_t * p_app_value, st_ble_gatt_value_t * p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Decode data value for 8bit array. 
 * @param[out]  p_app_value Application data value of characteristic or descriptor.
 * @param[in]   p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t decode_allcopy(uint8_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Encode data value for 8bit array. 
 * @param[in]   p_app_value Application data value of characteristic or descriptor.
 * @param[out]  p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t encode_allcopy(const uint8_t *p_app_value, st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Decode data value for value type st_ble_seq_data_t. 
 * @param[out]  p_app_value Application data value of characteristic or descriptor.
 * @param[in]   p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t decode_st_ble_seq_data_t(st_ble_seq_data_t *p_app_value, const st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Encode data value for value type st_ble_seq_data_t. 
 * @param[in]   p_app_value Application data value of characteristic or descriptor.
 * @param[out]  p_gatt_value GATT database value and length of characteristic or descriptor.
 * @return  See @ref ble_status_t
 **********************************************************************************************************************/
ble_status_t encode_st_ble_seq_data_t(const st_ble_seq_data_t *p_app_value, st_ble_gatt_value_t *p_gatt_value);

/******************************************************************************************************************//**
 * @brief   Pack value type of st_ble_ieee11073_sfloat_t to GATT DB. 
 * @param[out]  p_dst GATT database value of characteristic or descriptor.
 * @param[in]   p_src Application value field which is st_ble_ieee11073_sfloat_t type.
 * @return   Position of Pointer.
 **********************************************************************************************************************/
uint8_t pack_st_ble_ieee11073_sfloat_t(uint8_t *p_dst, const st_ble_ieee11073_sfloat_t *p_src);

/******************************************************************************************************************//**
 * @brief   Unpack value type of st_ble_ieee11073_sfloat_t from GATT DB. 
 * @param[out]  p_dst Application value field which is st_ble_ieee11073_sfloat_t type.
 * @param[in]   p_src GATT database value of characteristic or descriptor.
 * @return   Position of Pointer.
 **********************************************************************************************************************/
uint8_t unpack_st_ble_ieee11073_sfloat_t(st_ble_ieee11073_sfloat_t *p_dst, const uint8_t *p_src);

/******************************************************************************************************************//**
 * @brief   Pack value type of st_ble_date_time_t to GATT DB. 
 * @param[out]  p_dst GATT database value of characteristic or descriptor.
 * @param[in]   p_src Application value field which is st_ble_date_time_t type.
 * @return   Position of Pointer.
 **********************************************************************************************************************/
uint8_t pack_st_ble_date_time_t(uint8_t *p_dst, const st_ble_date_time_t *p_src);

/******************************************************************************************************************//**
 * @brief   Unpack value type of st_ble_date_time_t from GATT DB. 
 * @param[out]  p_dst Application value field which is st_ble_date_time_t type.
 * @param[in]   p_src GATT database value of characteristic or descriptor.
 * @return   Position of Pointer.
 **********************************************************************************************************************/
uint8_t unpack_st_ble_date_time_t(st_ble_date_time_t *p_dst, const uint8_t *p_src);
/*@}*/

/** @defgroup profile_cmn_macro Macros 
 *  @{
 *  @brief Macro definition
 */
/** 
 * @def decode_uint8_t
 * @brief Function macro for decoding uint8_t
 */
#define decode_uint8_t decode_8bit

/** 
 * @def encode_uint8_t
 * @brief Function macro for encoding uint8_t
 */
#define encode_uint8_t encode_8bit

/** 
 * @def decode_int8_t
 * @brief Function macro for decoding int8_t
 */
#define decode_int8_t  decode_8bit

/** 
 * @def encode_int8_t
 * @brief Function macro for encoding int8_t
 */
#define encode_int8_t  encode_8bit

/** 
 * @def decode_uint16_t
 * @brief Function macro for decoding uint16_t
 */
#define decode_uint16_t decode_16bit

/** 
 * @def encode_uint16_t
 * @brief Function macro for encoding uint16_t
 */
#define encode_uint16_t encode_16bit

/** 
 * @def decode_int16_t
 * @brief Function macro for decoding int16_t
 */
#define decode_int16_t  decode_16bit

/** 
 * @def encode_int16_t
 * @brief Function macro for encoding int16_t
 */
#define encode_int16_t  encode_16bit

/** 
 * @def decode_uint32_t
 * @brief Function macro for decoding uint32_t
 */
#define decode_uint32_t decode_32bit

/** 
 * @def encode_uint32_t
 * @brief Function macro for encoding uint32_t
 */
#define encode_uint32_t encode_32bit

/** 
 * @def decode_int32_t
 * @brief Function macro for decoding int32_t
 */
#define decode_int32_t  decode_32bit

/** 
 * @def encode_int32_t
 * @brief Function macro for encoding int32_t
 */
#define encode_int32_t  encode_32bit
/*@}*/

#endif /* R_BLE_SERV_COMMON_H */
/*@}*/
