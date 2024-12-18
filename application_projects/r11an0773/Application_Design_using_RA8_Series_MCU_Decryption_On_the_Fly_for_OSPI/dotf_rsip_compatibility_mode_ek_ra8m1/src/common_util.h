/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_


/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/
#include "hal_data.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define RESET_VALUE             (0x00)
#define APP_ERROR_TRAP(err)        if(err) { __asm("BKPT #0\n");} /* system execution breaks  */

#define CPU_CLOCK	R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_CPUCLK)
#define MULTIPICATION_FACTOR_TO_NANOSECOND	(1000)
#define SCALE_DWT_TO_MS	(CPU_CLOCK/CONVERT_SEC_TO_MILLISEC)

/** Common error codes */
typedef enum e_app_err
{
    APP_SUCCESS 				        = 0,

    APP_OSPI_OPEN                       = 1,                    ///< OSPI open API failed
    APP_OSPI_CLOSE                      = 2,                    ///< OSPI close API failed
    APP_OSPI_DIRECT_TRANSFER            = 3,                    ///< OSPI direct transfer API failed
    APP_OSPI_SPI_PROTOCOL_SET           = 4,                    ///< OSPI protocol set failed
    APP_FIBONACCI_CALCULATION           = 5,                    ///< fibonacci calculation failed
    APP_OSPI_OPERATION_TIMEOUT          = 6,                    ///< get OSPI status failed
    APP_OSPI_DOTF                       = 7,                    ///< OSPI DOTF operation failed
    APP_OSPI_ERASE                      = 8,                    ///< OSPI Erase failed
    APP_OSPI_WRITE                      = 9,                    ///< OSPI Write failed
    APP_OSPI_TIMEOUT                    = 10,                    ///< OSPI operation timeout
    APP_OSPI_STATUSGET                  = 11,                   ///< ospi_b_statusget failed
    APP_OSPI_DECRYPTED_DATA_NOT_MATCH   = 12,                   ///< decrypted data does not match plaintext data


    APP_JLINK_CONSOLE_INIT              = 15,                   ///< jlink console init failed
    APP_JLINK_CONSOLE_OPEN		        = 16,					///< jlink uart open error
    APP_JLINK_SONSOLE_WRITE	            = 17,					///< JLink console write failed
    APP_JLINK_SONSOLE_READ		        = 18,					///< JLink console read failed


    APP_DATA_ARRAY_NOT_MULTIPLE_OF_16   = 25,                   ///< Data array size is not multiple of 16 byte
    APP_WRAPPED_KEY_GEN                 = 26,                   ///< Wrapped Key Generation failed
    APP_RUNTIME_ENCRYPTION              = 27,                   ///< Runtime encryption failed

    APP_MBEDTLS_SETUP                   = 31,                   ///< emBedTLS Platform Setup failed
    APP_PSA_CRYPTO_INIT                 = 32,                   ///< PSA Crypto Init failed
    APP_PSA_GENERAT_KEY                 = 33,                   ///< psa_generate_key API failed
    APP_PSA_EXPORT_KEY                  = 34,                   ///< psa_export_key API failed
    APP_PSA_CIPHER_ENC_SETUP            = 35,                   ///< psa_cipher_encrypt_setup failed
    APP_PSA_CIPHER_SET_IV               = 36,                   ///< psa_cipher_encryption_setup API failed
    APP_PSA_CIPHER_UPDATE               = 37,                   ///< psa_cipher_update API failed
    APP_PSA_CIPHER_FINISH               = 38,                   ///< psa_cipher_finish API failed



    APP_ERR                             = 99,                   ///< Error code for none API error


} app_err_t;


/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

extern char sprintf_buffer[];


/**********************************************************************************************************************
 * Global Function Prototypes
 **********************************************************************************************************************/

extern void handle_error (uint32_t err, app_err_t err_enum);

#endif /* COMMON_UTIL_H_ */
