/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef APP_COMMON_UTIL_H_
#define APP_COMMON_UTIL_H_


/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/
#include "hal_data.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define RESET_VALUE             (0x00)
#define APP_ERROR_TRAP(err)     if(err) { __asm("BKPT #0\n");} /* system execution breaks  */

#define CPU_CLOCK	                            (R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_CPUCLK))
#define MULTIPICATION_FACTOR_TO_NANOSECOND	    (1000U)
#define SCALE_DWT_TO_MS	                        (CPU_CLOCK / CONVERT_SEC_TO_MILLISEC)

/** Common error codes */
typedef enum e_app_err
{
    APP_SUCCESS = 0,

    APP_OSPI_INIT,                                              ///< OSPI init failed
    APP_OSPI_OPEN,                                              ///< OSPI open API failed
    APP_OSPI_CLOSE,                                             ///< OSPI close API failed
    APP_OSPI_DIRECT_TRANSFER,                                   ///< OSPI direct transfer API failed
    APP_OSPI_SPI_PROTOCOL_SET,                                  ///< OSPI protocol set failed
    APP_OSPI_ERASE,                                             ///< OSPI Erase failed
    APP_OSPI_WRITE,                                             ///< OSPI Write failed
    APP_OSPI_STATUSGET,                                         ///< OSPI Status Get failed
    APP_OSPI_READ,                                              ///< OSPI Read failed
    APP_OSPI_WRITE_ENABLE,                                      ///< OSPI Write Enable failed
    APP_OSPI_OPERATION_TIMEOUT,                                 ///< OSPI timeout failed
    APP_OSPI_DOTF,                                              ///< OSPI DOTF operation failed
    APP_OSPI_DECRYPTED_DATA_NOT_MATCH,                          ///< decrypted data does not match plaintext data
    APP_FIBONACCI_CALCULATION,                                  ///< fibonacci calculation failed
    APP_WRITE_PLAINTEXT_DATA,                                   ///< write plaintext data failed
    APP_WRITE_ENCRYPTED_DATA,                                   ///< write encrypted data failed

    APP_JLINK_CONSOLE_INIT,                                     ///< jlink console init failed
    APP_JLINK_CONSOLE_OPEN,		        					    ///< jlink uart open error
    APP_JLINK_SONSOLE_WRITE,	            					///< JLink console write failed
    APP_JLINK_SONSOLE_READ,		        				        ///< JLink console read failed

    APP_DATA_ARRAY_NOT_MULTIPLE_OF_16,                          ///< Data array size is not multiple of 16 byte
    APP_WRAPPED_KEY_GEN,                                        ///< Wrapped Key Generation failed
    APP_RUNTIME_ENCRYPTION,                                     ///< Runtime encryption failed

    APP_MBEDTLS_SETUP,                                          ///< emBedTLS Platform Setup failed
    APP_PSA_CRYPTO_INIT,                                        ///< PSA Crypto Init failed
    APP_PSA_GENERAT_KEY,                                        ///< psa_generate_key API failed
    APP_PSA_EXPORT_KEY,                                         ///< psa_export_key API failed
    APP_PSA_CIPHER_ENC_SETUP,                                   ///< psa_cipher_encrypt_setup failed
    APP_PSA_CIPHER_SET_IV,                                      ///< psa_cipher_encryption_setup API failed
    APP_PSA_CIPHER_UPDATE,                                      ///< psa_cipher_update API failed
    APP_PSA_CIPHER_FINISH,                                      ///< psa_cipher_finish API failed

    APP_ERR_COMMON,                                             ///< Error code for none API error

    APP_ERR_MAX                                                 ///< Error code max
} app_err_t;

typedef struct {
    app_err_t        err_id;
    char            *error_message;
} app_err_info_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

extern char sprintf_buffer[];

/**********************************************************************************************************************
 * Global Function Prototypes
 **********************************************************************************************************************/

void app_handle_error(uint32_t err, app_err_t err_id);

#endif /* APP_COMMON_UTIL_H_ */
