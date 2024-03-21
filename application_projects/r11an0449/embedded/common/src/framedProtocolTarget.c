/***********************************************************************************************************************
 * File Name    : framedProtocolTarget.c
 * Description  : handle commands received via framedProtocol from PC.
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

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <user_cfg.h>
#include "framedProtocolTarget.h"
#include "hwWrappedKey.h"
#include "flash.h"
#include "common_util.h"
#include "cryptoTypes.h"
#include "common_utils.h" //RTT Viewer support

/* Callback to send packets */
static FpDataTransmitCallback pFpDataTransmitCallback;
/* maximum TX data buffer */
static uint8_t fpTxBuffer[MAX_LEN_DATA_PACKET];
/* maximum RX data buffer */
static uint8_t fpRxBuffer[MAX_LEN_DATA_PACKET];
static uint16_t fpNumRxBytes = 0;
static DevKey_t devKey;

BSP_DONT_REMOVE DevCertificatePEM_t  __attribute__ ((section(".device_cert")))devCert;
void read_devCert(uint8_t *deviceCert, uint32_t numBytes)  __attribute__((section(".secure_code")));

DevCertificatePEM_t devCertPEM;

/*****************************************************************************************************************
 *  @brief      FpCommandHandlerCallback: framedProtocol callback function
 *  @param[in]  cmdCode: command code
 *  @param[in]  pBuffer: pointer to received packet payload (may be NULL if numBytes == 0)
 *  @param[in]  numBytes: number of bytes in the packet payload
 *  @retval     none
 *  ****************************************************************************************************************/
typedef void (*FpCommandHandlerCallback)(const CommandCode cmdCode, const uint8_t *pBuffer, const uint32_t numBytes);

/* Structure representing an entry in the command handler table */
typedef struct CommandHandlerConfigEntry_tag {
    /* The command code associated with this handler */
    CommandCode cmdCode;
    /* The handler callback function */
    FpCommandHandlerCallback cmdHandlerCallback;
} CommandHandlerConfigEntry;

/*****************************************************************************************************************
 *  @brief      FpCommandHandlerCallback: framedProtocol callback function
 *  @param[in]  deviceCert: pointer to the device certificate buffer received
 *  @param[in]  numBytes: number of bytes to read
 *  @retval     none
 *  ****************************************************************************************************************/
void read_devCert(uint8_t *deviceCert, uint32_t numBytes)
{
    SECURE_PADDING

    uint16_t index = 0;
    uint8_t *dev_cert_ptr = (uint8_t*)((volatile uint8_t *)ADDR_DEV_CERT);

    for(index = 0; index < numBytes; index++)
    {
        deviceCert[index] = dev_cert_ptr[index];
    }
}

/*****************************************************************************************************************
 *  @brief      sendResponse: function to be called from the command handler function in order to
 *              send a response packet.
 *  @param[in]  commandCode: command code
 *  @param[in]  pPayload: pointer to the buffer holding the data to be sent
 *  @param[in]  numBytes: number of bytes to send
 *  @retval     none
 *  ****************************************************************************************************************/
static void sendResponse(const CommandCode commandCode, const uint8_t *pPayload, const uint16_t numBytes)
{
    uint16_t len = fpcBuildMessage(fpTxBuffer, MAX_LEN_DATA_PACKET, START_OF_DATA, commandCode, pPayload, numBytes);
    if (0 < len)
    {
        pFpDataTransmitCallback(fpTxBuffer, len);
    }
}

/*****************************************************************************************************************
 *  @brief      handleHrkKeyCreation: function to be called from the command handler function in order to
 *              ask the MCU to generate the hardware root key pair.
 *  @param[in]  cmdCode: command code
 *  @param[in]  pBuffer: pointer to the buffer holding the public key
 *  @param[in]  numBytes: number of bytes of the public key
 *  @retval     none
 *  ****************************************************************************************************************/
static void handleHrkKeyCreation(const CommandCode cmdCode __attribute__((unused)),
                                 const uint8_t *pBuffer __attribute__((unused)),
                                 const uint32_t numBytes __attribute__((unused)))
{
    ResponseCode status = OK;
    bool ret = false;
    uint8_t *devPubKeyPtr = devKey.pubKey;

    APP_PRINT("\r\nReceived WRAPPED_KEY_REQUEST request\r\n");

    ret = hwCreatekey(devPubKeyPtr, &(devKey.key_handle));
    if (false == ret)
    {

        APP_PRINT("hwCreatekey failed: %d\r\n",ret);

        status = UNSPECIFIED_ERROR;
        sendResponse(WRAPPED_KEY_REQUEST_ERR, &status, 1);
    }
    else
    {
        APP_PRINT("hwCreateKey successful\r\n");

        sendResponse(WRAPPER_KEY_REQUEST, devPubKeyPtr, (uint16_t) ECC_PUBLIC_KEY_SIZE);
    }
}

/*****************************************************************************************************************
 *  @brief      handleHrkKeyCreation: function to be called from the command handler function in order to
 *              ask the MCU to generate the hardware root key pair.
 *  @param[in]  cmdCode: command code
 *  @param[in]  pBuffer: pointer to the buffer holding the public key
 *  @param[in]  numBytes: number of bytes of the public key
 *  @retval     none
 *  ****************************************************************************************************************/

static void handleHrkCertProgram(const CommandCode cmdCode __attribute__((unused)),
                                 const uint8_t *pBuffer, const uint32_t numBytes)
{
    ResponseCode status = OK;
    fsp_err_t  fsp_status = FSP_SUCCESS;
    const DevCertificatePEM_t *devCertPtr = (const DevCertificatePEM_t *) pBuffer;


    APP_PRINT("\r\n WRAPPED_KEY_CERT_PROGRAM request received \r\n");

    if(0 == numBytes || NULL == pBuffer)
    {
        status = UNSPECIFIED_ERROR;

        APP_PRINT("Invalid byte count: %lu\r\n",numBytes);

    }
    else
    {
        fsp_status = flash_write ((uint8_t *)devCertPtr->certPEM, devCertPtr->certPEMLen, DEV_CERT_CFG);
        if (FSP_SUCCESS != fsp_status)
        {

            APP_PRINT("Device Cert: flash write error %d  \n\r", (int)fsp_status);

            status = UNSPECIFIED_ERROR;
        }
        else
        {

            APP_PRINT("Successfully wrote the Device Cert in the flash\r\n");

        }
    }

    if (OK == status)
    {

        APP_PRINT("WRAPPED_KEY_CERT_PROGRAM cmd successful !!!\r\n ");

        sendResponse(WRAPPED_KEY_CERT_PROGRAM, &status, 1);
    }
    else
    {

        APP_PRINT("WRAPPED_KEY_CERT_PROGRAM failed!!! \r ");

        sendResponse(WRAPPED_KEY_CERT_PROGRAM_ERR, &status, 1);
    }

    /*Read the device certificate.*/
    read_devCert(devCertPEM.certPEM, numBytes);
    if (0 == memcmp((void*)&devCertPEM.certPEM, (void *)&devCertPtr->certPEM, numBytes))
    {
        APP_PRINT("Flash write validation of Device Cert succeeded\n\r");

    }
    else
    {
        APP_PRINT("Failed flash write validation of Device Cert\n\r");

    }

#ifdef ENABLE_FAW
    /* Set the FAW to lock the Secure code and data region */
    flash_FAW_Set(FAW_START_ADDR, FAW_END_ADDR);
#endif
}


/*****************************************************************************************************************
 *  @brief      handleHrkCertChallengeResp: function to be called from the command handler function in order to
 *              ask the MCU to sign the hash buffer and return the signature to the caller.
 *  @param[in]  cmdCode: command code
 *  @param[in]  pBuffer: pointer to the buffer holding the signature
 *  @param[in]  numBytes: number of bytes of the signature
 *  @retval     none
 *  ****************************************************************************************************************/
static void handleHrkCertChallengeResp(const CommandCode cmdCode __attribute__((unused)),
                                       const uint8_t *pBuffer,
                                       const uint32_t numBytes)
{
    ResponseCode status = OK;
    bool ret = false;
    Sha256 hash;
    EccSignature   sig;


    APP_PRINT("\r\n Received WRAPPED_KEY_CHALLENGE_RESP command\r\n");

    if (sizeof(Sha256) != numBytes)
    {

        APP_PRINT("\r\nCert ChallengeResponse numBytes invalid: %d\n\r", (int) numBytes);

        status = UNSPECIFIED_ERROR;
        sendResponse(WRAPPED_KEY_CHALLENGE_RESP_ERR, &status, 1);
        return;
    }

    memcpy(hash, pBuffer, sizeof(Sha256));

    ret = hwSignData(hash, sig, &devKey.key_handle);
    if(false == ret)
    {

        APP_PRINT("\r\nCert ChallengeResponse signature generation failed: \n\r");

        status = UNSPECIFIED_ERROR;
        sendResponse(WRAPPED_KEY_CHALLENGE_RESP_ERR, &status, 1);
    }
    else
    {

        APP_PRINT("ChallengeResp successful. Responding with signed Hashed data\r\n ");

        sendResponse(WRAPPED_KEY_CHALLENGE_RESP, sig, (uint16_t)sizeof(EccSignature));
    }

    /* Clear out the temporary structure before we leave so we don't */
    /* leave potentially sensitive information in memory. */
    memset (hash, 0, sizeof (Sha256));
    memset (sig,  0, sizeof (EccSignature));
}

/* Framed protocol command handler callback table */
#define NUM_COMMAND_HANDLERS 4
const CommandHandlerConfigEntry commandHandlerTable[NUM_COMMAND_HANDLERS] = {
    { WRAPPER_KEY_REQUEST,         handleHrkKeyCreation },
    { WRAPPED_KEY_CERT_PROGRAM,    handleHrkCertProgram },
    { WRAPPED_KEY_CHALLENGE_RESP,  handleHrkCertChallengeResp},
};

/*****************************************************************************************************************
 *  @brief      dispatchCommand: analysis the data received from the PC and dispatch the corresponding command.
 *  @param[in]  cmdCode: command code
 *  @param[in]  pPayload: pointer to the buffer holding the Payload
 *  @param[in]  numPayloadBytes: number of bytes in the Payload
 *  @retval     none
 *  ****************************************************************************************************************/

/* Try to dispatch a received command to the application.*/
static void dispatchCommand(const CommandCode cmdCode, const uint8_t *pPayload, const uint16_t numPayloadBytes)
{
    /* Search the command table looking for a handler */
    bool found = false;
    int i = 0;

    while (!found && (i<NUM_COMMAND_HANDLERS))
    {
        if (commandHandlerTable[i].cmdCode == cmdCode)
        {
            #ifdef SWO_PRINTF
                printf("\r\ncmdCode = %x, numBytes = %d\r\n",cmdCode,numPayloadBytes);
            #endif
            /* Dispatch to the handler*/
            (commandHandlerTable[i].cmdHandlerCallback)(cmdCode, pPayload, numPayloadBytes);
            found = true;
        }

        i++;
    }
}

/*****************************************************************************************************************
 *  @brief      fpReceiveByte: handle a received bypte of serial data.
 *  @param[in]  byte: the byte received
 *  @param[in]  pPayload: pointer to the buffer holding the Payload
 *  @param[in]  numPayloadBytes: number of bytes in the Payload
 *  @retval     none
 *  ****************************************************************************************************************/
void fpReceiveByte(const uint8_t byte)
{
    /* The RX parser is in one of 2 states:
    *   - looking for a start code
    *   - receiving a packet */

    if (0 == fpNumRxBytes)
    {
        /* Looking for the start of a packet - discard anything else */
        if ((START_OF_HEADER == byte) || (START_OF_DATA == byte))
        {
            fpRxBuffer[fpNumRxBytes++] = byte;
        }
    }
    else
    {
        /* Receiving a packet */
        if (fpNumRxBytes < MAX_LEN_DATA_PACKET)
        {
            fpRxBuffer[fpNumRxBytes++] = byte;

            /* Minimum frame size is 6 bytes */
            if (6 <= fpNumRxBytes )
            {
                /* Read length field */
                uint8_t *pLen = &fpRxBuffer[1];
                uint16_t length = (uint16_t) READ16BE(pLen);
                if ((MAX_FRAME_LENGTH < length ) || (0 == length ))
                {
                    /* Abort frame, start resync */
                    fpNumRxBytes = 0;
                }
                else
                {
                    /* Length OK - have we received enough data to finish the frame? */
                    /* (length doesn't include SOH, 2LEN bytes, SUM and ETX) */
                    if (fpNumRxBytes == (length + 5))
                    {
                        /* If checksum is good and final byte is an end of packet, frame is received, otherwise sync is lost */
                        /* Checksum is over (2LEN bytes + opCode + payload) */
                        uint8_t checkSum = fpcCalculateChecksum(pLen, (uint16_t)(length + 2));
                        if ((checkSum == pLen[length + 2]) && (END_OF_PACKET == pLen[length + 3]))
                        {
                            /* Dispatch command or data */
                            if (START_OF_HEADER == fpRxBuffer[0])
                            {
                                dispatchCommand((CommandCode)pLen[2], &pLen[3], (uint16_t)(length - 1));
                            }
                        }
                        /* Start looking for the next packet */
                        fpNumRxBytes = 0;
                    }
                }
            }
        }
        else
        {
            /* Lost sync - start looking for start of frame again */
            fpNumRxBytes = 0;
        }
    }
}

/*****************************************************************************************************************
 *  @brief      fpInitialise: initialize the flash and provision the response callback function.
 *  @param[in]  pDataTransmitCallback: pointer to the transmit data callback
 *  @retval     true if initialized OK, false otherwise
 *  ****************************************************************************************************************/

bool fpInitialise(const FpDataTransmitCallback pDataTransmitCallback)
{
    /* Store the function pointer */
    pFpDataTransmitCallback = pDataTransmitCallback;

    /* Reset the RX buffer */
    fpNumRxBytes = 0;

    if(FSP_SUCCESS != flash_init())
    {

        APP_PRINT("Flash Open Failed!!!!!\n\r");

        APP_ERR_TRAP(true);
    }

    return true;
}
