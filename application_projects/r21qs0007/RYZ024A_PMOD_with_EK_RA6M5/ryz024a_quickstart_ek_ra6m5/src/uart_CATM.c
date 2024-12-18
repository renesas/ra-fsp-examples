/***********************************************************************************************************************
 * File Name    : uart_CATM.c
 * Description  : Contains UART functions for interface to CATM module
 **********************************************************************************************************************/
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

#include "ONE_common.h"
#include "uart_CATM.h"

#include "r_sci_uart.h"
#include "r_uart_api.h"
FSP_HEADER

/** UART on SCI Instance. */
extern const uart_instance_t g_uart;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart_ctrl;
extern const uart_cfg_t g_uart_cfg;
extern const sci_uart_extended_cfg_t g_uart_cfg_extend;

/** Defines for detailed output for diagnostics */
#define EXTRA_DIAG_OUTPUTS
#define SHOW_RESPONSES

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

/* Flag to check whether data is received or not */
static volatile uint8_t g_data_received_flag = false;

/* Flag for user callback */
static volatile uint8_t g_uart_event = RESET_VALUE;

static void removeLeadingCrLf(uint8_t *p2Str);
static void removeLeadingCrLf(uint8_t *p2Str)
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

// Return the first string found (terminated by null, CR, or LF)
static bool getFirstString(uint8_t *p2Dest, uint8_t *p2Src);
static bool getFirstString(uint8_t *p2Dest, uint8_t *p2Src)
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

static uint8_t getResponse(char *p2Result);
static uint8_t getResponse(char *p2Result)     // return the length received (0 if none)
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
            ONE_DelayMsec(10);  // wait a few more to catch the whole string

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
                ONE_DelayMsec(10);  // wait a few more to see if there's more data
            }
        }
        ONE_DelayMsec(1);
        if (loopCounter++ > 1000)   // 1 second timeout
        {
            ONE_PRINT_AT("\r\n *** loopCounter Timeout, g_counter_var=%d, len=%d", g_counter_var, input_length);
            break;
        }
    }

    // Then copy to the passed string buffer
    if (input_length > 0)
    {
        #ifdef EXTRA_DIAG_OUTPUTS
            ONE_PRINT("\r\nString returned=%s", g_temp_buffer);
        #endif
        strcpy (p2Result, (char *)&g_temp_buffer);  // temp buffer should always have a null terminator
        memset(g_temp_buffer, RESET_VALUE, UART_MAX_DATA_LENGTH);
        g_counter_var = RESET_VALUE;
        g_data_received_flag  = false;
    }

    return (input_length);
}

static bool isInStr(char *p2Str, char *p2LookingFor);
static bool isInStr(char *p2Str, char *p2LookingFor)
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

static bool isGetExpectedResponse(char *p2ExpectedStr);
static bool isGetExpectedResponse(char *p2ExpectedStr)
{
    bool    result = false;
    uint8_t rcvdLength = 0;
    char    tempBuffer[UART_MAX_DATA_LENGTH] = {RESET_VALUE}; // use an extra character to assure a null-terminator

    if ((rcvdLength = getResponse(tempBuffer)) > 0)
    {
        // Then compare to the passed string
        result = isInStr(tempBuffer, p2ExpectedStr);
        sprintf (g_print_buf, "\r\n\nrcvdLength=%d, expected=%s, rcvd=%s\r\n", rcvdLength, p2ExpectedStr, tempBuffer);
        ONE_PRINT_AT (g_print_buf);
    }
    else
    {
        ONE_PRINT_AT("\r\nNo valid string returned!!!");
    }

    if (result == false)
    {
         ONE_DelayMsec(500);  // wait more time before sending next attempt 
    }
    return (result);
}

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
        ONE_PRINT_AT (g_print_buf);
    }
    else
    {
        ONE_PRINT_AT("\r\nNo valid string returned!!!");
    }

    if ( false == result )
    {
        ONE_DelayMsec(2000);  // was 5 secs: wait more time before sending next attempt
    }
    return result;
}

// parseString
// Given an input string, search for a string inside delimiter characters
static uint8_t parseString(uint8_t *p2Results, uint8_t *p2InputStr, uint8_t delimiter, uint8_t targetIndex);
static uint8_t parseString(uint8_t *p2Results, uint8_t *p2InputStr, uint8_t delimiter, uint8_t targetIndex)
{
    uint8_t result = 0;
    uint8_t ii=0;
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
 * @brief       Assert reset signal on PMOD and Initialize Communication over UART.
 * @param[in]   None
 * @retval      FSP_SUCCESS         Upon successful open and start of comms
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 ***********************************************************************************************************************/
fsp_err_t comms_initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* RYZ024A PMOD2 RESET (active low) : assert */
    fsp_err = R_IOPORT_PinWrite(&g_ioport_ctrl, PMOD2_RST, BSP_IO_LEVEL_LOW);
    ONE_DelayMsec(5);
    /* PMOD2 RESET : de-assert */
    fsp_err = R_IOPORT_PinWrite(&g_ioport_ctrl, PMOD2_RST, BSP_IO_LEVEL_HIGH);
    if (FSP_SUCCESS != fsp_err)
    {
        ONE_ERR_PRINT ("\r\n**  R_IOPORT_PinWrite API failed  **\r\n");
        return fsp_err;
    }

    /* Initialize UART channel with baud rate set in FSP configurator @ 921600 */
    fsp_err = R_SCI_UART_Open (&g_uart_ctrl, &g_uart_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        ONE_ERR_PRINT ("\r\n**  R_SCI_UART_Open API failed  **\r\n");
    }
    return fsp_err;
}


fsp_err_t CATM_query(char *p2cmdStr, char *p2LookForStr, char delim, char *p2resultStr);
fsp_err_t CATM_query(char *p2cmdStr, char *p2LookForStr, char delim, char *p2resultStr)
{
    fsp_err_t status = FSP_SUCCESS;
    uint8_t rcvdLength = 0;
    char    tempBuffer[UART_MAX_DATA_LENGTH] = {RESET_VALUE}; // use an extra character to assure a null-terminator
    uint8_t retries = 5;

    sprintf (g_print_buf, "\r\n-> %s", p2cmdStr);
    ONE_PRINT_AT (g_print_buf);
    sprintf(tempBuffer, "%s\r\n", p2cmdStr);
    status = uart_print_user_msg((uint8_t *)tempBuffer);
    retries = 0;
    while (true)
    {
        memset(tempBuffer, RESET_VALUE, UART_MAX_DATA_LENGTH);
        if ((rcvdLength = getResponse(tempBuffer)) > 0)
        {
            #ifdef SHOW_RESPONSES
                sprintf (g_print_buf, "\r\n<- %s", tempBuffer);
                ONE_PRINT_AT (g_print_buf);
            #endif

            if (isInStr(tempBuffer, p2LookForStr))
            {
                // now parse the data...
                if (delim == 'X')
                {
                    // Note: in this case, no return string is necessary
                    break;
                }
                else if (parseString((uint8_t *)p2resultStr, (uint8_t *)tempBuffer, (uint8_t)delim, 1) == 1)
                {
                    #ifdef SHOW_RESPONSES
                        sprintf (g_print_buf, "\r\n<- %s found", p2LookForStr);
                        ONE_PRINT_AT (g_print_buf);
                    #endif
                    break;
                }
                else
                {
                    sprintf (g_print_buf, "\r\n<- %s failed, tempBuffer=%s", p2cmdStr, tempBuffer);
                    ONE_PRINT_AT (g_print_buf);
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
            ONE_DelayMsec(1000);
            ONE_PRINT_AT ("\r\n___nothing yet");
        }
    }

    return status;
}

/********************************************************************************************************************
* @brief      Validate SIM Card activation by Server Ping
* @param[in]  None
* @retval     FSP_SUCCESS        Upon SIM card validation success
* @retval     FSP_ERR_TIMEOUT    Upon SIM card validatation time-out
* @retval     Any other error code other than FSP_SUCCESS,   Unsuccessful SIM card validation
********************************************************************************************************************/
fsp_err_t CATM_Ping(void)
{
    fsp_err_t status = FSP_SUCCESS;
    uint8_t retries = 0;
    bool bPingTimeout;

    // Set Ping Echo Request Mode to Synchronous
    status = CATM_query("AT+PINGCFG=0", "OK", 'X', NULL);
    ONE_DelayMsec(1000);

    // Send AT command to measure Network Received Power (Signal Received Quality)
    status = CATM_query("AT+CESQ", "OK", 'X', NULL);
    if (FSP_SUCCESS != status)
    {
        ONE_PRINT_AT ("\r\n ** Network Received Power FAILED ** \r\n");
    }
    ONE_DelayMsec(1000);

    while (true)
    {
        ONE_PRINT_AT("\r\n-> AT+PING=\"9.9.9.9\",1,32,15  \r\n");
        // Send AT command to RYZ024A to PING Server
        status = uart_print_user_msg((uint8_t *)"AT+PING=\"9.9.9.9\",1,32,15\r\n");
        if (FSP_SUCCESS != status)
        {
            ONE_PRINT_AT ("\r\n ** PING FAILED ** \r\n");
            ONE_DelayMsec(3000);
        }
        else if (isGetExpectedResponsePing(&bPingTimeout) == true)
        {
            // PING timeout occurred : SIM maybe Inactive or NETWORK can not be reached
            if (bPingTimeout == true)
            {
                // Server did not respond:  RYZ024A responded with "+PING: 1,<ip addr>,-1,-1"
                // where Time = -1, TTL = -1
                ONE_PRINT_AT ("\r\n<- +PING Timeout : SIM Not Active or Network can not be reached\r\n");
                status = FSP_ERR_TIMEOUT;
            }
            else
            {
                // Server Responded with "+PING: 1,<ip addr>,Time, TTL", where Time > 0, and TTL > 0
                ONE_PRINT_AT ("\r\n<- +PING returned \r\n");
                status = FSP_SUCCESS;
            }
            break;
        }
        else
        {
            ONE_PRINT_AT ("\r\n ** +PING Response not returned ** \r\n");
            status = FSP_ERR_TIMEOUT;
            CATM_query("AT+PINGCFG=2", "OK", 'X', NULL);   // stop pinging
            ONE_DelayMsec(1000);
        }

        if (retries++ >= 3)
        {
            // max retries exceeded
            status = FSP_ERR_TIMEOUT;
            CATM_query("AT+PINGCFG=2", "OK", 'X', NULL);   // stop pinging
            ONE_PRINT_AT ("\r\n ** Retries Exceeded : +PING Response not returned ** \r\n");
            ONE_PRINT_AT ("\r\n ** SIM Not Active or Network cannot be reached ** \r\n");
            ONE_PRINT_AT ("\r\n ** Check SIM card or Antenna Attached ** \r\n");
            break;
        }
    }
    return (status);
}



/********************************************************************************************************************
* @brief      Initialize the CAT-M module
* @param[in]  None
* @retval     FSP_SUCCESS        Upon initialize success
* @retval     FSP_ERR_TIMEOUT    Upon initialize time-out
* @retval     Any other error code other than FSP_SUCCESS,   Unsuccessful initialize
********************************************************************************************************************/
fsp_err_t CATM_initialize(void)
{
    fsp_err_t status = FSP_SUCCESS;
    uint8_t rcvdLength = 0;
    char    tempBuffer[UART_MAX_DATA_LENGTH] = {RESET_VALUE}; // use an extra character to assure a null-terminator
    uint8_t retries = 0;

    /***************************************************************************************************
      * Send AT commands and look for the following responses:
      * Command              Response        Notes
      * -------              --------        -----
      * AT                   OK              Confirm the module connection is operational
      *
      * Configure RYZ024A                    UART Comms is No Flow Control and Low Power Mode support
      *
      * AT^RESET             OK              Reset the module to ensure CATM module is in a known state
      *                      +SYSSTART       Look for module start-up message
      * AT+SQNAUTOCONNECT=1  OK              Set module to boot with radio on
      * AT+CFUN=1            OK              Turn the radio on (full operation mode)
      * AT+CGSN              352567620031460 Read the IMEI of the module
      * AT+SQNCCID           +SQNCCID:   "8931080620054164161",""
      *                                      Read the ICCID of the SIM Card
     *****************************************************************************************/

    ONE_PRINT_AT("\r\nInit the CAT-M module");
    // wait for Module to start up from reset in comms_initialize() with AT comms online to accept commands.
    ONE_DelayMsec(1000);

    for (int attempts=0; attempts<10; attempts++)
    {
    	// Host MCU must use AT polling by sending AT+CFUN? and wait for OK response
    	// before sending any other AT commands
    	status = uart_print_user_msg((uint8_t *)"AT+CFUN?\r\n");
        if (FSP_SUCCESS != status)
        {
            ONE_PRINT_AT ("\r\n ** UART TX FAILED ** \r\n");
            return(status);
        }
        else if (isGetExpectedResponse("OK") == true) {
            ONE_PRINT_AT ("\r\n<- OK");
            break;
        }
        else
        {
            ONE_PRINT_AT ("\r\n ** Response not returned **");
            status = FSP_ERR_TIMEOUT;
        }
        ONE_DelayMsec(500);
    }
    if (FSP_SUCCESS != status)
    {
        // ONE_DelayMsec(1000);
        return (status);
    }

    retries = 0;
    while (true)
    {
    	ONE_PRINT_AT("\r\nAT^RESET");
        status = CATM_query("AT^RESET", "OK", 'X', NULL);
        if (FSP_SUCCESS != status)
        {
            ONE_DelayMsec(10000);
            return (status);
        }
        // After reset, it sends a SHUTDOWN until it starts back up, then it sends a SYSSTART
        ONE_DelayMsec(3000);
        if (isGetExpectedResponse("+SYSSTART") == true)
        {
            ONE_PRINT_AT ("\r\n<- +SYSSTART");
            break;
        }
        else
        {
            ONE_PRINT_AT ("\r\n ** Response not returned **");
            status = FSP_ERR_TIMEOUT;
        }

        if (retries++ >= 4)
        {
            status = FSP_ERR_TIMEOUT;
            return (status);
        }
        ONE_DelayMsec(1000);
    }

    // Set to Network Auto-connect after Power UP
    status = CATM_query("AT+SQNAUTOCONNECT=1", "OK", 'X', NULL);
    if (FSP_SUCCESS != status)
    {
        ONE_DelayMsec(10000);
        return (status);
    }

    // Full Functional Mode
    status = CATM_query("AT+CFUN=1", "OK", 'X', NULL);
    if (FSP_SUCCESS != status)
    {
        ONE_DelayMsec(10000);
        return (status);
    }

    // Read the IMEI of the CATM module
    ONE_PRINT_AT("\r\n-> AT+CGSN");
    status = uart_print_user_msg((uint8_t *)"AT+CGSN\r\n");
    if (FSP_SUCCESS != status)
    {
        ONE_PRINT_AT ("\r\n ** UART TX FAILED ** \r\n");
        return(status);
    }
    else
    {
        // look for the IMEI response
        // could be:  "OK",
        retries = 0;
        while (true)
        {
            memset(tempBuffer, RESET_VALUE, UART_MAX_DATA_LENGTH);
            if ((rcvdLength = getResponse(tempBuffer)) > 0)
            {
                #ifdef SHOW_RESPONSES
                    sprintf (g_print_buf, "\r\n<- %s", tempBuffer);
                    ONE_PRINT_AT (g_print_buf);
                #endif

                if (isInStr(tempBuffer, "OK"))
                {
                    // now parse the data, looking for the 15 character number
                    if (getFirstString((uint8_t *)g_strIMEI, (uint8_t *)tempBuffer) == true)
                    {
                        // RYZ024A IMEI found
                        sprintf (g_print_buf, "\r\n<- IMEI=%s", g_strIMEI);
                        ONE_PRINT_AT (g_print_buf);
                        break;
                    }
                    else
                    {
                        // RYZ024A IMEI not found
                        sprintf (g_print_buf, "\r\n<- IMEI failed, tempBuffer=%s", tempBuffer);
                        ONE_PRINT_AT (g_print_buf);
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
                ONE_PRINT_AT ("\r\n...nothing yet");
                ONE_DelayMsec(1000);
            }
        }
    }
    if (FSP_SUCCESS != status)
    {
        // report error condition
        return(status);
    }

    // Get the SIM Card ICCID
    ONE_PRINT_AT("\r\n-> AT+SQNCCID");
    status = uart_print_user_msg((uint8_t *)"AT+SQNCCID\r\n");
    if (FSP_SUCCESS != status)
    {
        ONE_PRINT_AT ("\r\n ** UART TX FAILED ** \r\n");
        return(status);
    }
    else
    {
        // look for the SIM Card ICCID response
        // could be:  "OK",
        retries = 0;
        while (true)
        {
            memset(tempBuffer, RESET_VALUE, UART_MAX_DATA_LENGTH);
            if ((rcvdLength = getResponse(tempBuffer)) > 0)
            {
                #ifdef SHOW_RESPONSES
                    sprintf (g_print_buf, "\r\n<- %s", tempBuffer);
                    ONE_PRINT_AT (g_print_buf);
                #endif

                if (isInStr(tempBuffer, "OK"))
                {
                    if (parseString((uint8_t *)g_strICCID, (uint8_t *)tempBuffer, '"', 1) == 1)
                    {
                        // SIM Card ICCID found : looking for the 19 character number
                        sprintf (g_print_buf, "\r\n<- ICCID=%s", g_strICCID);
                        ONE_PRINT_AT (g_print_buf);
                        break;
                    }
                    else
                    {
                        // SIM Card ICCID not found
                        sprintf (g_print_buf, "\r\n<- ICCID failed, tempBuffer=%s", tempBuffer);
                        ONE_PRINT_AT (g_print_buf);
                        status = FSP_ERR_TIMEOUT;
                    }
                }
            }
            if (retries++ >= 10)    // max retries?
            {
                // max retries exceeded
                status = FSP_ERR_TIMEOUT;
                break;
            }
            else
            {
                ONE_PRINT_AT ("\r\n....nothing yet");
                ONE_DelayMsec(500);  // was
            }
        }
    }

    // Read and display the FW Version LRx.y.z
    status = CATM_query("ATI1", "OK", 'X', NULL);

    // set the APN for Network Provider : must define in /src/uart_CATM.h
    // Set APN for Truphone SIM Card Provider -> iot.truphone.com
    status = uart_print_user_msg((uint8_t *)CELLULAR_CFG_AP_NAME);
    if (FSP_SUCCESS != status)
    {
        ONE_PRINT_AT ("\r\n ** Set APN FAILED ** \r\n");
        return (status);
    }
    // verify the APN has been set
    status = CATM_query("AT+CGDCONT?", "OK", 'X', NULL);
    if (FSP_SUCCESS != status)
    {
        ONE_PRINT_AT ("\r\n ** Verify APN FAILED ** \r\n");
        return (status);
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
fsp_err_t uart_print_user_msg(uint8_t *p_msg)
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
    fsp_err = R_SCI_UART_Write (&g_uart_ctrl, p_msg, msg_len);
    if (FSP_SUCCESS != fsp_err)
    {
        ONE_ERR_PRINT ("\r\n**  R_SCI_UART_Write API Failed  **\r\n");
        return fsp_err;
    }

    /* Check for event transfer complete */
    while ((UART_EVENT_TX_COMPLETE != g_uart_event) && (--local_timeout))
    {
        /* Check if any error event occurred */
        if (UART_ERROR_EVENTS == g_uart_event)
        {
            ONE_ERR_PRINT ("\r\n**  UART Error Event Received  **\r\n");
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
    fsp_err =  R_SCI_UART_Close (&g_uart_ctrl);
    if (FSP_SUCCESS != fsp_err)
    {
        ONE_ERR_PRINT ("\r\n**  R_SCI_UART_Close API failed  ** \r\n");
    }
}


/*****************************************************************************************************************
 *  @brief      UART user callback
 *  @param[in]  p_args
 *  @retval     None
 ****************************************************************************************************************/
void user_uart_callback(uart_callback_args_t *p_args);
void user_uart_callback(uart_callback_args_t *p_args)
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
