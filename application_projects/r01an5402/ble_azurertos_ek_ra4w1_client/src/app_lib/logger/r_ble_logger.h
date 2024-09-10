/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @file
 * @defgroup logger
 * @{
 * @ingroup app_lib
 * @brief Logger
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 15.12.2020 1.00    First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifndef R_BLE_LOGGER_H
#define R_BLE_LOGGER_H

/** @defgroup logger_macro Macros 
 *  @{
 *  @brief Macro definition
 */
/*******************************************************************************************************************//**
 * @def BLE_DEFAULT_LOG_LEVEL
 * @brief Set default log level used throughout the project.
 * @details The default value is 3 to output all log level.
***********************************************************************************************************************/
#ifndef BLE_DEFAULT_LOG_LEVEL
#define BLE_DEFAULT_LOG_LEVEL   (1)
#endif

/*******************************************************************************************************************//**
 * @def BLE_LOG_TAG
 * @brief Set module specific log level.
 * @details The default value is "----".
***********************************************************************************************************************/
#ifndef BLE_LOG_TAG
#define BLE_LOG_TAG "----"
#endif

/*******************************************************************************************************************//**
 * @def BLE_LOG_LEVEL
 * @brief Set module specific log level.
 * @details The default value is BLE_DEFAULT_LEVEL.
***********************************************************************************************************************/
#ifndef BLE_LOG_LEVEL
#define BLE_LOG_LEVEL (BLE_DEFAULT_LOG_LEVEL)
#endif

/*******************************************************************************************************************//**
 * @def BLE_LOG_LEVEL_ERR
 * @brief Set module specific log level "error".
 * @details The default value is 1.
***********************************************************************************************************************/
#define BLE_LOG_LEVEL_ERR   (1)

/*******************************************************************************************************************//**
 * @def BLE_LOG_LEVEL_WRN
 * @brief Set module specific log level "warning".
 * @details The default value is 2.
***********************************************************************************************************************/
#define BLE_LOG_LEVEL_WRN   (2)

/*******************************************************************************************************************//**
 * @def BLE_LOG_LEVEL_DBG
 * @brief Set module specific log level "debug".
 * @details The default value is 3.
***********************************************************************************************************************/
#define BLE_LOG_LEVEL_DBG   (3)

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

#if (BLE_DEFAULT_LOG_LEVEL != 0)

#if !(defined(__cplusplus))
/*******************************************************************************************************************//**
 * @def BLE_LOG
 * @brief Output log.
 * @details This macro is only usable when \ref BLE_DEFAULT_LOG_LEVEL is not 0 and \ref BLE_CFG_HCI_MODE_EN is 0.
***********************************************************************************************************************/
#define BLE_LOG(level_val, level_tag, ...)                              \
    do {                                                                \
        if (BLE_LOG_LEVEL >= level_val)                                 \
        {                                                               \
            printf("%s: [%s] (%s:%d) ", BLE_LOG_TAG, level_tag,         \
                   __func__, __LINE__);                                 \
            printf(__VA_ARGS__);                                        \
            printf("\n");                                               \
        }                                                               \
    } while (0)

/*******************************************************************************************************************//**
 * @def BLE_LOG_ERR
 * @brief Output log in error log level.
 * @details This macro is only usable when \ref BLE_LOG_LEVEL is \ref BLE_LOG_LEVEL_DBG or higher.
***********************************************************************************************************************/
#define BLE_LOG_ERR(...) BLE_LOG(BLE_LOG_LEVEL_ERR, "ERR", __VA_ARGS__)

/*******************************************************************************************************************//**
 * @def BLE_LOG_WRN
 * @brief Output log in warning log level.
 * @details This macro is only usable when BLE_LOG_LEVEL is BLE_LOG_LEVEL_DBG or higher.
***********************************************************************************************************************/
#define BLE_LOG_WRN(...) BLE_LOG(BLE_LOG_LEVEL_WRN, "WRN", __VA_ARGS__)

/*******************************************************************************************************************//**
 * @def BLE_LOG_DBG
 * @brief Output log in debug log level.
 * @details This macro is only usable when BLE_LOG_LEVEL is BLE_LOG_LEVEL_DBG or higher.
***********************************************************************************************************************/
#define BLE_LOG_DBG(...) BLE_LOG(BLE_LOG_LEVEL_DBG, "DBG", __VA_ARGS__)

#else /* !(defined(__cplusplus)) */
#define BLE_LOG(level, format, x)
#define BLE_LOG_ERR(x)
#define BLE_LOG_WRN(x)
#define BLE_LOG_DBG(x)
#endif /* !(defined(__cplusplus)) */

#else /* (BLE_DEFAULT_LOG_LEVEL != 0) */

#define BLE_LOG(level, format, x)
#define BLE_LOG_ERR(x)
#define BLE_LOG_WRN(x)
#define BLE_LOG_DBG(x)

#endif /* (BLE_DEFAULT_LOG_LEVEL != 0) */
/*@}*/

/** @defgroup logger_func Functions
 *  @{
 *  @brief Function definition
 */
/*******************************************************************************************************************//**
 * @brief Convert BD address in bytes array into string.
 * @details
 * This function shall be called only in BLE_LOG_XXX argument. Because
 * the return value of this function is the pointer to the interval static buffer
 * which retains the BD address string. 
 * 
 * @param[in] p_addr BD address in byte array
 * @param[in] addr_type BD address type
 *       |            value          |   description     | 
 *       |:------------------------- |:----------------- |
 *       | BLE_GAP_ADDR_PUBLIC(0x00) | Public Address.   | 
 *       | Other than the above      | Random Address.   | 
 * @return BD address in string format
***********************************************************************************************************************/
char *BLE_BD_ADDR_STR(uint8_t *p_addr, uint8_t addr_type);

/*******************************************************************************************************************//**
 * @brief Convert UUID in bytes array into string.
 * @details This function shall be called only in BLE_LOG_XXX argument. Because
 * the return value of this function is the pointer to the interval static buffer
 * which retains the BD address string. 
 *
 * @param[in] p_uuid UUID in byte array
 * @param[in] uuid_type UUID type
 *       |              macro                 | description      | 
 *       |:---------------------------------- |:---------------- |
 *       | BLE_GATT_16_BIT_UUID_FORMAT(0x01)  | 16-bit UUID      | 
 *       | BLE_GATT_128_BIT_UUID_FORMAT(0x02) | 128-bit UUID     | 
 * @return UUID in string format
***********************************************************************************************************************/
char *BLE_UUID_STR(uint8_t *p_uuid, uint8_t uuid_type);
/*@}*/

#endif /* R_BLE_LOGGER_H */
/*@}*/
