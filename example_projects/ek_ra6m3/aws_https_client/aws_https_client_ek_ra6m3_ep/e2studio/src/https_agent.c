/***********************************************************************************************************************
 * File Name    : https_agent.c
 * Description  : This file contains the User Application code for the https_agent cleint connection
 ***********************************************************************************************************************/
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

#include "https_agent.h"
#include "common_utils.h"
#include "user_app.h"

/*******************************************************************************************************************//**
 * @addtogroup aws_https_client_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 * Private global variables and functions
 ******************************************************************************/

/**
 * @brief Buffer used to store the internal connection context.
 */
static uint8_t _pConnUserBuffer[IOT_DEMO_HTTPS_CONN_BUFFER_SIZE] = { RESET_VALUE };

/**
 * @brief Buffer used to store the request context and the HTTP request header lines.
 */
static uint8_t _pReqUserBuffer[IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE] = { RESET_VALUE };

/**
 * @brief Buffer used to store the response context and the HTTP response header lines.
 */
static uint8_t _pRespUserBuffer[IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE] = { RESET_VALUE };

/**
 * @brief Buffer used to store parts of the response body.
 */
static uint8_t _pRespBodyBuffer[IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE] = { RESET_VALUE };

/******************************************************************************
 * Global variables and functions
 ******************************************************************************/

/* Buffers for HTTP library. */
uint8_t *pConnectionUserBuffer = NULL; /* Buffer to store the HTTP connection context. */
uint8_t *pRequestUserBuffer = NULL; /* Buffer to store the HTTP request context and header. */
uint8_t *pResponseUserBuffer = NULL; /* Buffer to store the HTTP response context and header. */
uint8_t *pResponseBodyBuffer = NULL; /* Buffer to store the HTTP response body. */

/* The location of the address within string IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
const char *pAddress = NULL;
/* Synchronous request specific configurations. */
IotHttpsSyncInfo_t reqSyncInfo = { RESET_VALUE };
/* Synchronous response specific configurations. */
IotHttpsSyncInfo_t respSyncInfo = { RESET_VALUE };
/* Configurations for the HTTPS request. */
IotHttpsRequestInfo_t reqConfig = { RESET_VALUE };
/* Configurations for the HTTPS connection. */
IotHttpsConnectionInfo_t connConfig = { RESET_VALUE };
/* Configurations for the HTTPS response. */
IotHttpsResponseInfo_t respConfig = { RESET_VALUE };
/* HTTPS Client library return status. */
IotHttpsReturnCode_t httpsClientStatus = IOT_HTTPS_OK;
/* The status of HTTP responses for each request. */
uint16_t respStatus = IOT_HTTPS_STATUS_OK;

/* Handle identifying the HTTP request. This is valid after the request has been initialized with
 * IotHttpsClient_InitializeRequest(). */
IotHttpsRequestHandle_t reqHandle = IOT_HTTPS_REQUEST_HANDLE_INITIALIZER;

/* Handle identifying the HTTPS connection. */
IotHttpsConnectionHandle_t connHandle = IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER;

/* The length of the address within string IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
size_t addressLen = RESET_VALUE;
/* Extracted ID from GET request to be updated in IOT_DEMO_HTTPS_PRESIGNED_PUT_URL */
char id[32] = { RESET_VALUE };
/* Flag bit for PUT request. if User calls directly without processed GET request */
bool is_get_called = false;

/*******************************************************************************************************************//**
 * @brief      Initializes the https client with the IOT_DEMO_HTTPS_PRESIGNED_GET_URL
 *
 * @param[in]  pURL                         url has to be updated by user with their user name
 *                                          in IOT_DEMO_HTTPS_PRESIGNED_GET_URL macro
 * @retval     IOT_HTTPS_OK                 Upon successful client Initialization.
 * @retval     Any other Error Code         Upon unsuccessful client Initialization.
 **********************************************************************************************************************/
IotHttpsReturnCode_t initialize_https_client(char *pURL)
{
    /* The location of the path within string IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
    const char *pPath = NULL;
    /* The length of the path within string IOT_DEMO_HTTPS_PRESIGNED_GET_URL. */
    size_t pathLen = RESET_VALUE;

    /* Retrieve the path location from IOT_DEMO_HTTPS_PRESIGNED_GET_URL. This function returns the length of the path
     * without the query into pathLen. */
    httpsClientStatus = IotHttpsClient_GetUrlPath (pURL, strlen (pURL), &pPath, &pathLen);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        APP_PRINT("An error occurred in IotHttpsClient_GetUrlPath() on URL %s. Error code: %d", pURL, httpsClientStatus);
        return httpsClientStatus;
    }

    /* Retrieve the address location and length from the IOT_DEMO_HTTPS_PRESIGNED_GET_URL.*/
    httpsClientStatus = IotHttpsClient_GetUrlAddress (pURL, strlen (pURL), &pAddress, &addressLen);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        APP_ERR_PRINT("An error occurred in GetUrlAddress() on URL %s\r\n. Error code %d", pURL, httpsClientStatus);
        return httpsClientStatus;
    }
    /* address length to be update in connection configurations */
    addressLen = (size_t) (pPath - pAddress);

    /* Initialize the HTTPS library */
    httpsClientStatus = IotHttpsClient_Init ();
    /* Error Handler */
    if (IOT_HTTPS_OK != httpsClientStatus)
    {
        APP_PRINT("Fail to initialize HTTP library. Error code: %d\r\n", httpsClientStatus);

        return httpsClientStatus;
    }

    return httpsClientStatus;
}

/*******************************************************************************************************************//**
 * @brief      Connects to the server with all required connection configuration settings
 *
 * @param[in]  pNetworkCredentialInfo       network credentials consists of client certificate and private key
 *                                          which has to be update by user by following the process document
 * @param[in]  pNetworkInterface            Creates a network interface with the server
 * @retval     IOT_HTTPS_OK                 Upon successful client Initialization.
 * @retval     Any other Error Code         Upon unsuccessful client Initialization.
 **********************************************************************************************************************/
IotHttpsReturnCode_t connect_aws_client(const IotNetworkInterface_t *pNetworkInterface)
{
    /* The current attempt in the number of connection tries. */
    uint32_t connAttempt = RESET_VALUE;

    /* Set the connection configurations. */
    connConfig.pAddress = pAddress;
    connConfig.addressLen = addressLen;
    connConfig.port = IOT_DEMO_HTTPS_PORT;
    connConfig.pCaCert = IOT_DEMO_HTTPS_TRUSTED_ROOT_CA;
    connConfig.caCertLen = sizeof( IOT_DEMO_HTTPS_TRUSTED_ROOT_CA);
    connConfig.userBuffer.pBuffer = _pConnUserBuffer;
    connConfig.userBuffer.bufferLen = sizeof(_pConnUserBuffer);
    connConfig.pClientCert = keyCLIENT_CERTIFICATE_PEM;
    connConfig.clientCertLen = sizeof( keyCLIENT_CERTIFICATE_PEM);
    connConfig.pPrivateKey = keyCLIENT_PRIVATE_KEY_PEM;
    connConfig.privateKeyLen = sizeof( keyCLIENT_PRIVATE_KEY_PEM);
    connConfig.pNetworkInterface = pNetworkInterface;
    connConfig.flags = RESET_VALUE;

    /* Connect to server. */
    for (connAttempt = 1; connAttempt <= IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY; connAttempt++)
    {
        httpsClientStatus = IotHttpsClient_Connect (&connHandle, &connConfig);

        if ((httpsClientStatus == IOT_HTTPS_CONNECTION_ERROR) && (connAttempt < IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY))
        {
            APP_ERR_PRINT("Failed to connect the server, retrying after %ld ms.\r\n", IOT_DEMO_HTTPS_CONNECTION_RETRY_WAIT_MS);
            IotClock_SleepMs ( IOT_DEMO_HTTPS_CONNECTION_RETRY_WAIT_MS);
            continue;
        }
        else
        {
            break;
        }
    }
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        APP_PRINT("Unable to connect the server. Error code: %d.\r\n", httpsClientStatus);
        if (connHandle != NULL)
        {
            IotHttpsClient_Disconnect (connHandle);
        }

        /* Clean up the library because we are done using it. */
        IotHttpsClient_Cleanup ();
        return httpsClientStatus;
    }

    APP_PRINT("\r\nConnected to the server\r\n");
    return httpsClientStatus;
}
/*******************************************************************************************************************//**
 * @brief      Process GET request and read the response from the server
 *
 * @param[in]  None
 * @retval     IOT_HTTPS_OK                 Upon successful client Initialization.
 * @retval     Any other Error Code         Upon unsuccessful client Initialization.
 **********************************************************************************************************************/
IotHttpsReturnCode_t Process_GETRequest(void)
{
    /* Synchronous request specific configurations. */
    /* Set the configurations needed for a synchronous request. */
    reqSyncInfo.pBody = NULL; /* This is a GET request so there is no data in the body. */
    reqSyncInfo.bodyLen = RESET_VALUE; /* Since there is no data in the body the length is 0. */

    /* Set the request configurations. */
    reqConfig.pPath = IOT_DEMO_HTTPS_PRESIGNED_GET_URL;
    reqConfig.pathLen = strlen ((const char*) reqConfig.pPath);
    reqConfig.pHost = IOT_DEMO_HOST_ADDRESS;
    reqConfig.hostLen = strlen ((const char*) reqConfig.pHost);
    reqConfig.method = IOT_HTTPS_METHOD_GET;
    reqConfig.isNonPersistent = false;
    reqConfig.userBuffer.pBuffer = _pReqUserBuffer;
    reqConfig.userBuffer.bufferLen = sizeof(_pReqUserBuffer);
    reqConfig.isAsync = false;
    reqConfig.u.pSyncInfo = &reqSyncInfo;

    /* Handle identifying the HTTP response. This is valid after the response has been received with
     * IotHttpsClient_SendSync(). */
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;

    /* Set the configurations needed for a synchronous response. */
    respSyncInfo.pBody = _pRespBodyBuffer; /* This is a GET request so should configure a place to retreive the
     * response body. */
    respSyncInfo.bodyLen = sizeof(_pRespBodyBuffer); /* The length of the GET request's response body. This should be
     * greater than or equal to the size of the file requested, for the
     * best performance. */

    /* Set the response configurations. */
    respConfig.userBuffer.pBuffer = _pRespUserBuffer;
    respConfig.userBuffer.bufferLen = sizeof(_pRespUserBuffer);
    /* Even though the body is being ignored, #IotHttpsResponseInfo_t.pSyncInfo should only be set to NULL if the
     * response is being received asynchronously. */
    respConfig.pSyncInfo = &respSyncInfo;

    /* Initialize the server connection request */
    httpsClientStatus = IotHttpsClient_InitializeRequest (&reqHandle, &reqConfig);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        APP_PRINT("An error occurred in IotHttpsClient_InitializeRequest() with error code: %d", httpsClientStatus);
        /* Disconnect from the server even if the server may have already disconnected us. */
        if (connHandle != NULL)
        {
            IotHttpsClient_Disconnect (connHandle);
        }
        /* Clean up the library because we are done using it. */
        IotHttpsClient_Cleanup ();
        return httpsClientStatus;
    }
    /* Enable this private key if privacy of feed data visibility is set to Private. Ignore if it is set to Public.
     * By default PRIVATE_KEY is set DISABLE.
     */
#if PRIVATE_KEY
    /* Adding header to send https request in JSON format */
    httpsClientStatus = add_header (reqHandle);
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        return httpsClientStatus;
    }
#endif

    /* Send Synchronous request with the configured connections provided by the user */
    httpsClientStatus = IotHttpsClient_SendSync (connHandle, reqHandle, &respHandle, &respConfig, IOT_DEMO_HTTPS_SYNC_NO_DELAY);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        /* Due to network error synchronous request may fail. Try to reconnect to the server */
        httpsClientStatus = IotHttpsClient_Connect (&connHandle, &connConfig);
        if (httpsClientStatus != IOT_HTTPS_OK)
        {
            APP_ERR_PRINT("Failed receiving the response on a second try after a network error. The error code is: %d", httpsClientStatus);
            /* Disconnect from the server even if the server may have already disconnected us. */
            if (connHandle != NULL)
            {
                IotHttpsClient_Disconnect (connHandle);
            }
            /* Clean up the library because we are done using it. */
            IotHttpsClient_Cleanup ();
            return httpsClientStatus;
        }
    }

    /* Read the response status from the server */
    httpsClientStatus = IotHttpsClient_ReadResponseStatus (respHandle, &respStatus);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        APP_PRINT("Error in retrieving the response status. Error code %d", httpsClientStatus);
        /* Disconnect from the server even if the server may have already disconnected us. */
        if (connHandle != NULL)
        {
            IotHttpsClient_Disconnect (connHandle);
        }
        /* Clean up the library because we are done using it. */
        IotHttpsClient_Cleanup ();
        return httpsClientStatus;
    }

    APP_PRINT("Received data using GET Request = %s\n", respSyncInfo.pBody);

    /* Fetch the feed id from the response body which to be update in IOT_DEMO_HTTPS_PRESIGNED_PUT_URL */
    /* SynchronousrResponse body starts with the string in the format like [{\"id\":\"0ENQG7RYQA40W17G2A2SFH8E9Q\",\"...\"}]".
     *  So, to fetch the ID_KEY other portion of string is avoided*/
    strncpy (&id[INDEX_ZERO], (char *)&respSyncInfo.pBody[ID_START_INDEX], ID_LEN);
    is_get_called = true;   //setting the flag to avoid GET call in the PUT request
    return httpsClientStatus;
}

/*******************************************************************************************************************//**
 * @brief      Process PUT request by updating the latest data to the GET requested value in server
 *
 * @param[in]  None
 * @retval     IOT_HTTPS_OK                 Upon successful client Initialization.
 * @retval     Any other Error Code         Upon unsuccessful client Initialization.
 **********************************************************************************************************************/
IotHttpsReturnCode_t Process_PUTRequest(float mcu_die_temp)
{
    /* Update the IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL with ID key to update the latest value*/
    char pPath_url[URL_SIZE] = { RESET_VALUE };
    (void*) memcpy (pPath_url, IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL, strlen(IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL));

    /* Check if PUT request called prior to the GET request and for updating the feed id in PUT request url */
    if (true != is_get_called)
    {
        httpsClientStatus = Process_GETRequest ();
        if (IOT_HTTPS_OK != httpsClientStatus)
        {
            APP_ERR_PRINT("** Failed in PUT Request, while fetching the feed id ** \r\n");
            return httpsClientStatus;
        }
        is_get_called = false; //clearing the flag
    }

    /* Feed ID is appending to the IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL for processing the PUT Request */
    strncat (pPath_url, id, ID_LEN);
    /* The path is everything that is not the address. It also includes the query. So we get the strlen( pPath ) to
     * acquire everything following in IOT_DEMO_HTTPS_PRESIGNED_PUT_URL. */
    /* Set the request configurations. */
    reqConfig.pPath = pPath_url;
    reqConfig.pathLen = strlen ((const char*) reqConfig.pPath);
    reqConfig.pHost = IOT_DEMO_HOST_ADDRESS;
    reqConfig.hostLen = strlen ((const char*) reqConfig.pHost);
    reqConfig.method = IOT_HTTPS_METHOD_PUT;
    reqConfig.isNonPersistent = false;
    reqConfig.userBuffer.pBuffer = _pReqUserBuffer;
    reqConfig.userBuffer.bufferLen = sizeof(_pReqUserBuffer);
    reqConfig.isAsync = false;
    reqConfig.u.pSyncInfo = &reqSyncInfo;

    /* Set the configurations needed for a synchronous request. */
    char upload_str[SIZE_64] = { RESET_VALUE };
    snprintf (upload_str, SIZE_64, "{\"datum\":{\"value\":\"%.02f\"}}", mcu_die_temp);
    reqSyncInfo.pBody = (uint8_t*) (upload_str); /* Pointer to the file/buffer of data we want to upload. */
    reqSyncInfo.bodyLen = strlen ((const char*) reqSyncInfo.pBody);

    /* Set the response configurations. */
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    respConfig.userBuffer.pBuffer = _pRespUserBuffer;
    respConfig.userBuffer.bufferLen = sizeof(_pRespUserBuffer);
    /* Even though the body is being ignored, #IotHttpsResponseInfo_t.pSyncInfo should only be set to NULL if the
     * response is being received asynchronously. */
    respConfig.pSyncInfo = &respSyncInfo;

    /* Initialize server connection request */
    httpsClientStatus = IotHttpsClient_InitializeRequest (&reqHandle, &reqConfig);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        APP_PRINT("An error occurred in PUT Request at IotHttpsClient_InitializeRequest() with error code: %d", httpsClientStatus);
        /* Disconnect from the server even if the server may have already disconnected us. */
        if (connHandle != NULL)
        {
            IotHttpsClient_Disconnect (connHandle);
        }
        /* Clean up the library because we are done using it. */
        IotHttpsClient_Cleanup ();
        return httpsClientStatus;
    }

    /* Adding header to send https request in JSON format */
    httpsClientStatus = add_header (reqHandle);
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        return httpsClientStatus;
    }

    /* Send Synchronous request with the configured connections provided by the user */
    httpsClientStatus = IotHttpsClient_SendSync (connHandle, reqHandle, &respHandle, &respConfig, IOT_DEMO_HTTPS_SYNC_NO_DELAY);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        /* Due to network error synchronous request may fail. Try to reconnect to the server */
        httpsClientStatus = IotHttpsClient_Connect (&connHandle, &connConfig);
        /* Error Handler */
        if (httpsClientStatus != IOT_HTTPS_OK)
        {

            APP_PRINT("Failed receiving the response on a second try after a network error. The error code is: %d", httpsClientStatus);

            /* Disconnect from the server even if the server may have already disconnected us. */
            if (connHandle != NULL)
            {
                IotHttpsClient_Disconnect (connHandle);
            }
            /* Clean up the library because we are done using it. */
            IotHttpsClient_Cleanup ();
            return httpsClientStatus;
        }
    }

    /* Read the response status from the server */
    httpsClientStatus = IotHttpsClient_ReadResponseStatus (respHandle, &respStatus);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {

        APP_PRINT("Error in retrieving the response status. Error code %d", httpsClientStatus);

        /* Disconnect from the server even if the server may have already disconnected us. */
        if (connHandle != NULL)
        {
            IotHttpsClient_Disconnect (connHandle);
        }
        /* Clean up the library because we are done using it. */
        IotHttpsClient_Cleanup ();
        return httpsClientStatus;
    }

    APP_PRINT("Received data using PUT Request = %s\n", respSyncInfo.pBody);

    return httpsClientStatus;
}
/*******************************************************************************************************************//**
 * @brief      Process POST request with updating the mcu die temperature into the server
 *
 * @param[in]  None
 * @retval     IOT_HTTPS_OK                 Upon successful client Initialization.
 * @retval     Any other Error Code         Upon unsuccessful client Initialization.
 **********************************************************************************************************************/

IotHttpsReturnCode_t Process_POSTRequest(float mcu_die_temp)
{
    /* Set the request configurations. */
    reqConfig.pPath = IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL;
    reqConfig.pathLen = strlen ((const char*) reqConfig.pPath);
    reqConfig.pHost = IOT_DEMO_HOST_ADDRESS;
    reqConfig.hostLen = strlen ((const char*) reqConfig.pHost);
    reqConfig.method = IOT_HTTPS_METHOD_POST;
    reqConfig.isNonPersistent = false;
    reqConfig.userBuffer.pBuffer = _pReqUserBuffer;
    reqConfig.userBuffer.bufferLen = sizeof(_pReqUserBuffer);
    reqConfig.isAsync = false;
    reqConfig.u.pSyncInfo = &reqSyncInfo;

    /* Set the configurations needed for a synchronous info request. */
    char upload_str[SIZE_64] = { RESET_VALUE }; //to upload the mcu die temperature value to the requested url
    snprintf (upload_str, SIZE_64, "{\"datum\":{\"value\":\"%.02f\"}}", mcu_die_temp); //formating into string to send in JSON format
    reqSyncInfo.pBody = (uint8_t*) (upload_str); /* Pointer to the file/buffer of data we want to upload. */
    reqSyncInfo.bodyLen = strlen ((const char*) reqSyncInfo.pBody);

    /* Set the response configurations. */
    IotHttpsResponseHandle_t respHandle = IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER;
    respConfig.userBuffer.pBuffer = _pRespUserBuffer;
    respConfig.userBuffer.bufferLen = sizeof(_pRespUserBuffer);
    /* Even though the body is being ignored, #IotHttpsResponseInfo_t.pSyncInfo should only be set to NULL if the
     * response is being received asynchronously. */
    respConfig.pSyncInfo = &respSyncInfo;

    /* Initialize server connection request */
    httpsClientStatus = IotHttpsClient_InitializeRequest (&reqHandle, &reqConfig);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        APP_PRINT("An error occurred in POST Request at IotHttpsClient_InitializeRequest() with error code: %d", httpsClientStatus);
        /* Disconnect from the server even if the server may have already disconnected us. */
        if (connHandle != NULL)
        {
            IotHttpsClient_Disconnect (connHandle);
        }
        /* Clean up the library because we are done using it. */
        IotHttpsClient_Cleanup ();
        return httpsClientStatus;
    }

    /* Adding header to send https request in JSON format */
    httpsClientStatus = add_header (reqHandle);
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        return httpsClientStatus;
    }

    /* Send Synchronous request with the configured connections provided by the user */
    httpsClientStatus = IotHttpsClient_SendSync (connHandle, reqHandle, &respHandle, &respConfig, IOT_DEMO_HTTPS_SYNC_NO_DELAY);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        /* Due to network error synchronous request may fail. Try to reconnect to the server */
        httpsClientStatus = IotHttpsClient_Connect (&connHandle, &connConfig);
        /* Error Handler */
        if (httpsClientStatus != IOT_HTTPS_OK)
        {
            APP_PRINT("Failed receiving the response on a second try after a network error. The error code is: %d", httpsClientStatus);
            /* Disconnect from the server even if the server may have already disconnected us. */
            if (connHandle != NULL)
            {
                IotHttpsClient_Disconnect (connHandle);
            }
            /* Clean up the library because we are done using it. */
            IotHttpsClient_Cleanup ();
            return httpsClientStatus;
        }
    }

    /* Read the response status from the server */
    httpsClientStatus = IotHttpsClient_ReadResponseStatus (respHandle, &respStatus);
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        APP_PRINT("Error in retrieving the response status. Error code %d", httpsClientStatus);
        /* Disconnect from the server even if the server may have already disconnected us. */
        if (connHandle != NULL)
        {
            IotHttpsClient_Disconnect (connHandle);
        }
        /* Clean up the library because we are done using it. */
        IotHttpsClient_Cleanup ();
        return httpsClientStatus;
    }

    APP_PRINT("Received data using POST Request = %s\n", respSyncInfo.pBody);

    return httpsClientStatus;
}

/*******************************************************************************************************************//**
 * @brief      This function adds the header for https request in JSON format.
 *             User has to update their Active Key generated from io.adafruit.com site in the ACTIVE_KEY macro.
 *
 * @param[in]  pReqHandle                   request handle should be updated according to the GET, PUT, POST methods.
 * @retval     IOT_HTTPS_OK                 Upon successful client Initialization.
 * @retval     Any other Error Code         Upon unsuccessful client Initialization.
 **********************************************************************************************************************/
IotHttpsReturnCode_t add_header(IotHttpsRequestHandle_t pReqHandle)
{
    /* Adding header to send in JSON format */
    httpsClientStatus = IotHttpsClient_AddHeader (pReqHandle, "Content-Type", strlen ("Content-Type"),
                                                  "application/json", strlen ("application/json"));
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        APP_PRINT("An error occurred at adding json format in IotHttpsClient_AddHeader() with error code: %d",
                  httpsClientStatus);
        /* Disconnect from the server even if the server may have already disconnected us. */
        if (connHandle != NULL)
        {
            IotHttpsClient_Disconnect (connHandle);
        }
        /* Clean up the library because we are done using it. */
        IotHttpsClient_Cleanup ();
        return httpsClientStatus;
    }

    /**
     * User has to update their Generated Active Key in the ACTIVE_KEY macro"
     **/
    httpsClientStatus = IotHttpsClient_AddHeader (pReqHandle, "X-AIO-Key", strlen ("X-AIO-Key"), ACTIVE_KEY, strlen (ACTIVE_KEY));
    /* Error Handler */
    if (httpsClientStatus != IOT_HTTPS_OK)
    {
        APP_PRINT("An error occurred at adding Active Key in IotHttpsClient_AddHeader() with error code: %d", httpsClientStatus);
        /* Disconnect from the server even if the server may have already disconnected us. */
        if (connHandle != NULL)
        {
            IotHttpsClient_Disconnect (connHandle);
        }
        /* Clean up the library because we are done using it. */
        IotHttpsClient_Cleanup ();
        return httpsClientStatus;
    }
    return httpsClientStatus;
}
/*******************************************************************************************************************//**
 * @} (end defgroup aws_https_client_ep)
 **********************************************************************************************************************/
