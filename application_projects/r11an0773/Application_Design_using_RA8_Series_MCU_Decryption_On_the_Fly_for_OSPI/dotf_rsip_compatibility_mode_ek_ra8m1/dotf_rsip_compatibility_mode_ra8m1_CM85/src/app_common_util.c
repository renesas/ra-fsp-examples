
/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/

#include "jlink_console.h"
#include "hal_data.h"

#include "app_common_util.h"

/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/

#define ENABLE_BREAK_POINT_AT_FAULT

/***************************************************************************************************************************
 * Typedef definitions
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***************************************************************************************************************************/
/***************************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***************************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables and functions
 ************************************************************************************************************************/


/*********************************************************************************************************************
 *  @brief       This function handle error if error occurred, closes all opened modules, print and traps error.
 *  @param[in]   status    error status
 *  @param[in]   err_str   error string
 *  @retval      None
 **********************************************************************************************************************/

static const app_err_info_t app_err_info[APP_ERR_MAX] =
{
     {.err_id = APP_OSPI_INIT,                      .error_message = "\r\n[failure: OSPI Initialization]: OSPI Initialization failed. \r\n"},
     {.err_id = APP_OSPI_OPEN,                      .error_message = "\r\n[failure: OSPI Open API]: R_OSPI_B_Open returned error. \r\n"},
     {.err_id = APP_OSPI_CLOSE,                     .error_message = "\r\n[failure: OSPI Close API]: R_OSPI_B_Close returned error. \r\n"},
     {.err_id = APP_OSPI_DIRECT_TRANSFER,           .error_message = "\r\n[failure: OSPI Direct Transfer API]: R_OSPI_B_DirectTransfer returned error. \r\n"},
     {.err_id = APP_OSPI_SPI_PROTOCOL_SET,          .error_message = "\r\n[failure: OSPI SPI Protocol Set API]: R_OSPI_B_SpiProtocolSet returned error. \r\n"},
     {.err_id = APP_OSPI_ERASE,                     .error_message = "\r\n[failure: OSPI Erase API]: R_OSPI_B_Erase returned error. \r\n"},
     {.err_id = APP_OSPI_WRITE,                     .error_message = "\r\n[failure: OSPI Write API]: R_OSPI_B_Write returned error. \r\n"},
     {.err_id = APP_OSPI_STATUSGET,                 .error_message = "\r\n[failure: OSPI Status Get API]: R_OSPI_B_StatusGet returned error. \r\n"},

     {.err_id = APP_OSPI_READ,                      .error_message = "\r\n[failure: OSPI operation read]: OSPI operation read failed. \r\n"},
     {.err_id = APP_OSPI_WRITE_ENABLE,              .error_message = "\r\n[failure: OSPI operation write enable]: OSPI operation write enable failed. \r\n"},
     {.err_id = APP_OSPI_OPERATION_TIMEOUT,         .error_message = "\r\n[failure: OSPI operation timeout]: OSPI operation timeout failed. \r\n"},
     {.err_id = APP_OSPI_DOTF,                      .error_message = "\r\n[failure: OSPI operation DOTF ]: DOTF operation failed. \r\n"},
     {.err_id = APP_OSPI_DECRYPTED_DATA_NOT_MATCH,  .error_message = "\r\n[failure: Decrypted data]: Decrypted data not match failed. \r\n"},
     {.err_id = APP_FIBONACCI_CALCULATION,          .error_message = "\r\n[failure: Fibonacci calculation]: Fibonacci calculation failed. \r\n"},
     {.err_id = APP_WRITE_PLAINTEXT_DATA,           .error_message = "\r\n[failure: Write plaintext data]: Write plaintext data failed. \r\n"},
     {.err_id = APP_WRITE_ENCRYPTED_DATA,           .error_message = "\r\n[failure: Write encrypted data]: Write encrypted data failed. \r\n"},

     {.err_id = APP_JLINK_CONSOLE_INIT,             .error_message = "\r\n[failure: JLink Initialization]: JLink console initialization failed. \r\n"},
     {.err_id = APP_JLINK_CONSOLE_OPEN,             .error_message = "\r\n[failure: SCI UART Open API]: R_SCI_B_UART_Open returned error. \r\n"},
     {.err_id = APP_JLINK_SONSOLE_WRITE,            .error_message = "\r\n[failure: SCI UART Write API: R_SCI_B_UART_Write returned error. \r\n"},
     {.err_id = APP_JLINK_SONSOLE_READ,             .error_message = "\r\n[failure: SCI UART Read API]: R_SCI_B_UART_Read returned error. \r\n"},

     {.err_id = APP_DATA_ARRAY_NOT_MULTIPLE_OF_16,  .error_message = "\r\n[failure: APP_DATA_ARRAY_NOT_MULTIPLE_OF_16]: input data is not multiple of 16 bytes. \r\n"},
     {.err_id = APP_WRAPPED_KEY_GEN,                .error_message = "\r\n[failure: APP_WRAPPED_KEY_GEN]: generate wrapped AES key failed. \r\n"},
     {.err_id = APP_RUNTIME_ENCRYPTION,             .error_message = "\r\n[failure: APP_RUNTIME_ENCRYPTION]: runtime encryption failed. \r\n"},

     {.err_id = APP_MBEDTLS_SETUP,                  .error_message = "\r\n[failure: APP_MBEDTLS_SETUP] : mbedtls_platform_setup returned error. \r\n"},
     {.err_id = APP_PSA_CRYPTO_INIT,                .error_message = "\r\n[failure: PSA Crypto Init API]: psa_crypto_init returned error. \r\n"},
     {.err_id = APP_PSA_GENERAT_KEY,                .error_message = "\r\n[failure: PSA Generate Key API]: psa_generate_key returned error. \r\n"},
     {.err_id = APP_PSA_EXPORT_KEY,                 .error_message = "\r\n[failure: PSA Export Key API]: psa_export_key returned error. \r\n"},
     {.err_id = APP_PSA_CIPHER_ENC_SETUP,           .error_message = "\r\n[failure: PSA Encrypt Cipher API: psa_cipher_encrypt_setup returned error. \r\n"},
     {.err_id = APP_PSA_CIPHER_SET_IV,              .error_message = "\r\n[failure: PSA Set IV API: psa_cipher_set_iv returned error. \r\n"},
     {.err_id = APP_PSA_CIPHER_UPDATE,              .error_message = "\r\n[failure: PSA Cipher Update API: psa_cipher_update returned error. \r\n"},
     {.err_id = APP_PSA_CIPHER_FINISH,              .error_message = "\r\n[failure: PSA Cipher Finish API: psa_cipher_finish returned error. \r\n"},
};

static char *app_err_info_lookup(app_err_t err_id)
{
    for (app_err_t index = 0; index < APP_ERR_MAX; index++)
    {
        if (err_id != app_err_info[index].err_id)
        {
            continue;
        }

        return app_err_info[index].error_message;
    }

    return NULL;
}

void app_handle_error(uint32_t err, app_err_t err_id)
{
    if (APP_SUCCESS != err)
    {
        char *err_message = app_err_info_lookup(err_id);

        sprintf(sprintf_buffer, "%s", err_message);
        jlink_print_to_console(sprintf_buffer);

        /* if user wants to debug the project, then enable macro ENABLE_DEBUG_OUTPUT */
        #if defined(ENABLE_BREAK_POINT_AT_FAULT)
             __asm("BKPT #0\n"); /* trap upon the error  */
        #else

        /*
         * This section can be updated to implement a system recovery scheme which is out of scope of this example application.
         * By default, this application breaks execution at a fault with ENABLE_BREAK_POINT_AT_FAULT defined.
         */

         #endif
    }
}
