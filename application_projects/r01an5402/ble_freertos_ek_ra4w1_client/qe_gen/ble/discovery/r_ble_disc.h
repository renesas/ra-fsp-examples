/**********************************************************************************************************************
* Copyright (c) 2018 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup discovery GATT Discovery Library
 * @{
 * @ingroup profile_cmn
 * @brief GATT Discovery Library
 * @details This library provides APIs to discovery remote GATT database.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 23.08.2019 1.00    First Release
***********************************************************************************************************************/

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

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifndef R_BLE_DISC_H
#define R_BLE_DISC_H

/** @defgroup discovery_macro Macros 
 *  @{
 *  @brief Macro definition
 */
/*******************************************************************************************************************//**
 * @brief Maximum number of services to discover.
***********************************************************************************************************************/
#define BLE_DISC_PRIM_SERV_MAX_NUM (10)

/*******************************************************************************************************************//**
 * @brief Maximum number of included services to discover.
***********************************************************************************************************************/
#define BLE_DISC_INC_SERV_MAX_NUM (2)

/*******************************************************************************************************************//**
 * @brief Maximum number of characteristics to discover in a service.
***********************************************************************************************************************/
#define BLE_DISC_CHAR_MAX_NUM      (40)

/*******************************************************************************************************************//**
 * @brief Maximum number of descriptors to discover in a characteristic
***********************************************************************************************************************/
#define BLE_DISC_DESC_MAX_NUM      (10)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief GATT Discovery event.
***********************************************************************************************************************/
typedef enum
{
    BLE_DISC_PRIM_SERV_FOUND,     /**< Service discovered event */
    BLE_DISC_INC_SERV_FOUND,      /**< Included service discovered event */
    BLE_DISC_PRIM_SERV_NOT_FOUND, /**< Service not discovered event */
    BLE_DISC_CHAR_FOUND,          /**< Characteristic discovered event */
} e_ble_disc_evt_t;
/*@}*/

/** @defgroup discovery_callback Callbacks
 *  @{
 *  @brief Callback definition
 */
/*******************************************************************************************************************//**
 * @brief Callback invoked when discovery is completed.
 * @param conn_hdl connection handle
***********************************************************************************************************************/
typedef void (* ble_disc_comp_cb_t)(uint16_t conn_hdl);

/*******************************************************************************************************************//**
 * @brief Callback invoked when service discovered.
 * @param conn_hdl connection handle
***********************************************************************************************************************/
typedef void (* ble_disc_serv_cb_t)(uint16_t conn_hdl, uint8_t idx, uint16_t type, void *p_param);
/*@}*/

/** @defgroup discovery_struct Structures
 *  @{
 *  @brief Structure definition
 */
/*******************************************************************************************************************//**
 * @brief Structure of service uuid to discover.
***********************************************************************************************************************/
typedef struct st_ble_disc_entry_t
{
    uint8_t                     idx;              /**< Service Index */
    uint8_t                    *p_uuid;           /**< Service UUID */
    uint8_t                     uuid_type;        /**< Service UUID type */
    ble_disc_serv_cb_t          serv_cb;          /**< Service discovery callback */
    uint8_t                     num_of_inc_servs; /**< Number of included services */
    struct st_ble_disc_entry_t *inc_servs;        /**< Included service entries */
} st_ble_disc_entry_t;

/*******************************************************************************************************************//**
 * @brief Discovered service information.
***********************************************************************************************************************/
typedef struct
{
    uint8_t uuid_type; /**< Discovered service UUID type */
    union
    {
        st_ble_gattc_serv_16_evt_t  serv_16;
        st_ble_gattc_serv_128_evt_t serv_128;
    } value;
} st_disc_serv_param_t;

/*******************************************************************************************************************//**
 * @brief Discovered included service information.
***********************************************************************************************************************/
typedef struct
{
    uint8_t uuid_type; /**< Discovered service UUID type */
    union
    {
        st_ble_gattc_inc_serv_16_evt_t  inc_serv_16;
        st_ble_gattc_inc_serv_128_evt_t inc_serv_128;
    } value;
} st_disc_inc_serv_param_t;

/*******************************************************************************************************************//**
 * @brief Discovered descriptor information.
***********************************************************************************************************************/
typedef struct
{
    uint8_t uuid_type; /**< Discovered descriptor UUID type */
    union
    {
        st_ble_gattc_char_desc_16_evt_t  desc_16;
        st_ble_gattc_char_desc_128_evt_t desc_128;
    } value;
} st_disc_desc_param_t;

/*******************************************************************************************************************//**
 * @brief Discovered characteristic information.
***********************************************************************************************************************/
typedef struct
{
    uint8_t uuid_type; /**< Discovered characteristic UUID type */
    union
    {
        st_ble_gattc_char_16_evt_t  char_16;
        st_ble_gattc_char_128_evt_t char_128;
    } value;

    uint8_t num_of_descs;
    st_disc_desc_param_t *descs;
} st_disc_char_param_t;
/*@}*/

/** @defgroup discovery_func Functions
 *  @{
 *  @brief Function definition
 */

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @fn ble_status_t R_BLE_DISC_Init(void)
 * @brief Initialize the GATT services discovery function.
 * @return See @ref ble_status_t
***********************************************************************************************************************/
ble_status_t R_BLE_DISC_Init(void);

/*******************************************************************************************************************//**
 * @fn ble_status_t R_BLE_DISC_Start(uint16_t conn_hdl, 
 *                                   const st_ble_disc_entry_t *p_entries, 
 *                                   uint8_t num_of_entires, 
 *                                   ble_disc_comp_cb_t cb
 *                                   )
 * @brief Start GATT service discovery.
 * @param[in] conn_hdl connection handle
 * @param[in] p_entries The information of services to be detected.
 * @param[in] num_of_entires The number of services to be detected.
 * @param[in] cb The callback function which is called when the service has been detected.
 * @return See @ref ble_status_t
***********************************************************************************************************************/
ble_status_t R_BLE_DISC_Start(uint16_t conn_hdl, const st_ble_disc_entry_t *p_entries, uint8_t num_of_entires, ble_disc_comp_cb_t cb);
/*@}*/

#endif /* R_BLE_DISC_H */

/** @} */
