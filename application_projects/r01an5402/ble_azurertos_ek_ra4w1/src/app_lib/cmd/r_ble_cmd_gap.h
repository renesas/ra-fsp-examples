/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019-2020 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup cmd_gap GAP Command
 * @{
 * @ingroup app_lib
 * @brief GAP Commands
 * @details Provides CLI for GAP. And exposes APIs used in the commands.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 15.12.2020 1.00    First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "cli/r_ble_cli.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifndef R_BLE_CMD_GAP_H
#define R_BLE_CMD_GAP_H

/*******************************************************************************************************************//**
 * @brief Default advertising handle used in this library.
***********************************************************************************************************************/
#define BLE_CMD_DEFAULT_ADV_HDL   (0x00)

/*******************************************************************************************************************//**
 * @brief Default advertising interval used in this library.
***********************************************************************************************************************/
#define BLE_CMD_DEFAULT_ADV_INTV  (0x300)

/*******************************************************************************************************************//**
 * @brief Default scan interval used in this library.
***********************************************************************************************************************/
#define BLE_CMD_DEFAULT_SCAN_INTV (0x300)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Advertising data type.
***********************************************************************************************************************/
typedef enum
{
    BLE_CMD_ADV_DATA_TYPE_FLAGS                   = 0x01, /**< Flags */
    BLE_CMD_ADV_DATA_TYPE_INCOMP_16BIT_UUID_LIST  = 0x02, /**< Incomplete List of 16-bit Service Class UUIDs */
    BLE_CMD_ADV_DATA_TYPE_COMP_16BIT_UUID_LIST    = 0x03, /**< Complete List of 16-bit Service Class UUIDs */
    BLE_CMD_ADV_DATA_TYPE_INCOMP_128BIT_UUID_LIST = 0x06, /**< Incomplete List of 128-bit Service Class UUIDs */
    BLE_CMD_ADV_DATA_TYPE_COMP_128BIT_UUID_LIST   = 0x07, /**< Complete List of 128-bit Service Class UUIDs */
    BLE_CMD_ADV_DATA_TYPE_SHORT_LOCAL_NAME        = 0x08, /**< Shortened Local Name */
    BLE_CMD_ADV_DATA_TYPE_COMP_LOCAL_NAME         = 0x09, /**< Complete Local Name */
    BLE_CMD_ADV_DATA_TYPE_SLAVE_CONN_INTV_RANGE   = 0x12, /**< Slave Connection Interval Range */
    BLE_CMD_ADV_DATA_TYPE_TX_POWER_LEVEL          = 0x0A, /**< Tx Power Level */
    BLE_CMD_ADV_DATA_TYPE_SERV_DATA               = 0x16, /**< Service Data */
    BLE_CMD_ADV_DATA_TYPE_PUBLIC_ADDR             = 0x17, /**< Public Target Address */
    BLE_CMD_ADV_DATA_TYPE_RAND_ADDR               = 0x18, /**< Random Target Address */
    BLE_CMD_ADV_DATA_TYPE_APPEARANCE              = 0x19, /**< Appearance */
    BLE_CMD_ADV_DATA_TYPE_ADV_INTV                = 0x1A, /**< Advertising Interval */
} e_ble_cmd_adv_data_type_t;

/*******************************************************************************************************************//**
 * @brief Callback used to inform filtered advertising report.
 * @param addr BD address
 * @param addr_type BD address type
***********************************************************************************************************************/
typedef void (*ble_cmd_adv_rept_cb_t)(uint8_t *addr, uint8_t addr_type);

/*******************************************************************************************************************//**
 * @brief Advertising report filtering criteria.
 * @details @c len should be the length AD Data only.
***********************************************************************************************************************/
typedef struct
{
    uint8_t  ad_type;  /**< AD Type */
    uint8_t *ad_data;  /**< AD Data */
    uint16_t len;      /**< AD Data length */
} ble_cmd_adv_rept_filter_t;

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

/** @brief GAP command definition. */
extern const st_ble_cli_cmd_t g_gap_cmd;

/*******************************************************************************************************************//**
 * @brief GAP event handler.
 * @details This function shall be called from GATTS callback registred by @ref R_BLE_GAP_RegisterCb function.
***********************************************************************************************************************/
void R_BLE_CMD_GapCb(uint16_t type, ble_status_t result, st_ble_evt_data_t *data);

/*******************************************************************************************************************//**
 * @brief Start advertising.
 * @details
 * This function starts advertising with advertising data.
 * @param[in] p_adv_data Advertising data
 * @param[in] data_len   Advertising data length
***********************************************************************************************************************/
void R_BLE_CMD_StartLegacyAdv(uint8_t *p_adv_data, uint8_t data_len);

/*******************************************************************************************************************//**
 * @brief Start advertising(Non connectable).
 * @details
 * This function starts advertising with advertising data.(non connectable)
 * @param[in] p_adv_data Advertising data
 * @param[in] data_len   Advertising data length
***********************************************************************************************************************/
void R_BLE_CMD_StartLegacyAdvNonConn(uint8_t *p_adv_data, uint8_t data_len);

/*******************************************************************************************************************//**
 * @brief Stop advertising.
 * @details This function stops advertising started by @ref R_BLE_CMD_StartLegacyAdv.
***********************************************************************************************************************/
void R_BLE_CMD_StopLegacyAdv(void);

/*******************************************************************************************************************//**
 * @brief Start scanning.
 * @details
 * This function starts scanning. @c cb is involed when advertising report is received.
 * If @c filters is not NULL, only filtered advertising report is reported.
 * @param[in] cb The callback invoked when filtered advertising report is received.
 * @param[in] filters Advertising report filtering setting.
 **********************************************************************************************************************/
void R_BLE_CMD_StartLegacyScan(ble_cmd_adv_rept_cb_t cb, ble_cmd_adv_rept_filter_t *filters);

/*******************************************************************************************************************//**
 * @brief Stop scanning.
 * @details This function stops scanning started by @ref R_BLE_CMD_StartLegacyScan.
 **********************************************************************************************************************/
void R_BLE_CMD_StopLegacyScan(void);

/*******************************************************************************************************************//**
 * @brief Create Connection.
 * @details This function create connection.
 **********************************************************************************************************************/
void R_BLE_CMD_CreateConn(uint8_t *addr, uint8_t addr_type);

/*******************************************************************************************************************//**
 * @brief Start Periodic Advertising.
 * @details This function start periodic advertising.
 **********************************************************************************************************************/
void R_BLE_CMD_StartPerdAdv(void);

/*******************************************************************************************************************//**
 * @brief Stop Periodic Advertising.
 * @details This function stop periodic advertising.
 **********************************************************************************************************************/
void R_BLE_CMD_StopPerdAdv(void);


#endif /* R_BLE_CMD_GAP_H */

/** @} */
