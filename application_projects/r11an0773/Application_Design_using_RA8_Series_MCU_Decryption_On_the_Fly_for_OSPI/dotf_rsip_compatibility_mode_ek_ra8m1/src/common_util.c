
/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/

#include "jlink_console.h"
#include "hal_data.h"
#include "stdio.h"
#include "common_util.h"

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
void handle_error (uint32_t err, app_err_t app_enum)
{
    if (APP_SUCCESS != err)
    {

        switch (app_enum)
        {

        case APP_OSPI_OPEN:
        {
           /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[failure: OSPI Open API]: R_OSPI_B_Open returned error. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_OSPI_DIRECT_TRANSFER:
        {
            /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[failure: OSPI Direct Transfer API]: R_OSPI_B_DirectTransfer returned error. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_OSPI_SPI_PROTOCOL_SET:
        {
            /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[failure: OSPI SPI Protocol Set]: R_OSPI_B_SpiProtocolSet returned error. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_FIBONACCI_CALCULATION:
        {
            /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[failure: Fibonacci calculation]: Fibonacci calculation failed. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_OSPI_OPERATION_TIMEOUT:
        {
            /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[failure: OSPI operation timeout]: OSPI operation timed out. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_OSPI_DOTF:
        {
            /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[OSPI DOTF ]: DOTF operation failed. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_JLINK_CONSOLE_INIT:
        {
           /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[failure: Jlink Initialization]: Jlink console initialization returned error. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_JLINK_CONSOLE_OPEN:
        {
           /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[failure: APP_JLINK_CONSOLE_OPEN]: R_SCI_B_UART_Open returned error. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }

        case APP_JLINK_SONSOLE_WRITE:
        {
           /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[failure: APP_JLINK_SONSOLE_WRITE: R_SCI_B_UART_Write returned error. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_JLINK_SONSOLE_READ:
        {
           /*Suggest valid the input and output buffer.  */
            sprintf(sprintf_buffer, "\r\n[failure: APP_JLINK_SONSOLE_READ: R_SCI_B_UART_Read returned error. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }

        case APP_DATA_ARRAY_NOT_MULTIPLE_OF_16:
        {
            /*Suggest check input buffer length  */
             sprintf(sprintf_buffer, "\r\n[failure: APP_DATA_ARRAY_NOT_MULTIPLE_OF_16: input data is not multiple of 16 bytes. \r\n");
             print_to_console(sprintf_buffer);
             break;
        }
        case APP_MBEDTLS_SETUP:
        {
            /*Suggest check input buffer length  */
             sprintf(sprintf_buffer, "\r\n[failure: APP_MBEDTLS_SETUP: mbedtls_platform_setup API failed. \r\n");
             print_to_console(sprintf_buffer);
             break;
        }
        case APP_PSA_CRYPTO_INIT:
        {
           /*Suggest check input buffer length  */
            sprintf(sprintf_buffer, "\r\n[failure: APP_PSA_CRYPTO_INIT: psa_crypto_init API failed. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_PSA_GENERAT_KEY:
        {
            /*Suggest check input buffer length  */
            sprintf(sprintf_buffer, "\r\n[failure: APP_PSA_GENERAT_KEY: psa_generate_key API failed. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_PSA_CIPHER_ENC_SETUP:
        {
            /*Suggest check input buffer length  */
            sprintf(sprintf_buffer, "\r\n[failure: APP_PSA_CIPHER_ENC_SETUP: psa_cipher_encrypt_setup API failed. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }
        case APP_RUNTIME_ENCRYPTION:
        {
            /*Suggest check input buffer length  */
            sprintf(sprintf_buffer, "\r\n[failure: APP_RUNTIME_ENCRYPTION: runtime encryption failed. \r\n");
            print_to_console(sprintf_buffer);
            break;
        }

        default:
        {
           break;
        }
    }
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
