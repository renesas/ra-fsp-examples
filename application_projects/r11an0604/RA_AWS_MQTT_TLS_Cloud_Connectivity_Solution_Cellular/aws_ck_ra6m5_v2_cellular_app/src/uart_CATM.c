/***********************************************************************************************************************
 * File Name    : uart_CATM.c
 * Description  : Contains UART functions for interface to CATM module
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
 ***********************************************************************************************************************/

#define UART_CATM_C

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSconfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"

#include "uart_CATM.h"

#include "r_sci_uart.h"
#include "r_uart_api.h"
FSP_HEADER

char g_print_buf[BUFFER_LINE_LENGTH];

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_catm1_uart_ctrl;
extern const uart_cfg_t g_catm1_uart_cfg;

#define SHOW_RESPONSES

#define ONE_DELAYMSEC(delayMs)    (vTaskDelay(pdMS_TO_TICKS(delayMs)))

/*
 * Private function declarations
 */

/*
 * Private global variables
 */
/* Temporary buffer to save data from receive buffer for further processing */
static uint8_t g_temp_buffer[UART_MAX_DATA_LENGTH] = {RESET_VALUE};

/* Counter to update g_temp_buffer index */
static volatile uint8_t g_counter_var = RESET_VALUE;

/* Flag for user callback */
static volatile uint8_t g_uart_event = RESET_VALUE;

/*******************************************************************************************************************//**
 * @brief       removeLeadingCrLf function
 * @param       p2Str       The pointer points to the address of the string
 * @retval      None
 ***********************************************************************************************************************/
void removeLeadingCrLf(uint8_t *p2Str);
void removeLeadingCrLf(uint8_t *p2Str)
{
    uint8_t prevLength = (uint8_t)strlen((char *)p2Str);
    uint8_t removedBytes = 0;

    for (int ii=0; ii<prevLength; ii++)
    {
        if ( (*(p2Str+ii) == 0x0A) || (*(p2Str+ii) == 0x0D))
        {
            removedBytes++;
        }
        else
        {
            break;
        }
    }
    strcpy ((char *)p2Str, (char *)(p2Str+removedBytes));  // temp buffer should always have a null terminator
}

/*******************************************************************************************************************//**
 * @brief       Return the first string found (terminated by null, CR, or LF)
 * @param       p2Dest      The pointer points to the address of the destination string
 * @param[in]   p2Src       The pointer points to the address of the source string
 * @retval      True        The first string found successful
 * @retval      False       Nothing found
 ***********************************************************************************************************************/
bool getFirstString(uint8_t *p2Dest, uint8_t *p2Src);
bool getFirstString(uint8_t *p2Dest, uint8_t *p2Src)
{
    uint8_t srcLength = (uint8_t)strlen((char *)p2Src);
    uint8_t count = 0;
    bool result = false;    // return false if nothing found besides terminator char's

    for (int ii=0; ii<srcLength; ii++)
    {
        if ( (*(p2Src+ii) == 0x0A) || (*(p2Src+ii) == 0x0D) || (*(p2Src+ii) == 0))
        {
            break;
        }
        else
        {
            count++;
        }
    }
    if (count > 0)
    {
        strncpy ((char *)p2Dest, (char *)(p2Src), count);  // temp buffer should always have a null terminator
        result = true;
    }
    return (result);
}

/*******************************************************************************************************************//**
 * @brief       Handle response
 * @param       p2Result        The pointer points to the address of the destination string
 * @retval      uint8_t         The length received (0 if none)
 ***********************************************************************************************************************/
uint8_t getResponse(char *p2Result);
uint8_t getResponse(char *p2Result)
{
    uint8_t input_length = 0;
    uint8_t last_length = 0;
    volatile bool b_valid_data = true;
    int loopCounter = 0;

    // Wait for a response
    while (true)
    {
        if((g_counter_var > 0) ||
                ((loopCounter > 1000) && (g_counter_var > 0)))    // this is only if we have data, but no terminator (may still need it)
        {
            ONE_DELAYMSEC(10);  // wait a few more to catch the whole string

            /* Calculate g_temp_buffer length */
            input_length = ((uint8_t)(strlen((char *) &g_temp_buffer)));    // buffer is filled by interrupt callback routine

            /* Check if input data length is in limit */
            if (UART_MAX_DATA_LENGTH > (uint8_t)input_length)   // forces the string to have a null terminator at the end
            {
                /* This loop validates input data byte by byte to filter out non-text char's.
                 * Any such data will be considered as invalid. */
                for(int buf_index = RESET_VALUE; buf_index < input_length; buf_index++)
                {
                    if(g_temp_buffer[buf_index] >= SPACE_CHAR && g_temp_buffer[buf_index] <= TILDE_CHAR)
                    {
                        /* Set b_valid_data Flag as byte is valid */
                        b_valid_data = true;
                    }
                    else if ((g_temp_buffer[buf_index] == '\r') || (g_temp_buffer[buf_index] == '\n'))
                    {
                        // skip these
                    }
                    else
                    {
                        /* Clear data_valid flag as data is not valid, Clear the buffer and break the loop */
                        memset(g_temp_buffer, RESET_VALUE, UART_MAX_DATA_LENGTH);
                        b_valid_data = false;
                        input_length = 0;
                        break;
                    }
                }
            }

            if ((b_valid_data == true) && (last_length == input_length))    // check to see when no additional data has been added
            {
                // A valid string is ready to return
                // now remove any leading CR/LF characters
                removeLeadingCrLf(g_temp_buffer);
                break;
            }
            else
            {
                last_length = input_length;     // for comparison in the next loop
                ONE_DELAYMSEC(10);  // wait a few more to see if there's more data
            }
        }
        ONE_DELAYMSEC(1);
        if (loopCounter++ > 1000)   // 1 second timeout
        {
            APP_DBG_PRINT("\r\n *** loopCounter Timeout, g_counter_var=%d, len=%d", g_counter_var, input_length);
            break;
        }
    } // while(true)

    // Then copy to the passed string buffer
    if (input_length > 0)
    {
        #ifdef EXTRA_DIAG_OUTPUTS
            ONE_PRINT("\r\nString returned=%s", g_temp_buffer);
        #endif
        strcpy (p2Result, (char *)&g_temp_buffer);  // temp buffer should always have a null terminator
        memset(g_temp_buffer, RESET_VALUE, UART_MAX_DATA_LENGTH);
        g_counter_var = RESET_VALUE;
    }

    return (input_length);
}

/*******************************************************************************************************************//**
 * @brief       Find string in string
 * @param[in]   p2Str           The pointer points to the address of the source string
 * @param[in]   p2LookingFor    The pointer points to the address of string
 * @retval      True            Equal
 * @retval      False           Not equal
 ***********************************************************************************************************************/
bool isInStr(char *p2Str, char *p2LookingFor);
bool isInStr(char *p2Str, char *p2LookingFor)
{
    bool    result = false;
    char    *p2result = strstr(p2Str,p2LookingFor);             // find it here
    size_t  strSize = strlen(p2LookingFor);
    int     intTemp = strncmp(p2LookingFor, p2result, strSize); // compare the exact string
    if (intTemp == 0)   // are they equal?
    {
        result = true;
    }

    return (result);
}

/*******************************************************************************************************************//**
 * @brief       Check a response string
 * @param[in]   p2ExpectedStr   The pointer points to the address of the string
 * @retval      True            Valid string
 * @retval      False           Invalid string
 ***********************************************************************************************************************/
bool isGetExpectedResponse(char *p2ExpectedStr);
bool isGetExpectedResponse(char *p2ExpectedStr)
{
    bool    result = false;
    uint8_t rcvdLength = 0;
    char    tempBuffer[UART_MAX_DATA_LENGTH] = {RESET_VALUE}; // use an extra character to assure a null-terminator

    if ((rcvdLength = getResponse(tempBuffer)) > 0)
    {
        // Then compare to the passed string
        result = isInStr(tempBuffer, p2ExpectedStr);
    }
    else
    {
        APP_DBG_PRINT("\r\nNo valid string returned!!!");
    }

    if (result == false)
    {
        sprintf (g_print_buf, "\r\n\nrcvdLength=%d, expected=%s, rcvd=%s\r\n", rcvdLength, p2ExpectedStr, tempBuffer);
        APP_DBG_PRINT(g_print_buf);
    }
    return (result);
}

/*******************************************************************************************************************//**
 * @brief       Read all response data on the receive line and output to the terminal
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
void outputAllResponseData( void );
void outputAllResponseData( void )
{
    uint8_t rcvdLength = 0;
    char    tempBuffer[UART_MAX_DATA_LENGTH] = {RESET_VALUE}; // use an extra character to assure a null-terminator

    while (true)
    {
        if ((rcvdLength = getResponse(tempBuffer)) > 0)
        {
            sprintf (g_print_buf, "\r\n%s", tempBuffer);
            APP_DBG_PRINT(g_print_buf);
        }
        else
            break;

    }

}

/*******************************************************************************************************************//**
 * @brief       Given an input string, search for a string inside delimiter characters
 * @param       p2Results       The pointer points to the address of the destination string
 * @param[in]   p2InputStr      The pointer points to the address of the source string
 * @param[in]   delimiter       Delimiter characters
 * @param[in]   targetIndex
 * @retval      1               Found string
 * @retval      0               String not found
 ***********************************************************************************************************************/
uint8_t parseString(uint8_t *p2Results, uint8_t *p2InputStr, uint8_t delimiter, uint8_t targetIndex);
uint8_t parseString(uint8_t *p2Results, uint8_t *p2InputStr, uint8_t delimiter, uint8_t targetIndex)
{
    uint8_t result = 0;
    uint8_t ii=0; //, counter=0;
    uint8_t size = (uint8_t)strlen((const char *)p2InputStr);
    bool    isDelimiter = false;
    uint8_t resultStr[UART_MAX_DATA_LENGTH] = {RESET_VALUE};
    uint8_t resultCounter = 0;
    uint8_t currIndex = 0;
    uint8_t tempByte;


    while (result == 0)
    {
        isDelimiter = false;

        for (; ii<size && isDelimiter==false; ii++)
        {
            tempByte = *(p2InputStr+ii);
            if (tempByte == delimiter)
            {
                isDelimiter = true;

                if (delimiter == '\r')  // stop at the first CR if that is the delimiter
                {
                    isDelimiter = false;    // stop looking for this
                    strncpy((char *)p2Results, (const char *)p2InputStr, ii);
                    result = 1;
                }
            }
        }

        if (isDelimiter == true)        // now get the string
        {
            currIndex++;
            isDelimiter = false;
            for (resultCounter=0 ; ii<size &&  isDelimiter==false; ii++)
            {
                if (*(p2InputStr+ii) == delimiter)  // done?
                {
                    isDelimiter = true;
                }
                else
                {
                    resultStr[resultCounter++] = *(uint8_t *)(p2InputStr+ii);
                }
            }

            //if ((resultCounter > 0) && (targetIndex == currIndex))
             if (targetIndex == currIndex)
            {
                strcpy((char *)p2Results, (const char *)resultStr);
                result = 1;
            }
        }
        else        // nothing here...
            break;
    }
    return (result);
}


/*******************************************************************************************************************//**
 * @brief       Initialize UART.
 * @param[in]   None
 * @retval      FSP_SUCCESS         Upon successful open and start of timer
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t CATM_uart_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Initialize UART channel with baud rate 115200 */
    fsp_err = R_SCI_UART_Open (&g_catm1_uart_ctrl, &g_catm1_uart_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        APP_DBG_PRINT("\r\n**  R_SCI_UART_Open API failed  **\r\n");
    }
    return fsp_err;
}


/*******************************************************************************************************************//**
 * @brief       Get a response when pinging
 * @param       PingTimeout     Ping status
 * @retval      True            Ping successfully
 * @retval      False           Ping unsuccessfully
 ***********************************************************************************************************************/
static bool isGetExpectedResponsePing(bool * PingTimeout);
static bool isGetExpectedResponsePing(bool * PingTimeout)
{
    bool    result = false;
    uint8_t rcvdLength = 0;
    char    tempBuffer[UART_MAX_DATA_LENGTH] = {RESET_VALUE}; // use an extra character to assure a null-terminator

    if (PingTimeout)
    {
        *PingTimeout = false;
    }

    if ((rcvdLength = getResponse(tempBuffer)) > 0)
    {
        // Then compare to the string : PING timeout condition
        result = isInStr(tempBuffer, "+PING: 1,9.9.9.9,-1,-1");
        if ( true == result )
        {
            *PingTimeout = true;
        }
        else
        {
            // Check for PING with a non-zero Time and non-zero TTL
            // Time value will vary, TTL will vary
            *PingTimeout = false;
            // Then compare to the string : "+PING: 1,<ipaddr>,"
            result = isInStr(tempBuffer, "+PING:");
        }
        sprintf (g_print_buf, "\r\n\nrcvdLength=%d, rcvd=%s\r\n", rcvdLength, tempBuffer);
        printf_colour (g_print_buf);
    }
    else
    {
        printf_colour("\r\nNo valid string returned!!!");
    }

    if ( false == result )
    {
        R_BSP_SoftwareDelay(2000, BSP_DELAY_UNITS_MILLISECONDS);//ONE_DelayMsec(1000);
    }
    return result;
}

/*******************************************************************************************************************//**
 * @brief       This function resolves the CATM query
 * @param[in]   p2cmdStr            The pointer points to the address of the source string
 * @param[in]   p2LookForStr        The pointer points to the address of string
 * @param[in]   delim               Delimiter characters
 * @param       p2resultStr         The pointer points to the address of the destination string
 * @retval      FSP_SUCCESS         Resolves the CATM query successfully
 * @retval      Any other error     Resolves the CATM query unsuccessfully
 ***********************************************************************************************************************/
fsp_err_t CATM_query(char *p2cmdStr, char *p2LookForStr, char delim, char *p2resultStr);
fsp_err_t CATM_query(char *p2cmdStr, char *p2LookForStr, char delim, char *p2resultStr)
{
    fsp_err_t status = FSP_SUCCESS;
    uint8_t rcvdLength = 0;
    char    tempBuffer[UART_MAX_DATA_LENGTH] = {RESET_VALUE}; // use an extra character to assure a null-terminator
    uint8_t retries = 5;

    sprintf (g_print_buf, "\r\n-> %s", p2cmdStr);
    APP_DBG_PRINT(g_print_buf);
    sprintf(tempBuffer, "%s\r\n", p2cmdStr);
    status = CATM_uart_print_user_msg((uint8_t *)tempBuffer);
    retries = 0;
    while (true)
    {
        memset(tempBuffer, RESET_VALUE, UART_MAX_DATA_LENGTH);
        if ((rcvdLength = getResponse(tempBuffer)) > 0)
        {
            #ifdef SHOW_RESPONSES
                sprintf (g_print_buf, "\r\n<- %s", tempBuffer);
                APP_DBG_PRINT(g_print_buf);
            #endif

            if (isInStr(tempBuffer, p2LookForStr))
            {
                // now parse the data, looking for the 19 character number
                if (parseString((uint8_t *)p2resultStr, (uint8_t *)tempBuffer, (uint8_t)delim, 1) == 1)
                {
                    #ifdef SHOW_RESPONSES
                        sprintf (g_print_buf, "\r\n<- %s found", p2LookForStr);
                        APP_DBG_PRINT(g_print_buf);
                    #endif
                    break;
                }
                else
                {
                    sprintf (g_print_buf, "\r\n<- %s failed, tempBuffer=%s", p2cmdStr, tempBuffer);
                    APP_DBG_PRINT(g_print_buf);
                    status = FSP_ERR_TIMEOUT;
                }
            }
        }
        if (retries++ >= 5)    // max retries?
        {
            status = FSP_ERR_TIMEOUT;
            break;
        }
        else
        {
            ONE_DELAYMSEC(1000);
            APP_DBG_PRINT("\r\n___nothing yet");
        }
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief       This function pings the CATM
 * @param[in]   None
 * @retval      FSP_SUCCESS         Ping successfully
 * @retval      Any other error     Ping unsuccessfully
 ***********************************************************************************************************************/
fsp_err_t CATM_Ping(void)
{
    fsp_err_t status = FSP_SUCCESS;
    uint8_t retries = 0;
    bool bPingTimeout;

    // Set Ping Echo Request Mode to Synchronous
    status = CATM_query("AT+PINGCFG=0", "OK", 'X', NULL);
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);//ONE_DelayMsec(1000);

    // Send AT command to measure Network Received Power (Signal Received Quality)
    status = CATM_query("AT+CESQ", "OK", ':', NULL);
    if (FSP_SUCCESS != status)
    {
        printf_colour ("\r\n ** Received Signal  Power is low ** \r\n");
    }
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);//ONE_DelayMsec(1000);


    while (true)
    {
        printf_colour ("\r\n-> AT+PING=\"9.9.9.9\",1,32,15  \r\n");
        // Send AT command to RYZ014A to PING Server
        status = CATM_uart_print_user_msg((uint8_t *)"AT+PING=\"9.9.9.9\",1,32,15\r\n");
        if (FSP_SUCCESS != status)
        {
            printf_colour ("\r\n ** PING FAILED ** \r\n");
            R_BSP_SoftwareDelay(3000, BSP_DELAY_UNITS_MILLISECONDS);//ONE_DelayMsec(3000);
        }
        else if (isGetExpectedResponsePing(&bPingTimeout) == true)
        {
            // PING timeout occurred : SIM maybe Inactive or NETWORK can not be reached
            if (bPingTimeout == true)
            {
                // Server did not respond:  RYZ024A responded with "+PING: 1,<ip addr>,-1,-1"
                // where Time = -1, TTL = -1
                printf_colour ("\r\n<- +PING Timeout : SIM Not Active or Network can not be reached\r\n");
                status = FSP_ERR_TIMEOUT;
            }
            else
            {
                // Server Responded with "+PING: 1,<ip addr>,Time, TTL", where Time > 0, and TTL > 0
                printf_colour ("\r\n<- +PING returned \r\n");
                printf_colour ("\r\nPing Successful !!!\r\n");
                printf_colour ("\r\nSIM Activated   !!!\r\n");
                status = FSP_SUCCESS;
            }
            break;
        }
        else
        {
            printf_colour ("\r\n ** +PING Response not returned ** \r\n");
            status = FSP_ERR_TIMEOUT;
            CATM_query("AT+PINGCFG=2", "OK", 'X', NULL);   // stop pinging
            R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);//ONE_DelayMsec(1000);
        }

        if (retries++ >= 3)
        {
            // max retries exceeded
            status = FSP_ERR_TIMEOUT;
            CATM_query("AT+PINGCFG=2", "OK", 'X', NULL);   // stop pinging
            printf_colour ("\r\n ** Retries Exceeded : +PING Response not returned ** \r\n");
            printf_colour ("\r\n ** SIM Not Active or Network can not be reached ** \r\n");
            printf_colour ("\r\n ** Check SIM card or Antenna Attached ** \r\n");
            break;
        }
    }

    return (status);
}

/*******************************************************************************************************************//**
 * @brief       Initialize the CATM
 * @param[in]   None
 * @retval      FSP_SUCCESS         Initialize the CATM successfully
 * @retval      Any other error     Initialize the CATM unsuccessfully
 ***********************************************************************************************************************/
fsp_err_t CATM_initialize(void)
{
    fsp_err_t status = FSP_SUCCESS;
    uint8_t rcvdLength = 0;
    char    tempBuffer[UART_MAX_DATA_LENGTH] = {RESET_VALUE}; // use an extra character to assure a null-terminator
    uint8_t retries = 5;
    uint8_t loopCounter = 0;

    // Send commands and look for proper responses
    // Command              Response        Notes
    // -------              --------        -----
    // AT                   OK              confirm the connection is good
    // AT^RESET             OK              To ensure the CATM Module is in a known state
    // AT+SQNAUTOCONNECT=1  OK              So that CATM module will boot with he radio on
    // AT+CFUN=1            OK              To turn the radio on for the first time
    // AT+CGSN              111111111111111            Read the IMEI of the CATM module
    // AT+SQNCCID           +SQNCCID: "1111111111111111111",""
    // ---                                  Read the ICCID of the sim card

    APP_DBG_PRINT("\r\nInit the CAT-M module\r\n-> AT");
    for (int ii=0; ii<10; ii++)
    {
        status = CATM_uart_print_user_msg((uint8_t *)"AT\r\n");
        if (FSP_SUCCESS != status)
        {
            APP_DBG_PRINT ("\r\n ** UART TX FAILED ** \r\n");
            return(status);
        }
        else if (isGetExpectedResponse("OK") == true) {
            APP_DBG_PRINT("\r\n<- OK");
            break;
        }
        else
        {
            APP_DBG_PRINT("\r\n ** Response not returned **");
            status = FSP_ERR_TIMEOUT;
        }
        ONE_DELAYMSEC(100);
    }
    if (FSP_SUCCESS != status)
    {
        ONE_DELAYMSEC(10000);
        return (status);
    }

    loopCounter = 0;
    while (true)
    {
        APP_DBG_PRINT("\r\n-> AT^RESET");
        status = CATM_uart_print_user_msg((uint8_t *)"AT^RESET\r\n");
        if (FSP_SUCCESS != status)
        {
            APP_DBG_PRINT("\r\n ** UART TX FAILED ** \r\n");
            return(status);
        }
        if (isGetExpectedResponse("OK") == true) {
            APP_DBG_PRINT("\r\n<- OK");
        }
        else    {
            APP_DBG_PRINT("\r\n ** Response not returned **");
            status = FSP_ERR_TIMEOUT;
        }

        // After reset, it sends a SHUTDOWN until it starts back up, then it sends a SYSSTART
        ONE_DELAYMSEC(3000);    // 1 sec is too little, 2 sec's works, but 3 to be safe
        APP_DBG_PRINT("\r\nConfirm Re-Start\r\n-> AT");
        status = CATM_uart_print_user_msg((uint8_t *)"AT\r\n");
        if (FSP_SUCCESS != status)
        {
            APP_DBG_PRINT("\r\n ** UART TX FAILED ** \r\n");
            return(status);
        }
        if (isGetExpectedResponse("+SYSSTART") == true) {
            APP_DBG_PRINT("\r\n<- +SYSSTART");
            break;
        }
        else    {
            APP_DBG_PRINT("\r\n ** Response not returned **");
            status = FSP_ERR_TIMEOUT;
        }

        if (loopCounter++ > 5)
        {
            return (status);
        }

        ONE_DELAYMSEC(1000);
    }


    ONE_DELAYMSEC(1000);
    APP_DBG_PRINT("\r\n-> AT+SQNAUTOCONNECT=1"); // So that CATM module will boot with the radio on
    status = CATM_uart_print_user_msg((uint8_t *)"AT+SQNAUTOCONNECT=1\r\n");
    if (FSP_SUCCESS != status)
    {
        APP_DBG_PRINT("\r\n ** UART TX FAILED ** \r\n");
        return(status);
    }
    if (isGetExpectedResponse("OK") == true) {
        APP_DBG_PRINT("\r\n<- OK");
    }
    else {
        APP_DBG_PRINT("\r\n ** Response not returned **");
        return(FSP_ERR_TIMEOUT);
    }


    ONE_DELAYMSEC(1000);
    APP_DBG_PRINT("\r\n-> AT+CFUN=1"); // To turn the radio on for the first time
    // about a 1/2 sec delay for response
    status = CATM_uart_print_user_msg((uint8_t *)"AT+CFUN=1\r\n");
    if (FSP_SUCCESS != status)
    {
        APP_DBG_PRINT("\r\n ** UART TX FAILED ** \r\n");
        return(status);
    }
    if (isGetExpectedResponse("OK") == true) {
        APP_DBG_PRINT("\r\n<- OK");
    }
    else {
        APP_DBG_PRINT("\r\n ** Response not returned **");
        return(FSP_ERR_TIMEOUT);
    }


    ONE_DELAYMSEC(1000);
    APP_DBG_PRINT("\r\n-> AT+CGSN"); // Read the IMEI of the CATM module
    status = CATM_uart_print_user_msg((uint8_t *)"AT+CGSN\r\n");
    if (FSP_SUCCESS != status)
    {
        APP_DBG_PRINT("\r\n ** UART TX FAILED ** \r\n");
        return(status);
    }
    else    // look for the proper response
    {       // could be:  "OK",
        retries = 0;
        while (true)
        {
            memset(tempBuffer, RESET_VALUE, UART_MAX_DATA_LENGTH);
            if ((rcvdLength = getResponse(tempBuffer)) > 0)
            {
                    sprintf (g_print_buf, "\r\n<- %s", tempBuffer);
                    APP_DBG_PRINT(g_print_buf);

                if (isInStr(tempBuffer, "OK"))
                {
                    // now parse the data, looking for the 19 character number
                    //    char    *strtok (char *__restrict, const char *__restrict);
                    //    strtok is not working as expected, so do it manually...
                    if (getFirstString((uint8_t *)g_strIMEI, (uint8_t *)tempBuffer) == true)
                    {
                        sprintf (g_print_buf, "\r\n<- IMEI=%s", g_strIMEI);
                        APP_DBG_PRINT(g_print_buf);
                        break;
                    }
                    else
                    {
                        sprintf (g_print_buf, "\r\n<- IMEI failed, tempBuffer=%s", tempBuffer);
                        APP_DBG_PRINT(g_print_buf);
                        status = FSP_ERR_TIMEOUT;
                    }
                }
            }

            if (retries++ >= 5)    // max retries?
            {
                status = FSP_ERR_TIMEOUT;
                break;
            }
            else
            {
                ONE_DELAYMSEC(1000);
                APP_DBG_PRINT("\r\n...nothing yet");
            }
        }
    }
    if (FSP_SUCCESS != status)
    {
        return(status);
    }

    ONE_DELAYMSEC(1000);
    APP_DBG_PRINT("\r\n-> AT+SQNCCID");
    status = CATM_uart_print_user_msg((uint8_t *)"AT+SQNCCID\r\n");
    if (FSP_SUCCESS != status)
    {
        APP_DBG_PRINT("\r\n ** UART TX FAILED ** \r\n");
        return(status);
    }
    else    // look for the proper response
    {       // could be:  "OK",
        retries = 0;
        while (true)
        {
            memset(tempBuffer, RESET_VALUE, UART_MAX_DATA_LENGTH);
            if ((rcvdLength = getResponse(tempBuffer)) > 0)
            {
                    sprintf (g_print_buf, "\r\n<- %s", tempBuffer);
                    APP_DBG_PRINT(g_print_buf);

                if (isInStr(tempBuffer, "OK"))
                {
                    // now parse the data, looking for the 19 character number
                    //    char    *strtok (char *__restrict, const char *__restrict);
                    //    strtok is not working as expected, so do it manually...
                    if (parseString((uint8_t *)g_strICCID, (uint8_t *)tempBuffer, '"', 1) == 1)
                    {
                        sprintf (g_print_buf, "\r\n<- ICCID=%s", g_strICCID);
                        APP_DBG_PRINT(g_print_buf);
                        break;
                    }
                    else
                    {
                        sprintf (g_print_buf, "\r\n<- ICCID failed, tempBuffer=%s", tempBuffer);
                        APP_DBG_PRINT(g_print_buf);
                        status = FSP_ERR_TIMEOUT;
                    }
                }
            }
            if (retries++ >= 5)    // max retries?
            {
                status = FSP_ERR_TIMEOUT;
                break;
            }
            else
            {
                ONE_DELAYMSEC(1000);
                APP_DBG_PRINT("\r\n....nothing yet");
            }
        }
    }
    if (FSP_SUCCESS != status)
    {
        return(status);
    }

    return (status);
}


/*****************************************************************************************************************
 *  @brief       print user message to terminal
 *  @param[in]   p_msg
 *  @retval      FSP_SUCCESS                Upon success
 *  @retval      FSP_ERR_TRANSFER_ABORTED   Upon event failure
 *  @retval      Any Other Error code apart from FSP_SUCCESS,  Unsuccessful write operation
 ****************************************************************************************************************/
fsp_err_t CATM_uart_print_user_msg(uint8_t *p_msg)
{
    fsp_err_t fsp_err   = FSP_SUCCESS;
    uint16_t msg_len = RESET_VALUE;
    uint32_t local_timeout;
    char *p_temp_ptr = (char *)p_msg;

    /* Calculate length of message received */
    msg_len = ((uint16_t)(strlen(p_temp_ptr)));
    local_timeout = msg_len * (uint32_t)UINT16_MAX;

    /* Reset callback capture variable */
    g_uart_event = RESET_VALUE;

    /* Writing to terminal */
    fsp_err = R_SCI_UART_Write (&g_catm1_uart_ctrl, p_msg, msg_len);
    if (FSP_SUCCESS != fsp_err)
    {
        APP_DBG_PRINT("\r\n**  R_SCI_UART_Write API Failed  **\r\n");
        return fsp_err;
    }

    /* Check for event transfer complete */
    while ((UART_EVENT_TX_COMPLETE != g_uart_event) && (--local_timeout))
    {
        /* Check if any error event occurred */
        if (UART_ERROR_EVENTS == g_uart_event)
        {
            APP_DBG_PRINT("\r\n**  UART Error Event Received  **\r\n");
            return FSP_ERR_TRANSFER_ABORTED;
        }
    }
    if(RESET_VALUE == local_timeout)
    {
        fsp_err = FSP_ERR_TIMEOUT;
    }
    return fsp_err;
}

/*******************************************************************************************************************//**
 *  @brief       Deinitialize SCI UART module
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void deinit_uart(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Close module */
    fsp_err =  R_SCI_UART_Close (&g_catm1_uart_ctrl);
    if (FSP_SUCCESS != fsp_err)
    {
        APP_DBG_PRINT("\r\n**  R_SCI_UART_Close API failed  ** \r\n");
    }
}


/*****************************************************************************************************************
 *  @brief      UART user callback
 *  @param[in]  p_args
 *  @retval     None
 ****************************************************************************************************************/
void catm1_uart_callback(uart_callback_args_t *p_args);
void catm1_uart_callback(uart_callback_args_t *p_args)
{

    /* Logged the event in global variable */
    g_uart_event = (uint8_t)p_args->event;

    /* Reset g_temp_buffer index if it exceeds than buffer size */
    if(UART_MAX_DATA_LENGTH == g_counter_var)
    {
        g_counter_var = RESET_VALUE;
    }

    if(UART_EVENT_RX_CHAR == p_args->event)
    {
        g_temp_buffer[g_counter_var++] = (uint8_t ) p_args->data;
    }

}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_sci_uart_ep)
 **********************************************************************************************************************/
