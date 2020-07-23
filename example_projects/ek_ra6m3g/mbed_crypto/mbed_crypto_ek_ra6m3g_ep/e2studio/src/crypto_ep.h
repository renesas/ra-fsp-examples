/***********************************************************************************************************************
 * File Name    : crypto_ep.h
 * Description  : Contains data structures and functions used in crypto_ep.h.
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef CRYPTO_EP_H_
#define CRYPTO_EP_H_

/* Macro definitions.*/
#define ECC_256_BIT_LENGTH        (256U)              /* key size in bits for ECC*/
#define RSA_2048_BIT_LENGTH       (2048U)             /* key size in bits for RSA*/
/* Buffer size is calculated by using PSA_KEY_EXPORT_MAX_SIZE(type, bits)
 * where type is the key type and bits is the key size in bits.*/
#define RSA_2048_EXPORTED_SIZE    (1210U)             /* Size of the data buffer(to store key data) in bytes for RSA*/
#define ECC_256_EXPORTED_SIZE     (500U)              /* Size of the data buffer(to store key data) in bytes for ECC*/
#define AES_256_EXPORTED_SIZE     (500U)              /* Size of the data buffer(to store key data) in bytes for AES*/
#define TAG_LENGTH                ((size_t)16U)       /* number of bytes that make up the returned tag.*/
#define AES_KEY_ID                ((psa_key_id_t) 5)  /* persistent identifier for the key.*/
#define ECC_KEY_ID                ((psa_key_id_t) 6)
#define RSA_KEY_ID                ((psa_key_id_t) 7)
#define AES_KEY_BITS              (256U)             /* key size in bits for AES*/
#define ECC_RSA_PAYLOAD_SIZE      (30U)


#if (! (defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK)))
#define EP_INFO              "\r\nThis Example Project demonstrates the functionality of mbed crypto module on\r\n"\
                             "Renesas RA MCUs based on Renesas FSP.The EP performs following crypto operations : \r\n"\
                             "1. SHA256 Hashing.\r\n"\
                             "2. AES256 for GCM Mode.\r\n"\
                             "3. ECC -P256R1 operation.\r\n"\
                             "4. RSA 2048 operation.\r\n"\
                             "On successful completion of each operation, success message will be printed on\r\n"\
                             "RTT viewer.Error and info messages will be printed on JlinkRTTViewer. \r\n\n\n"
#else
#define EP_INFO              "\r\nThis Example Project demonstrates the functionality of mbed crypto module on\r\n"\
                             "Renesas RA MCUs based on Renesas FSP.The EP performs AES256 crypto operation for GCM\r\n"\
                             "Mode. On successful completion of each operation, success message will be printed on\r\n"\
                             "RTT viewer.Error and info messages will be printed on JlinkRTTViewer. \r\n\n\n"
#endif

/*Function declaration */
psa_status_t aes_operation(void);
void handle_error(psa_status_t status, char * err_str);
#if (! (defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK)))
psa_status_t sha_operation(void);
psa_status_t ecc_operation(void);
psa_status_t rsa_operation(void);
fsp_err_t littlefs_init(void);
void deinit_littlefs(void);
psa_status_t ecc_rsa_hashing_operation(unsigned char * payload, uint8_t * payload_hash, size_t * payload_hash_len);
#endif

#endif /* CRYPTO_EP_H_ */
