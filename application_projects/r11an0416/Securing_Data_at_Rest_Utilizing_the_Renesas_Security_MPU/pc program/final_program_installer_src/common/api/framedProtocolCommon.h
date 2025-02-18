/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*
 *  Common defines and code for the framed serial protocol between PC and embedded code.
 */

#ifndef FRAMED_PROTOCOL_COMMON_H_
#define FRAMED_PROTOCOL_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

    /*
    * Protocol definitions
    */

    // Framing codes
    #define START_OF_HEADER 0x01
    #define START_OF_DATA   0x81
    #define END_OF_PACKET   0x03

    // Maximum packet payload length
    #define MAX_PACKET_PAYLOAD 1024

    // Maximum length field in framed packets (1 byte opCode + payload)
    #define MAX_FRAME_LENGTH (1 + MAX_PACKET_PAYLOAD)

    typedef enum CommandCode_tag {
       INQUIRY                         = 0x00,
       ERASE                           = 0x12,
       WRITE                           = 0x13,
       READ                            = 0x15,
       ID_AUTHENTICATION               = 0x30,
       BAUDRATE                        = 0x34,
       SIGNATURE_REQUEST               = 0x3A,
       AREA_REQUEST                    = 0x3B,
       CERTIFICATE_REQUEST             = 0x3C,
       APPLY_IMAGE                     = 0x3D,
       MODULE_VERSION_LIST_REQUEST     = 0x3E,
       PULL_REQUEST                    = 0x3F,
       WRAPPED_KEY_REQUEST             = 0x40,
       WRAPPED_KEY_ENCRYPT_DATA        = 0x41,
       WRAPPED_KEY_INIT_VECTOR_DATA    = 0x42,
       WRAPPED_KEY_CERT_PROGRAM        = 0X43,
       WRAPPED_KEY_CERT_REQUEST        = 0x44,
       WRAPPED_KEY_CHALLENGE_RESP      = 0x45,
        // Errored commands
       INQUIRY_ERR                     = 0x80,
       ERASE_ERR                       = 0x92,
       WRITE_ERR                       = 0x93,
       READ_ERR                        = 0x95,
       ID_AUTHENTICATION_ERR           = 0xB0,
       BAUDRATE_ERR                    = 0xB4,
       SIGNATURE_REQUEST_ERR           = 0xBA,
       AREA_REQUEST_ERR                = 0xBB,
       CERTIFICATE_REQUEST_ERR         = 0xBC,
       APPLY_IMAGE_ERR                 = 0xBD,
       MODULE_VERSION_LIST_REQUEST_ERR = 0xBE,
       WRAPPED_KEY_REQUEST_ERR         = 0xC0,
       WRAPPED_KEY_ENCRYPT_DATA_ERR    = 0xC1,
       WRAPPED_KEY_INIT_VECTOR_DATA_ERR= 0xC2,
       WRAPPED_KEY_CERT_PROGRAM_ERR    = 0XC3,
       WRAPPED_KEY_CERT_REQUEST_ERR    = 0XC4,
       WRAPPED_KEY_CHALLENGE_RESP_ERR  = 0xC5
    } CommandCode;

    typedef enum ResponseCode_tag {
       // OK for all commands
       OK =  0x00,
       // And because in data packets the response code can be the command
       WRITE_RESP = 0x13,
       // Specific error codes
       UNSUPPORTED_COMMAND       = 0xC0,
       PACKET_ERROR              = 0xC1,
       CHECKSUM_ERROR            = 0xC2,
       FLOW_ERROR                = 0xC3,
       ADDRESS_ERROR             = 0xD0,
       BAUDRATE_MARGIN_ERROR     = 0xD4,
       PROTECTION_ERROR          = 0xDA,
       ID_DISCORD_ERROR          = 0xDB,
       SERIAL_PROG_DISABLE_ERROR = 0xDC,
       ERASE_ERROR               = 0xE1,
       WRITE_ERROR               = 0xE2,
       SEQUENCE_ERROR            = 0xE7,
       NO_OEM_CERT_ERROR         = 0xF0,
       // To keep the same protocol format but without a specific error code
       UNSPECIFIED_ERROR         = 0xFF
    } ResponseCode;

    // Command packet sizes
    #define LEN_BAUDRATE_CMD    10
    #define LEN_ERASE_CMD       14
    #define LEN_WRITE_CMD       14
    #define LEN_READ_CMD        14
    #define LEN_CERTIFICATE_CMD  6
    #define LEN_VERSION_LIST_CMD 6
    #define LEN_APPLY_IMAGE_CMD  6
    #define LEN_DEVICEKEY_CMD    6
    #define LEN_CERT_RESP        6
    #define LEN_CERT_VALIDATE    6
    #define MAX_LEN_DATA_PACKET 1030


    // Helpers for network byte order access
    #define WRITE16BE(pBUFF, vALUE) {         \
       *(pBUFF)     = ((vALUE) >> 8) & 0xFF;  \
       *((pBUFF)+1) = (vALUE) & 0xFF;         \
    }
    #define WRITE32BE(pBUFF, vALUE) {         \
       *(pBUFF)     = ((vALUE) >> 24) & 0xFF; \
       *((pBUFF)+1) = ((vALUE) >> 16) & 0xFF; \
       *((pBUFF)+2) = ((vALUE) >> 8)  & 0xFF; \
       *((pBUFF)+3) = (vALUE) & 0xFF;         \
    }

    #define READ16BE(pBUFF) ((*(pBUFF) << 8) + (*((pBUFF) + 1)))
    #define READ32BE(pBUFF) ((*(pBUFF) << 24) + (*((pBUFF) + 1) << 16) + (*((pBUFF) + 2) << 8) + (*((pBUFF) + 3)))

    /*
    * Common functions
    */

    // Calculate a checksum over the given byte array
    // Input: pointer to buffer over which teh checksum is to be calculated
    // Input: number of bytes in the buffer
    // Returns: the calculated checksum
    extern uint8_t fpcCalculateChecksum(const uint8_t *pData, const uint16_t numBytes);

    // Build a message of the given startCode, opcode and optional data
    // Function signature for serial transmit callback
    // Input: pointer to buffer into which the message will be compiled
    // Input: length of the buffer
    // Input: the message start code to be used
    // Input: the message command code to use
    // Input: pointer to optional message payload data (NULL if none to be sent)
    // Input: number of bytes of payload data to send (0..MAX_PACKET_PAYLOAD)
    // Returns size of command or zero on failure.
    extern uint16_t fpcBuildMessage(const uint8_t *pBuffer, const uint16_t bufferLength, const uint8_t startCode, const CommandCode opCode, const uint8_t *pData, const uint16_t numBytes);


#ifdef __cplusplus
}
#endif

#endif /* FRAMED_PROTOCOL_COMMON_H_ */
