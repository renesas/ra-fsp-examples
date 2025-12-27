/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
/**********************************************************************************************************************
 * File Name    : common_definitions.h
 * Description  : This file contains FSBL related structures when using CRC boot
 *********************************************************************************************************************/
#ifndef COMMON_DEFINITIONS_H_
#define COMMON_DEFINITIONS_H_

typedef struct
{
    uint8_t     sha256_oem_bl_fsblctrl1[32];
    uint8_t     signer_id[32];  /* for CRC boot, this will be the CRC values repeated four times */
    uint16_t    version_num_oem_bl;
}r_fsbl_measurement_report;

typedef struct
{
    uint8_t     code_cert_header[32];
    uint8_t     tlv_length[4];
    uint8_t     tlv_crc_signer_id[44];
    uint8_t     tlv_expected_sig[68];
}r_code_cert;

typedef struct
{
    uint8_t     fixed_type_length[4];
    uint8_t     value[32];
}r_oem_app_bl_digest;

#endif /* COMMON_DEFINITIONS_H_ */
