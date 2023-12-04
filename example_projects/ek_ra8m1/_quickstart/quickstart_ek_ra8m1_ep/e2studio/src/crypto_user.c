/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/

#include <ctype.h>

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "bsp_api.h"
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "usb_multiport.h"

/* Macro definitions.*/
bool_t get_key_as_str (char_t * target_str, size_t max_len);
void initialise_crypto_demo ();
bool_t initialise_crypto_key ();
void decrypt_demo ();
/* Buffer size is calculated by using PSA_KEY_EXPORT_MAX_SIZE(type, bits)
 * where type is the key type and bits is the key size in bits.*/
#define AES_256_EXPORTED_SIZE     (500U)    /* Size of the data buffer(to store key data) in bytes for AES */
#define TAG_LENGTH                (16U)     /* number of bytes that make up the returned tag.*/
#define AES_KEY_ID                (5)       /* persistent identifier for the key.*/
#define AES_KEY_BITS              (256U)    /* key size in bits for AES*/
#define ECC_RSA_PAYLOAD_SIZE      (30U)

#define MSG_FAILED_TO_ENCRYPT     ("Failed to authenticate and encrypt")

/* Global variables.*/

/* platform context structure.*/

uint8_t g_input_data[USB_IO_BUFFER] = {RESET_VALUE};
size_t  g_input_data_size           = 0;

uint8_t g_encrypted_data[USB_IO_BUFFER] = {RESET_VALUE};
size_t  g_encrypted_size                = 0;
size_t  g_encrypted_length              = 0;

uint8_t g_output_data[USB_IO_BUFFER] = {RESET_VALUE};
size_t  g_output_data_size           = 0;
size_t  g_output_length              = 0;


/* NOTE by default the Key is based upon the UUID of the target board */
static const uint8_t s_key_len_bytes = 16; // 256-bit key expected

static bool_t s_key_initialised      = false;

static uint8_t s_key[16]             = {};

static const uint8_t s_fallback_key[] = {
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF };

#define NUM_OF_NIST_KEYS    (16)
#define NUM_OF_FIXED_KEYS   (128)


void deinitialise_crypto_demo ();

void encrypt_demo ();


#ifdef PSA_SUCCESS
#define USE_HW_ENCRYPTION (1)
#else
#define PSA_SUCCESS (FSP_SUCCESS)
#endif


/**********************************************************************************************************************
 * Function Name: initialise_crypto_key
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t initialise_crypto_key(void)
{
    if (false == s_key_initialised) // key is not initialised
    {
        uint8_t const         * p_kptr = NULL;
        bsp_unique_id_t const * p_uid  = R_BSP_UniqueIdGet ();
        uint8_t  uuid_len              = sizeof (bsp_unique_id_t);
        uint16_t uuid_sum              = 0;

        /* Traverse the uuid accessing as uint8_t to determine if one has been set */
        p_kptr = (uint8_t *)p_uid;


        for (uint8_t ndx = 0; ndx < uuid_len; ndx++)
        {
            /* Maintain unsigned nature of bsp_unique_id_t  */
            uuid_sum = (uint16_t)(uuid_sum + ((uint16_t) *p_kptr++));
        }

        /* Detection of all 0x00 or 0xFF indicates BAD UUID */
        if ((0 == uuid_sum) || (uuid_sum >= 0xFF0))
        {
            p_kptr = &s_fallback_key[0];
        }
        else
        {
            /* Accessing p_uid as index into uint8_t  unique_id_bytes */
            p_kptr = (uint8_t *)p_uid;
        }

        for (uint8_t ndx = 0; ndx < s_key_len_bytes; ndx++)
        {
            /* Accessing p_kptr as index into uint8_t  unique_id_bytes */
            s_key[ndx] = (*p_kptr++);
        }

        s_key_initialised = true;
    }

    return (s_key_initialised);
}
/**********************************************************************************************************************
 End of function initialise_crypto_key
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_key_as_str
 * Description  : Check s the internal key and reuturns an ASCII representation of that key
 * Arguments    : target_str - buffer to hold resulting ASCII key
 *              : max_len    - length of the buffer, allowing the function to fail if the buffer is too small
 * Return Value : true - target_str is valid
 *********************************************************************************************************************/
bool_t get_key_as_str(char_t *target_str, size_t max_len)
{
    bool_t ret    = false;
    int_t  offset = ('A' - 'a');

    char_t t_str[3];

    if (max_len > (s_key_len_bytes * 2)) // Format as string  'XX'
    {
        for (uint16_t ndx = 0; ndx < s_key_len_bytes; ndx++)
        {
            char_t * p_str = t_str;

            sprintf(t_str, "%02x", s_key[ndx]);
            for (; *p_str; p_str++)
            {
                /* check for alphanumeric character */
                if (isalpha(*p_str))
                {
                    /* convert to Upper case */
                    *p_str = (char_t)((*p_str) + offset);
                }
            }

            *(target_str + (ndx * 2))     = t_str[0];
            *(target_str + (ndx * 2) + 1) = t_str[1];
        }
        ret = true;
    }

    return (ret);
}
/**********************************************************************************************************************
 End of function get_key_as_str
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: encrypt_demo
 * Description  : Perform data encryption, data encrypted from g_input_data and encrypted into g_encrypted_data
 * Return Value : .
 *********************************************************************************************************************/
void encrypt_demo(void)
{
#ifdef USE_HW_ENCRYPTION
    psa_status_t status;
    size_t tag_length = TAG_LENGTH;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_handle_t handle;

    g_encrypted_size = g_input_data_size + tag_length;

    /* Import a key */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);
    status = psa_import_key(&attributes, s_key, sizeof(s_key), &handle);
    psa_reset_key_attributes(&attributes);

    /* Authenticate and encrypt */
    status = psa_aead_encrypt(handle, PSA_ALG_CCM,
                                s_nonce, sizeof(s_nonce),
                                s_additional_data, sizeof(s_additional_data),
                                g_input_data, g_input_data_size,
                                g_encrypted_data, g_encrypted_size,
                                &g_encrypted_length);
    if (PSA_SUCCESS != status)
    {
        sprintf (s_print_buffer, "%s\r\n", MSG_FAILED_TO_ENCRYPT);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);
        return;
    }

    /* Destroy the key */
    psa_destroy_key(handle);
#else /* USE_SW_ENCRYPTION */
    struct tc_aes_key_sched_struct s;
    int_t    tc_encrypt_state = TC_CRYPTO_FAIL;

    (void)tc_aes128_set_encrypt_key(&s, s_key);

    /* Encrypt input buffer rounded up to blocks of 16 bytes */
    uint32_t in_limit = (g_input_data_size + 16)/ 16;
    in_limit = in_limit * 16;


    for (uint32_t offset = 0; offset < in_limit; offset+=16)
    {
        g_encrypted_length += 16;
        tc_encrypt_state = tc_aes_encrypt((uint8_t*)(g_encrypted_data + offset), (uint8_t*)(g_input_data + offset), &s);

        if (TC_CRYPTO_FAIL == tc_encrypt_state)
        {
            print_to_console((uint8_t *)"AES128 SW (NIST encryption) failed.\n");
            g_encrypted_length = 0;
            break;
        }

    }


#endif /* #if USE_HW_ENCRYPTION */
    print_to_console((uint8_t *)" Encryption of data completed\r\n");

}
/**********************************************************************************************************************
 End of function encrypt_demo
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: decrypt_demo
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void decrypt_demo(void)
{
#ifdef USE_HW_ENCRYPTION
    psa_status_t status;

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_handle_t handle;

    if (0 == g_encrypted_length)
    {
        /* Nothing to decrypt */
        return;
    }

    g_output_data_size = g_encrypted_length;

    /* Import a key */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);
    status = psa_import_key(&attributes, s_key, sizeof(s_key), &handle);

    if (PSA_SUCCESS != status)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"Failed to import a key\r\n");
        return;
    }
    psa_reset_key_attributes(&attributes);

    /* Authenticate and decrypt */
    status = psa_aead_decrypt(handle, PSA_ALG_CCM,
                                s_nonce, sizeof(s_nonce),
                                s_additional_data, sizeof(s_additional_data),
                                g_encrypted_data, g_encrypted_length,
                                g_output_data, g_output_data_size,
                                &g_output_length);
    if (PSA_SUCCESS != status)
    {
        sprintf(s_print_buffer, "Failed to authenticate and decrypt %ld\r\n", status);

        /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
        print_to_console((void*)s_print_buffer);
        return;
    }

    /* Destroy the key */
    psa_destroy_key(handle);
#else /* USE_SW_ENCRYPTION */
    struct tc_aes_key_sched_struct s;
    int_t    tc_decrypt_state = TC_CRYPTO_FAIL;

    (void)tc_aes128_set_encrypt_key(&s, s_key);

    /*  buffer rounded up to blocks of 16 bytes */
    uint32_t in_limit = (g_input_data_size + 16) / 16;
    in_limit = in_limit * 16;

    for (uint32_t offset = 0; offset < in_limit; offset += 16)
    {
        g_output_data_size += 16;
        tc_decrypt_state = tc_aes_decrypt((uint8_t*)(g_output_data + offset), \
                                            (uint8_t*)(g_encrypted_data + offset), &s);

        if (TC_CRYPTO_FAIL == tc_decrypt_state)
        {
            print_to_console((uint8_t *)"AES128 SW (NIST decryption) failed.\n");
            g_output_data_size = 0;
            break;
        }
    }

#endif /* #if USE_HW_ENCRYPTION */

    /* ignoring -Wpointer-sign is OK for a constant string */
    print_to_console((uint8_t *)" Decryption of data completed\r\n");
}
/**********************************************************************************************************************
 End of function decrypt_demo
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: initialise_crypto_demo
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void initialise_crypto_demo(void)
{
#ifdef USE_HW_ENCRYPTION
    psa_status_t  status       = RESET_VALUE;
    int_t         mbed_ret_val = RESET_VALUE;

    initialise_crypto_key();

    /* Setup the platform; initialize the SCE */
    mbed_ret_val = mbedtls_platform_setup(&s_ctx);
    if (RESET_VALUE != mbed_ret_val)
    {

        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\n** mbedtls_platform_setup API FAILED ** \r\n");
        return;
    }

    /* Initialize crypto library.*/
    status = psa_crypto_init();
    if (PSA_SUCCESS != status)
    {
        /* ignoring -Wpointer-sign is OK for a constant string */
        print_to_console((uint8_t *)"\r\n** psa_crypto_init API FAILED ** \r\n");

        /* De-initialize the platform.*/
        mbedtls_platform_teardown(&s_ctx);
        return;
    }
#else /* USE_SW_ENCRYPTION */
    /* Clean up */
    g_input_data_size   = 0;
    g_encrypted_size    = 0;
    g_encrypted_length  = 0;
    g_output_data_size  = 0;
    g_output_length     = 0;

    memset(&g_input_data[0], RESET_VALUE, USB_IO_BUFFER);
    memset(&g_encrypted_data[0], RESET_VALUE, USB_IO_BUFFER);
    memset(&g_output_data[0], RESET_VALUE, USB_IO_BUFFER);

#endif /* #if USE_HW_ENCRYPTION */
}
/**********************************************************************************************************************
 End of function initialise_crypto_demo
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: deinitialise_crypto_demo
 * Description  : Return the crypto system and associated memory buffers to an empty state
 * Return Value : .
 *********************************************************************************************************************/
void deinitialise_crypto_demo(void)
{
#ifdef USE_HW_ENCRYPTION
    /* Clean up */
    g_input_data_size = 0;
    g_encrypted_size = 0;
    g_encrypted_length = 0;
    g_output_data_size = 0;
    g_output_length = 0;

    memset(&g_input_data[0], RESET_VALUE, USB_IO_BUFFER);
    memset(&g_encrypted_data[0], RESET_VALUE, USB_IO_BUFFER);
    memset(&g_output_data[0], RESET_VALUE, USB_IO_BUFFER);

    /* De-initialize the platform.*/
    mbedtls_psa_crypto_free();
    mbedtls_platform_teardown(&s_ctx);
#else /* USE_SW_ENCRYPTION */
    /* Clean up */
    g_input_data_size   = 0;
    g_encrypted_size    = 0;
    g_encrypted_length  = 0;
    g_output_data_size  = 0;
    g_output_length     = 0;

    memset(&g_input_data[0], RESET_VALUE, USB_IO_BUFFER);
    memset(&g_encrypted_data[0], RESET_VALUE, USB_IO_BUFFER);
    memset(&g_output_data[0], RESET_VALUE, USB_IO_BUFFER);

#endif /* #if USE_HW_ENCRYPTION */
}
/**********************************************************************************************************************
 End of function deinitialise_crypto_demo
 *********************************************************************************************************************/
