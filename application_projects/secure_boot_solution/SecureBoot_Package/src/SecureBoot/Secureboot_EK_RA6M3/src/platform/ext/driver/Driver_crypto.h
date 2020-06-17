/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * This file is part of Renesas Flexible Software Package (FSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : r_crypto_api.h
 * Description  : Crypto_Interface
 ***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *           Crypto_Interface  1.00    Initial Release.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup Interface_Library
 * @defgroup Crypto_API Crypto Interface
 *
 * @brief Cryptographic algorithm APIs for encryption/decryption, signing/verification, and hashing.
 *
 * @{
 **********************************************************************************************************************/

#ifndef DRV_CRYPTO_API_H
#define DRV_CRYPTO_API_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
/** Register definitions, common services and error codes. */

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define CRYPTO_API_VERSION_MAJOR (01)
#define CRYPTO_API_VERSION_MINOR (00)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Crypto_Interface Add API definitions required by user here. */
typedef struct st_crypto_ctrl
{
    uint32_t  state; //!< indicates state of the SCE/SCE-Lite driver e.g whether it is initialized
    uint32_t  cb_data;
    void (*p_sce_long_plg_start_callback)(void);
    void (*p_sce_long_plg_end_callback)(void);
} crypto_ctrl_t;

/**********************************************************************************************************************
 * Enumerations
 **********************************************************************************************************************/

/** Enumerator for Crypto API uint32_t[] array word endian selection */
typedef enum e_crypto_word_endian
{
    CRYPTO_WORD_ENDIAN_BIG = 0,
    CRYPTO_WORD_ENDIAN_LITTLE = 1
} crypto_word_endian_t;

/** MIN and MAX values under enums are for internal use only */

/** Enumerated Crypto Algorithm Type */
typedef enum e_crypto_algorithm_type
{
    CRYPTO_ALGORITHM_TYPE_MIN,
    CRYPTO_ALGORITHM_TYPE_RSA,
    CRYPTO_ALGORITHM_TYPE_HASH,
    CRYPTO_ALGORITHM_TYPE_AES,
    CRYPTO_ALGORITHM_TYPE_TRNG,
    CRYPTO_ALGORITHM_TYPE_ECC,
    /* Add any new entries here */
    CRYPTO_ALGORITHM_TYPE_MAX
}crypto_algorithm_type_t;

/** Enumerated HASH Types */
typedef enum e_crypto_type_hash
{
    CRYPTO_TYPE_HASH_MIN,
    CRYPTO_TYPE_HASH_1,
    CRYPTO_TYPE_HASH_224,
    CRYPTO_TYPE_HASH_256,
    /* Add any new entries here */
    CRYPTO_TYPE_HASH_MD5,
    CRYPTO_TYPE_HASH_MAX
}crypto_hash_type_t;

/** Enumerated Key Types */
typedef enum e_crypto_key_type
{
    CRYPTO_KEY_TYPE_MIN,
    CRYPTO_KEY_TYPE_AES_PLAIN_TEXT,
    CRYPTO_KEY_TYPE_AES_WRAPPED,
    CRYPTO_KEY_TYPE_RSA_PLAIN_TEXT,
    CRYPTO_KEY_TYPE_RSA_CRT_PLAIN_TEXT,
    CRYPTO_KEY_TYPE_RSA_WRAPPED,
    CRYPTO_KEY_TYPE_ECC_PLAIN_TEXT,
    CRYPTO_KEY_TYPE_ECC_WRAPPED,
    /* Add any new entries here */
    CRYPTO_KEY_TYPE_MAX
}crypto_key_type_t;

/** Enumerated Key Sizes */
typedef enum e_crypto_key_size
{
    CRYPTO_KEY_SIZE_MIN,
    CRYPTO_KEY_SIZE_AES_128,
    CRYPTO_KEY_SIZE_AES_256,
    CRYPTO_KEY_SIZE_AES_192,
    CRYPTO_KEY_SIZE_RSA_1024,
    CRYPTO_KEY_SIZE_RSA_2048,
    CRYPTO_KEY_SIZE_ECC_256,
    CRYPTO_KEY_SIZE_ECC_192,
    /* Add any new entries here */
    CRYPTO_KEY_SIZE_MAX
}crypto_key_size_t;

/** Enumerated chaining modes */
typedef enum e_crypto_chaining_mode
{
    CRYPTO_CHAINING_MODE_MIN,
    CRYPTO_ECB_MODE,
    CRYPTO_CBC_MODE,
    CRYPTO_CTR_MODE,
    CRYPTO_GCM_MODE,
    CRYPTO_XTS_MODE,
    /* Add any new entries here */
    CRYPTO_CHAINING_MODE_MAX,
}crypto_chaining_mode_t;

/**********************************************************************************************************************
 * Structures
 **********************************************************************************************************************/

/** A structure to handle data among Crypto HAL modules */
typedef  struct r_crypto_data_handle
{
    uint32_t   * p_data;             ///< Pointer to data
    uint32_t    data_length;         ///< The length of data pointed by p_data
} r_crypto_data_handle_t;

/** Crypto engine configuration parameters */
typedef struct st_crypto_cfg
{
    void (*p_sce_long_plg_start_callback)(void);

    /** Callback provided when a ISR occurs.  Set to NULL for no CPU interrupt. */
    void (*p_sce_long_plg_end_callback)(void);

    /** Endian flag, indicates word endianness for the uint32_t[] array inputs used in Crypto APIs */
    crypto_word_endian_t endian_flag;
} crypto_cfg_t;


/** Parameters for requesting HAL API interface object */
typedef struct r_crypto_interface_get_param
{
    crypto_algorithm_type_t      algorithm_type;
    crypto_key_type_t            key_type;
    crypto_key_size_t            key_size;
    crypto_chaining_mode_t       chaining_mode;
    crypto_hash_type_t           hash_type;
}crypto_interface_get_param_t;

/** Crypto_Interface SCE functions implemented at the HAL layer will follow this API. */
typedef struct st_crypto_api
{
    /** Open crypto module using the given configuration
     * @param[in,out] p_ctrl pointer to control structure. Must be declared by user. Elements set here.
     * @param[in]     p_cfg  pointer to configuration structure. All elements of this structure must be set by user
     */
    uint32_t (* open)(crypto_ctrl_t * const p_ctrl, crypto_cfg_t const * const p_cfg);

    /** Close the crypto interface module for the given control structure p_ctrl
     * @param[in] p_ctrl pointer to control structure
     */
    uint32_t (* close)(crypto_ctrl_t * const p_ctrl);

    /** Get API interface structure object based on the interface_info provided.
     * @param[in] interface_info    pointer to structure containing requested interface information.
     * @param[out] p_interface      pointer whose value points to interface structure object.
     *
     * @note p_interface must be of pointer type and its address must be passed in this API.
     *       Passing the pointer and not its address may result in undefined behavior at run-time.
     * @note Value of p_interface is allowed to be passed as NULL at the time of API call.
     */
    uint32_t (*interfaceGet)(crypto_interface_get_param_t * const interface_info, void * const  p_interface);

    /** Get status of SCE initialization
     * @param[out] p_status initialization status of SCE module will be written to the memory pointed to by p_status
     */
    uint32_t (* statusGet)(uint32_t * p_status);

    /** Gets version and stores it in provided pointer p_version.
     * @param[out]    p_version  Code and API version used.
     */
    uint32_t (* versionGet)(fsp_version_t * const p_version);
} crypto_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_crypto_instance {
    crypto_ctrl_t   * p_ctrl ;         ///< Pointer to the control structure for this instance
    crypto_cfg_t    const * p_cfg  ;   ///< Pointer to the configuration structure for this instance
    crypto_api_t    const * p_api  ;   ///< Pointer to the API structure for this instance
} crypto_instance_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure. */
//extern const crypto_api_t g_sce_crypto_api;
/** @endcond */

#endif /* DRV_CRYPTO_API_H */

/*******************************************************************************************************************//**
 * @} (end defgroup Crypto_API)
 **********************************************************************************************************************/
