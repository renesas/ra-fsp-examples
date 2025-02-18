/**********************************************************************************************************************
* Copyright (c) 2018 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#ifndef R_BLE_PROFILE_CMN_H
#define R_BLE_PROFILE_CMN_H

#ifdef ENABLE_PROFILE_UT_TEST
#define UT_MOCK(name) __mock_ ## name
#else
#define UT_MOCK(name) name
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array)\
        (sizeof(array) / sizeof(array[0]))
#endif

/*******************************************************************************************************************//**
 * @file
 * @defgroup profile_cmn Profile Common Library
 * @{
 * @ingroup app_lib
 * @brief Profile Common Library
 * @details This library provides APIs to encode/decode default type and data types.
***********************************************************************************************************************/
/** @defgroup profile_cmn_utility_macro Utility Macros
 *  @{
 *  @brief Utility Macros from Profile Common Library
 *  @details Packing Macros and Unpacking Macros for Little Endian and MIN/MAX Macro are defined.\n
 *  By default both the packing and unpaking macros uses pointer to
 *  a single or multi-octet variable which to be packed to or unpacked from
 *  a buffer (unsinged character array).\n
 */

/** 
 * @def BT_PACK_LE_1_BYTE
 * Little Endian Packing Macros for 1 byte (uint8_t for source).
 */
#ifndef BT_PACK_LE_1_BYTE
#define BT_PACK_LE_1_BYTE(dst, src) \
    { \
        uint8_t val; \
        val = (uint8_t)(*(src)); \
        BT_PACK_LE_1_BYTE_VAL((dst), val); \
    }
#endif

/** 
 * @def BT_PACK_LE_1_BYTE_VAL
 * Little Endian Packing Macros for 1 byte (uint8_t[1] for source).
 */
#ifndef BT_PACK_LE_1_BYTE_VAL
#define BT_PACK_LE_1_BYTE_VAL(dst, src) \
    *((uint8_t *)(dst) + 0) = (src);
#endif

/** 
 * @def BT_PACK_LE_2_BYTE
 * Little Endian Packing Macros for 2 byte (uint16_t for source).
 */
#ifndef BT_PACK_LE_2_BYTE
#define BT_PACK_LE_2_BYTE(dst, src) \
    { \
        uint16_t val; \
        val = (uint16_t)(*(src)); \
        BT_PACK_LE_2_BYTE_VAL((dst), val); \
    }
#endif

/** 
 * @def BT_PACK_LE_2_BYTE_VAL
 * Little Endian Packing Macros for 2 byte (uint8_t[2] for source).
 */
#ifndef BT_PACK_LE_2_BYTE_VAL
#define BT_PACK_LE_2_BYTE_VAL(dst, src) \
    *((uint8_t *)(dst) + 0) = (uint8_t)(src); \
    *((uint8_t *)(dst) + 1) = (uint8_t)((src) >> 8);
#endif

/** 
 * @def BT_PACK_LE_3_BYTE
 * Little Endian Packing Macros for 3 byte (uint32_t for source)..
 */
#ifndef BT_PACK_LE_3_BYTE
#define BT_PACK_LE_3_BYTE(dst, src) \
    { \
        uint32_t val; \
        val = (uint32_t)(*(src)); \
        BT_PACK_LE_3_BYTE_VAL((dst), val); \
    }
#endif

/** 
 * @def BT_PACK_LE_3_BYTE_VAL
 * Little Endian Packing Macros for 3 byte (uint8_t[3] for source).
 */
#ifndef BT_PACK_LE_3_BYTE_VAL
#define BT_PACK_LE_3_BYTE_VAL(dst, src) \
    *((uint8_t *)(dst) + 0) = (uint8_t)(src);\
    *((uint8_t *)(dst) + 1) = (uint8_t)((src) >> 8);\
    *((uint8_t *)(dst) + 2) = (uint8_t)((src) >> 16);
#endif

/** 
 * @def BT_PACK_LE_4_BYTE
 * Little Endian Packing Macros for 4 byte (uint32_t for source).
 */
#ifndef BT_PACK_LE_4_BYTE
#define BT_PACK_LE_4_BYTE(dst, src) \
    { \
        uint32_t val; \
        val = (uint32_t)(*(src)); \
        BT_PACK_LE_4_BYTE_VAL((dst), val); \
    }
#endif

/** 
 * @def BT_PACK_LE_4_BYTE_VAL
 * Little Endian Packing Macros for 1 byte (uint8_t[4] for source).
 */
#ifndef BT_PACK_LE_4_BYTE_VAL
#define BT_PACK_LE_4_BYTE_VAL(dst, src) \
    *((uint8_t *)(dst) + 0) = (uint8_t)(src);\
    *((uint8_t *)(dst) + 1) = (uint8_t)((src) >> 8);\
    *((uint8_t *)(dst) + 2) = (uint8_t)((src) >> 16);\
    *((uint8_t *)(dst) + 3) = (uint8_t)((src) >> 24);
#endif

/* Update based on 64 Bit, 128 Bit Data Types */

/** 
 * @def BT_PACK_LE_8_BYTE
 * Little Endian Packing Macros for 8 byte.
 */
#ifndef BT_PACK_LE_8_BYTE
#define BT_PACK_LE_8_BYTE(dst,val)\
        memcpy ((dst), (val), 8)
#endif

/** 
 * @def BT_PACK_LE_16_BYTE
 * Little Endian Packing Macros for 16 byte.
 */
#ifndef BT_PACK_LE_16_BYTE
#define BT_PACK_LE_16_BYTE(dst,val)\
        memcpy ((dst), (val), 16)
#endif

/** 
 * @def BT_PACK_LE_N_BYTE
 * Little Endian Packing Macros for variable length.
 */
#ifndef BT_PACK_LE_N_BYTE
#define BT_PACK_LE_N_BYTE(dst,val,n)\
        memcpy ((dst), (val), (n))
#endif

/** 
 * @def BT_UNPACK_LE_1_BYTE
 * Little Endian Unpacking Macros for 1 byte.
 */
#ifndef BT_UNPACK_LE_1_BYTE
#define BT_UNPACK_LE_1_BYTE(dst,src)\
    *((uint8_t *)(dst)) = (uint8_t)(*((uint8_t *)(src)));
#endif

/** 
 * @def BT_UNPACK_LE_2_BYTE
 * Little Endian Unpacking Macros for 2 byte.
 */
#ifndef BT_UNPACK_LE_2_BYTE
#define BT_UNPACK_LE_2_BYTE(dst,src)\
        *((uint16_t *)(dst)) = (uint16_t)( \
                        (((uint16_t)(*((src) + 0))) <<  0) | \
                        (((uint16_t)(*((src) + 1))) <<  8)   \
                    );
#endif

/** 
 * @def BT_UNPACK_LE_3_BYTE
 * Little Endian Unpacking Macros for 3 byte.
 */
#ifndef BT_UNPACK_LE_3_BYTE
#define BT_UNPACK_LE_3_BYTE(dst,src)\
        *((uint32_t *)(dst)) = (uint32_t)( \
                        (((uint32_t)(*((src) + 0))) <<  0) | \
                        (((uint32_t)(*((src) + 1))) <<  8) | \
                        (((uint32_t)(*((src) + 2))) << 16)   \
                    );
#endif

/** 
 * @def BT_UNPACK_LE_4_BYTE
 * Little Endian Unpacking Macros for 4 byte.
 */
#ifndef BT_UNPACK_LE_4_BYTE
#define BT_UNPACK_LE_4_BYTE(dst,src)\
        *((uint32_t *)(dst)) = (uint32_t)( \
                        (((uint32_t)(*((src) + 0))) <<  0) | \
                        (((uint32_t)(*((src) + 1))) <<  8) | \
                        (((uint32_t)(*((src) + 2))) << 16) | \
                        (((uint32_t)(*((src) + 3))) << 24)   \
                    );
#endif

/* Update based on 64 Bit, 128 Bit Data Types */

/** 
 * @def BT_UNPACK_LE_8_BYTE
 * Little Endian Unpacking Macros for 8 byte.
 */
#ifndef BT_UNPACK_LE_8_BYTE
#define BT_UNPACK_LE_8_BYTE(dst,src)\
        memcpy ((dst), (src), 8)
#endif

/** 
 * @def BT_UNPACK_LE_16_BYTE
 * Little Endian Unpacking Macros for 16 byte.
 */
#ifndef BT_UNPACK_LE_16_BYTE
#define BT_UNPACK_LE_16_BYTE(dst,src)\
        memcpy ((dst), (src), 16)
#endif

/** 
 * @def BT_UNPACK_LE_N_BYTE
 * Little Endian Unpacking Macros for variable length.
 */
#ifndef BT_UNPACK_LE_N_BYTE
#define BT_UNPACK_LE_N_BYTE(dst,src,n)\
        memcpy ((dst), (src), (n))
#endif

/** 
 * @def MAX
 * Defines which value is bigger.
 */
#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

/** 
 * @def MIN
 * Defines which value is smaller.
 */
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

/*@}*/

/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 23.08.2019 1.00    First Release
*         : 31.10.2019 1.01    Add doxygen comments.
***********************************************************************************************************************/

/** @defgroup profile_cmn_struct Structures
 *  @{
 *  @brief Structure definition
 */
/*******************************************************************************************************************//**
 * @brief IEEE 11073 FLOAT type.
***********************************************************************************************************************/
typedef struct {
    int8_t  exponent; /**< 8-bit exponent to base 10 */
    int32_t mantissa; /**< 24-bit mantissa */
} st_ble_ieee11073_float_t;

/*******************************************************************************************************************//**
 * @brief IEEE 11073 short FLOAT type.
***********************************************************************************************************************/
typedef struct {
    int8_t  exponent; /**< 4-bit exponent to base 10 */
    int16_t mantissa; /**< 12-bit mantissa */
} st_ble_ieee11073_sfloat_t;

/*******************************************************************************************************************//**
 * @brief Date Time characteristic parameters.
***********************************************************************************************************************/
typedef struct {
    uint16_t year;    /**< Year */
    uint8_t  month;   /**< Month */
    uint8_t  day;     /**< Day */
    uint8_t  hours;   /**< Hours */
    uint8_t  minutes; /**< Minutes */
    uint8_t  seconds; /**< Seconds */
} st_ble_date_time_t;
/*@}*/

#endif /* R_BLE_PROFILE_CMN_H */
/*@}*/