/***********************************************************************************************************************
* Copyright (c) 2012 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
 /******************************************************************************
 * File Name    : webCGI.c
 * Version      : 1.00
 * Description  : Common Gateway Interface file handler functions
 ******************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 04.02.2010 1.00 First Release
 ******************************************************************************/

/******************************************************************************
 WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
 OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
 SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
 ******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include <ctype.h>
#include <stdio.h>

#include "net_thread.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"

#include "websys.h"
#include "webCGI.h"

#include "common_init.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/*****************************************************************************
 Function Macros
 ******************************************************************************/

/*****************************************************************************
 Enumerated Types
 ******************************************************************************/

/*****************************************************************************
 Typedefs
 ******************************************************************************/

/*****************************************************************************
 Constant Data
 ******************************************************************************/

/* The check boxes and label links for the LED control */
static const char * const gpszLedCtrl = "<p><input type=\"checkbox\" onclick=\"callFunc"
        "('ledCheckBox', 'led_ctrl.cgi', '0,%d')\" %s/>\r\n"
        "<a href=\"#\"    onclick=\"callFunc"
        "('ledCheckBox', 'led_ctrl.cgi', '0,%d')\">User LED</a></p>\r\n";

static const char * const gpszNotifyString =
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3c.org/TR/1999/REC-html401-19991224/loose.dtd\">\r\n"
                "<HTML lang=en xml:lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\"><HEAD><TITLE>%s | Renesas Electronics</TITLE>\r\n"
                "<META content=\"text/html; charset=utf-8\" http-equiv=Content-Type>\r\n"
                "<META name=Keywords content=\"\">\r\n"
                "<META name=Description content=\"\">\r\n"
                "<STYLE type=text/css>* {\r\n"
                " PADDING-BOTTOM: 0px; LINE-HEIGHT: 1.4; MARGIN: 0px; PADDING-LEFT: 0px; PADDING-RIGHT: 0px; FONT-FAMILY: verdana, \"####\", Arial, sans-serif; COLOR: #333; FONT-SIZE: 12px; PADDING-TOP: 0px\r\n"
                "}\r\n"
                "BODY {\r\n"
                " BACKGROUND: url(images/bg_body.gif) #eeeef8 repeat-x\r\n"
                "}\r\n"
                ".title {\r\n"
                " LINE-HEIGHT: 1.5; COLOR: #c00; MARGIN-LEFT: 60px; FONT-SIZE: 20px; FONT-WEIGHT: bold\r\n"
                "}\r\n"
                ".titlesub {\r\n"
                " LINE-HEIGHT: 1; COLOR: #ccc; MARGIN-LEFT: 60px; FONT-SIZE: 12px\r\n"
                "}\r\n"
                "A {\r\n"
                " COLOR: #277400; TEXT-DECORATION: none\r\n"
                "}\r\n"
                "A:hover {\r\n"
                " TEXT-DECORATION: underline\r\n"
                "}\r\n"
                "</STYLE>\r\n"
                "<META name=GENERATOR content=\"MSHTML 8.00.6001.18999\"></HEAD>\r\n"
                "<BODY>\r\n"
                "<DIV style=\"BORDER-BOTTOM: #ccc 1px solid; TEXT-ALIGN: center; BORDER-LEFT: #ccc 1px solid; MARGIN: 90px auto 0px; WIDTH: 680px; BACKGROUND: #fff; BORDER-TOP: #ccc 1px solid; BORDER-RIGHT: #ccc 1px solid\">\r\n"
                "<DIV style=\"MARGIN-TOP: 50px\">%s</DIV>\r\n"
                "<DIV style=\"MARGIN-TOP: 20px\">\r\n"
                "<P><IMG alt=\"\" src=\"images/icon_arrow_l.gif\" width=11 height=10>&nbsp;<A href=\"index.html\">RSK home page.</A></P></DIV>\r\n"
                "<IMG style=\"MARGIN: 80px 0px 30px\" alt=\"\" src=\"images/logo.gif\" width=189 height=39></DIV>\r\n"
                "<DIV style=\"TEXT-ALIGN: center; MARGIN: 10px auto 0px; WIDTH: 680px; COLOR: #666\">\r\n"
                "<P style=\"FONT-SIZE: 80%%\">&copy; 2010-2011 Renesas Electronics Corporation. All rights reserved.</P></DIV>\r\n"
                "</BODY>\r\n"
                "</HTML>\r\n";

/*****************************************************************************
 Function Prototypes
 ******************************************************************************/

extern int cgiMsExplore (PSESS pSess, PEOFILE pEoFile);
int cgiMsTest (PSESS pSess, PEOFILE pEoFile);

static uint8_t cgiGetBinary (char chAsciiHex);
static _Bool cgiIsReserved (char ch);

/*****************************************************************************
 External Variables
 ******************************************************************************/

extern char gpszLoginsAndPasswords[];

/*****************************************************************************
 Global Variables
 ******************************************************************************/

static const CGITAB gCgiTab;

/*****************************************************************************
 Public Functions
 ******************************************************************************/

/*****************************************************************************
 Function Name: khanCompare
 Description:   Function to perform a slash and case insensitive string compare
 Arguments:     IN  pszS1 - Pointer to the first string
 IN  pszS2 - Pointer to the second string
 IN  stLengthS1 - The length of S1
 Return value:  If the two strings are equivalent the return is zero.
 *****************************************************************************/
int khanCompare (const int8_t *pszS1, const int8_t *pszS2, size_t stLengthS1)
{
    /* Until the end of either sting */
    while (( *pszS1) && ( *pszS2))
    {
        /* Do a case insensitive compare */
        if ((( *pszS1) | 0x20) != (( *pszS2) | 0x20))
        {
            if ((('\\' == ( *pszS1)) || ('/' == ( *pszS1))) && (('\\' == ( *pszS2)) || ('/' == ( *pszS2))))
            {
                /* Only different by the slash is OK */
            }
            else
            {
                break;
            }
        }

        /* Bump the pointers */
        pszS1++;
        pszS2++;
        stLengthS1--;

        /* Both strings must be the same length */
        if (( ! *pszS2) && ( !stLengthS1))
        {
            return 0;
        }
    }
    return 1;
}
/*****************************************************************************
 End of function  khanCompare
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiGetFunction
 Description:   Function to check the table of embedded CGI functions for the
 given file name
 Arguments:     IN  pszCgiFileName - Pointer to the CGI file name
 Return value:  Pointer to the function to handle the request or NULL on error
 *****************************************************************************/
PSVRFN cgiGetFunction (int8_t *pszCgiFileName)
{
    size_t stIndex = gCgiTab.stNumber;
    while (stIndex--)
    {
        if (!pathCompare((const char *) gCgiTab.pCgiList[stIndex].pszCgiFileName, (const char *) pszCgiFileName))
        {
            return gCgiTab.pCgiList[stIndex].pCgiFunction;
        }
    }
    return NULL;
}
/*****************************************************************************
 End of function  cgiGetFunction
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiGetArgument
 Description:   Function to get the arguments passed to the CGI function
 Parameters:    IN  pSess - Pointer to the session data
 IN  bfFirst - Set true for the first argument and false for
 the subsequent arguments.
 Return value:  Pointet to the argument or NULL if not found
 *****************************************************************************/
char *cgiGetArgument (PSESS pSess, _Bool bfFirst)
{
    char *pszResult;
    if (bfFirst)
    {
        size_t stCount = WI_RXBUFSIZE;

        /* First find the part where the first % is */
        pszResult = (char *) pSess->ws_rxbuf;
        while (stCount--)
        {
            if ('-' == *pszResult)
            {
                return strtok(pszResult, "-");
            }
            else
            {
                pszResult++;
            }
        }
        return NULL;
    }
    else
    {
        /* Search for the next one */
        return strtok(NULL, "-");
    }
}
/*****************************************************************************
 End of function  cgiGetArgument
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiSearch
 Description:   Function to find a keyword in the session header
 Parameters:    IN  pSearch - Pointer to the start of the search
 IN  pszEnd - Pointer to the end of memory to search
 IN  pszKeyWord - Pointer to the key word to find
 Return value:  Pointer to the start of the keyword or NULL if not found
 *****************************************************************************/
char *cgiSearch (char *pszSearch, char *pszEnd, char *pszKeyWord)
{
    if ((pszSearch) && (pszEnd) && (pszKeyWord))
    {
        int iIndex;
        while (pszSearch < pszEnd)
        {
            /* Check to see if the strings match */
            for (iIndex = 0; *(pszSearch + iIndex) == *(pszKeyWord + iIndex); iIndex++)
            {
                /* If they match and the next char is the string terminator */
                if ( ! *(pszKeyWord + iIndex + 1))
                {
                    /* The keyword has been found */
                    return pszSearch;
                }
            }
            pszSearch++;
        }
    }
    return NULL;
}
/*****************************************************************************
 End of function  cgiSearch
 *****************************************************************************/

/*****************************************************************************
 Function Name: cgiFindKeyword
 Description:   Function to find a keyword in the session header
 Parameters:    IN  pSess - Pointer to the session data
 IN  pszKeyWord - Pointer to the key word to find
 Return value:  Pointer to the start of the keyword or NULL if not found
 *****************************************************************************/
char *cgiFindKeyword (PSESS pSess, char *pszKeyWord)
{
    return cgiSearch((char *) pSess->ws_rxbuf, (char *) pSess->ws_rxbuf + pSess->ws_rxsize, pszKeyWord);
}
/*****************************************************************************
 End of function  cgiFindKeyword
 *****************************************************************************/

/*****************************************************************************
 Function Name: cgiArgument
 Description:   Function to check the argument
 Parameters:    IN  pszArgument - Pointer to the argument
 IN  pszCommand - Pointer to the command
 Return value:  true if the argment matches the command
 *****************************************************************************/
_Bool cgiArgument (char *pszArgument, char *pszCommand)
{
    size_t stLength = strlen(pszCommand);
    while (stLength--)
    {
        if ( *pszArgument++ != *pszCommand++)
        {
            return false;
        }
    }
    return true;
}
/*****************************************************************************
 End of function  cgiArgument
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiSplitArgument
 Description:   Function to split the argument string at the ','
 Parameters:    IN  pszArgument - Pointer to the string to search & terminate
 Return value:  The pointer to the argument after the ','
 *****************************************************************************/
char *cgiSplitArgument (char *pszArgument)
{
    while ( *pszArgument)
    {
        if (',' == *pszArgument)
        {
            *pszArgument = '\0';
            pszArgument++;
            break;
        }
        pszArgument++;
    }
    return pszArgument;
}
/*****************************************************************************
 End of function  cgiSplitArgument
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiDecodeString
 Description:   Function to decode the string encoded by the browser
 Parameters:    IN/OUT  pszString - Pointer to the string to decode
 Return value:  none
 *****************************************************************************/
void cgiDecodeString (char *pszString)
{
    char *pszDest = pszString;
    while ( *pszString)
    {
        /* Check for the encoder character */
        if ('%' == *pszString)
        {
            int iValue = 0;
            int iDigitCount = 2;
            pszString++;

            /* Decode the hex value */
            while (iDigitCount--)
            {
                iValue = (iValue * 16) + cgiGetBinary( *pszString++);
            }

            /* Save to destination */
            *pszDest++ = (char) iValue;
        }
        else
        {
            /* Save to destination */
            *pszDest++ = *pszString++;
        }
    }
    *pszDest = 0;
}
/*****************************************************************************
 End of function  cgiDecodeString
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiEncodeString
 Description:   Function to encode a string to remove any characters that are
 not safe in a URL
 Parameters:    OUT pszDest - Pointer to the destination string
 IN  stLength - The length of the destination string
 IN  pszSrc - Pointer to the string to encode
 Return value:  none
 *****************************************************************************/
void cgiEncodeString (char *pszDest, size_t stLength, char *pszSrc)
{
    char *pszEnd = pszDest + stLength;
    static const char pszHexAscii[] = "0123456789ABCDEF";
    while ( *pszSrc)
    {
        char ch = *pszSrc++;

        /* If it is not printable or a reserved character */
        if (( !isprint(ch)) || (cgiIsReserved(ch)))
        {
            char pszEncode[4] =
            { '%', 0, 0, 0 };
            char *pReplace = pszEncode;
            pszEncode[1] = pszHexAscii[(0x0F & (ch >> 4))];
            pszEncode[2] = pszHexAscii[(ch & 0x0F)];
            while ( *pReplace)
            {
                if (pszDest >= pszEnd)
                {
                    /* Insufficient buffer */
                    return;
                }
                else
                {
                    *pszDest++ = *pReplace++;
                }
            }
        }
        else
        {
            *pszDest++ = ch;
        }
    }
    *pszDest = '\0';
}
/*****************************************************************************
 End of function  cgiEncodeString
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiReplaceUnprintableChars
 Description:   Function to replace unprintable chars
 Parameters:    IN/OUT pszString - Pointer to the string to search
 IN  chReplace - The character to replace them with
 Return value:  none
 *****************************************************************************/
void cgiReplaceUnprintableChars (char *pszString, char chReplace)
{
    while ( *pszString)
    {
        if ( !isprint( *pszString))
        {
            *pszString = chReplace;
        }
        pszString++;
    }
}
/*****************************************************************************
 End of function  cgiReplaceUnprintableChars
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiFileSize
 Description:   Function to format the file size 39.124k, 987.323M
 Parameters:    OUT pszDest - Pointer to the destination string
 IN  stFileSize - The size of the file
 Return value:  none
 *****************************************************************************/
void cgiFileSize (char *pszDest, float fFileSize)
{
    static char * const pszEngChar = "BkMGTPEZY";
    char *pszEngMult = pszEngChar;
    char pszTemp[32];
    while (fFileSize >= 1024.0f)
    {
        fFileSize /= 1024.0f;
        pszEngMult++;
    }
    sprintf(pszTemp, "%.3f%c", fFileSize, *pszEngMult);
    sprintf(pszDest, "%*s", 12, pszTemp);
}
/*****************************************************************************
 End of function  cgiFileSize
 ******************************************************************************/

/*****************************************************************************
 Private Functions
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiGetBinary
 Description:   Function to turn an ASCII hex character into a binary number
 Parameters:    IN  chAsciiHex - The ASCII character
 Return value:  The binary value
 *****************************************************************************/
static uint8_t cgiGetBinary (char chAsciiHex)
{
    uint8_t byHex = 0;
    if (chAsciiHex >= '0' && chAsciiHex <= '9')
    {
        byHex = (uint8_t) (chAsciiHex - '0');
    }
    else if (chAsciiHex >= 'A' && chAsciiHex <= 'F')
    {
        byHex = (uint8_t) ((chAsciiHex - 'A') + 10);
    }
    return byHex;
}
/*****************************************************************************
 End of function  cgiGetBinary
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiIsReserved
 Description:   Function check to see if the character is valid for a URL
 Parameters:    IN  ch - The character to check
 Return value:  true if the character is reserved
 *****************************************************************************/
static _Bool cgiIsReserved (char ch)
{
    static char * const pszReserved = " -!{}|\\\"^~[]`<>%$&+,:;=?@";
    char *pszCheck = pszReserved;
    while ( *pszCheck)
    {
        if ( *pszCheck++ == ch)
        {
            return true;
        }
    }
    return false;
}
/*****************************************************************************
 End of function  cgiIsReserved
 ******************************************************************************/

static HeapStats_t system_status_btm;
static size_t prev_mem = 0;

/*****************************************************************************
 Function Name: cgiGetTime
 Description:   Function to format the current time and date for the
 time.cgi file
 Arguments:     IN/OUT pSess - Pointer to the session data
 IN/OUT pEoFile - Pointer to the embedded file object
 Return value:  0 for success or error code
 *****************************************************************************/
static int cgiGetTime (PSESS pSess, PEOFILE pEoFile)
{
    (void) pEoFile;

    vPortGetHeapStats( &system_status_btm );
    prev_mem = system_status_btm.xAvailableHeapSpaceInBytes;

    wi_printf(pSess,
            "</div><div id=\"realTimeClock\"><p class=\"boxTitle pb01\">Device ID</p>" \
            "<p style=\"margin-left: 44px;\">20057b48 - 57303132<br> 99ed4e36 - 4e4b277d</right></p>" \
            "<br><p class=\"boxTitle pb01\">MCU Temperature (F): %d.%d</p><p class=\"boxTitle pb01\">" \
            "MCU Temperature (C): %d.%d</p><p class=\"boxTitle pb02\">Blue LED Attributes " \
            "</p><p class=\"boxTitle pb02\">Frequency (Hz): %d" \
            "</p><p class=\"boxTitle pb03\">Intensity (%%): %d</p><br>",
            g_board_status.temperature_f.whole_number, g_board_status.temperature_f.mantissa,
            g_board_status.temperature_c.whole_number, g_board_status.temperature_c.mantissa,
            g_pwm_rates_data[g_board_status.led_frequency],
            g_pwm_dcs_data[g_board_status.led_intensity]
            );

    return 0;
}
/*****************************************************************************
 End of function  cgiGetTime
 ******************************************************************************/



/******************************************************************************
 Function Name: cgiSW1Ctrl
 Description:   Function to respond to virtual button 1 press from the Web Server
 Arguments:     IN/OUT pSess - Pointer to the session data
 IN/OUT pEoFile - Pointer to the embedded file object
 Return value:  0 for success or error code
 ******************************************************************************/
static int cgiSW1Ctrl (PSESS pSess, PEOFILE pEoFile)
{
    (void) pSess;
    (void) pEoFile;

    g_board_status.led_intensity = (uint16_t)((g_board_status.led_intensity + 1)%3);
    xEventGroupSetBits(g_update_console_event, STATUS_UPDATE_INTENSE_INFO);
    return (0);
}
/******************************************************************************
 End of function  cgiSW1Ctrl
 ******************************************************************************/



/******************************************************************************
 Function Name: cgiSW2Ctrl
 Description:   Function to respond to virtual button 1 press from the Web Server
 Arguments:     IN/OUT pSess - Pointer to the session data
 IN/OUT pEoFile - Pointer to the embedded file object
 Return value:  0 for success or error code
 ******************************************************************************/
static int cgiSW2Ctrl (PSESS pSess, PEOFILE pEoFile)
{
    (void) pSess;
    (void) pEoFile;

    g_board_status.led_frequency = (uint16_t)((g_board_status.led_frequency + 1)%3);
    xEventGroupSetBits(g_update_console_event, STATUS_UPDATE_FREQ_INFO);
    return (0);
}
/******************************************************************************
 End of function  cgiSW2Ctrl
 ******************************************************************************/



/******************************************************************************
 Function Name: cgiLedCtrl
 Description:   Function to control the LEDs
 Arguments:     IN/OUT pSess - Pointer to the session data
 IN/OUT pEoFile - Pointer to the embedded file object
 Return value:  0 for success or error code
 ******************************************************************************/
static int cgiLedCtrl (PSESS pSess, PEOFILE pEoFile)
{
    static char *pp_checked[2] =
    { "", "checked=\"checked\"" };
    char *p_szled;

    uint32_t led_state = 0u;
//    uint32_t led0_ident = LED0;
#if 0
    p_szled = cgiGetArgument(pSess, true);

    /* reference pEoFile argument */
    (void) pEoFile;
    if (p_szled)
    {
        /* Set the appropriate LED state */
        switch ( *p_szled)
        {
            case '0':
            {
                /* cast pointer to LED identifier (uint32_t *) to void * to fit function call */
//                direct_control(g_led_handle, CTL_SET_LED_TOGGLE, (void *) &led0_ident);
                break;
            }
            default:
            {
                /* nothing */
                break;
            }
        }
    }

    /* ensure led state is updated */
    direct_read(g_led_handle, (uint8_t *) &led_state, (sizeof(led_state)));

    /* Format the check boxes and links */
    wi_printf(pSess, gpszLedCtrl,

    /* The ? : statement inverts the setting and makes sure that
     the argument is an integer */
    (led_state) ? 0 : 1, pp_checked[(led_state) ? 1 : 0], (led_state) ? 0 : 1);
#endif

    return (0);
}
/******************************************************************************
 End of function  cgiLedCtrl
 ******************************************************************************/

/******************************************************************************
 Function Name: cgiSetTime
 Description:   Function to set the time
 Arguments:     IN/OUT pSess - Pointer to the session data
 IN/OUT pEoFile - Pointer to the embedded file object
 Return value:  0 for success or error code
 ******************************************************************************/
static int cgiSetTime (PSESS pSess, PEOFILE pEoFile)
{
#if 0 // Knock out time/date view support

    UNUSED_PARAM(pEoFile);

    if (pSess->ws_formlist)
    {
        int iHour = -1, iMinute = -1;
        sscanf(pSess->ws_formlist->pairs[0].value, "%d", &iHour);
        sscanf(pSess->ws_formlist->pairs[1].value, "%d", &iMinute);
        if ((iHour >= 0) && (iHour <= 23) && (iMinute >= 0) && (iMinute <= 59))
        {
            DATE Date;
            if (control(g_rtc_handle, CTL_GET_DATE, &Date) == 0)
            {
                Date.Field.Hour = (unsigned) (iHour & 0x1f);
                Date.Field.Minute = (unsigned) (iMinute & 0x3f);
                Date.Field.Second = 0;
                control(g_rtc_handle, CTL_SET_DATE, &Date);
            }
        }
    }
#endif

    /* Could forward to a "set" page */
    return wi_redirect(pSess, "admin.html");
}
/******************************************************************************
 End of function  cgiSetTime
 ******************************************************************************/

/******************************************************************************
 Function Name: cgiSetDate
 Description:   Function to set the date
 Arguments:     IN/OUT pSess - Pointer to the session data
 IN/OUT pEoFile - Pointer to the embedded file object
 Return value:  0 for success or error code
 ******************************************************************************/
static int cgiSetDate (PSESS pSess, PEOFILE pEoFile)
{
    (void) pEoFile;

#if 0
    if (pSess->ws_formlist)
    {
        DATE Date;
        int iDay = -1, iMonth = -1, iYear = -1;
        sscanf(pSess->ws_formlist->pairs[0].value, "%d", &iDay);
        sscanf(pSess->ws_formlist->pairs[1].value, "%d", &iMonth);
        sscanf(pSess->ws_formlist->pairs[2].value, "%d", &iYear);
        if (control(g_rtc_handle, CTL_GET_DATE, &Date) == 0)
        {
            Date.Field.Day = (unsigned) (iDay & 0x1f);
            Date.Field.Month = (unsigned) (iMonth & 0xf);
            Date.Field.Year = (unsigned) iYear;
            control(g_rtc_handle, CTL_SET_DATE, &Date);
        }
    }

#endif
    return wi_redirect(pSess, "admin.html");
}
/******************************************************************************
 End of function  cgiSetDate
 ******************************************************************************/

/******************************************************************************
 Function Name: cgiSetUserName
 Description:   Function to set the username
 Arguments:     IN/OUT pSess - Pointer to the session data
 IN/OUT pEoFile - Pointer to the embedded file object
 Return value:  0 for success or error code
 ******************************************************************************/
static int cgiSetUserName (PSESS pSess, PEOFILE pEoFile)
{
    (void) pEoFile;

#if 0 // Knock out admin view support

    if (pSess->ws_formlist)
    {
        int8_t *pTerm;
        int8_t pszUserName[21];
        int8_t pszNewUserName[21];
        int8_t *ppArguments[3];
        size_t stLength = strlen(pSess->ws_formlist->pairs[0].value);
        size_t stupdLen = stLength > 14 ? 14 : stLength;

        ppArguments[0] = (int8_t *) "user";
        ppArguments[1] = pszUserName;
        ppArguments[2] = (int8_t *) pSess->ws_formlist->pairs[0].value;

        do
        {
            if (0x20 == pSess->ws_formlist->pairs[0].value[stupdLen])
            {
                pSess->ws_formlist->pairs[0].value[stupdLen] = 0;
                stLength = stupdLen;
            }
        } while (stupdLen-- > 0);

        /* Only short user names and passwords are acceptable */
        if (stLength > 14)
        {
            strcpy((char *) pszNewUserName, pSess->ws_formlist->pairs[0].value);
            pszNewUserName[stLength] = '\0';
            sprintf((char *) pszUserName, "Sorry the user name \"%s\" is too long.", pszNewUserName);
            wi_printf(pSess, gpszNotifyString, "User Name Set", pszUserName);
        }
        else if (stLength)
        {
            /* Copy the first user name on the list */
            strcpy((char *) pszUserName, gpszLoginsAndPasswords);
            pTerm = memchr(pszUserName, ':', sizeof(pszUserName));
            if (pTerm)
            {
                *pTerm = '\0';
            }

            /* Just call the console command */
            cmdUserName(3, ppArguments, NULL);

            /* Show a notification page */
            sprintf((char *) pszUserName, "The user name has been set to \"%s\".", pSess->ws_formlist->pairs[0].value);
            wi_printf(pSess, gpszNotifyString, "User Name Set", pszUserName);
        }
        else
        {
            wi_printf(pSess, gpszNotifyString, "User Name Set", "User name required.");
        }
    }
#endif // Knock out admin view support

    return 0;
}
/******************************************************************************
 End of function  cgiSetUserName
 ******************************************************************************/

/******************************************************************************
 Function Name: cgiSetPassword
 Description:   Function to set the password
 Arguments:     IN/OUT pSess - Pointer to the session data
 IN/OUT pEoFile - Pointer to the embedded file object
 Return value:  0 for success or error code
 ******************************************************************************/
static int cgiSetPassword (PSESS pSess, PEOFILE pEoFile)
{
    size_t stLength = strlen(pSess->ws_formlist->pairs[0].value);
    size_t stupdLen = stLength > 14 ? 14 : stLength;
    (void) pEoFile;
#if 0 // Knock out admin view support
    do
    {
        if (0x20 == pSess->ws_formlist->pairs[0].value[stupdLen])
        {
            pSess->ws_formlist->pairs[0].value[stupdLen] = 0;
            stLength = stupdLen;
        }
    } while (stupdLen-- > 0);

    stLength = strlen(pSess->ws_formlist->pairs[1].value);
    stupdLen = stLength > 14 ? 14 : stLength;

    do
    {
        if (0x20 == pSess->ws_formlist->pairs[1].value[stupdLen])
        {
            pSess->ws_formlist->pairs[1].value[stupdLen] = 0;
            stLength = stupdLen;
        }
    } while (stupdLen-- > 0);

    if (pSess->ws_formlist)
    {
        /* Both the passwords must match */
        if ( !strcmp(pSess->ws_formlist->pairs[0].value, pSess->ws_formlist->pairs[1].value))
        {
            int8_t pszUserName[32];

            /* Only short user names and passwords are acceptable */
            if (strlen(pSess->ws_formlist->pairs[0].value) > 14)
            {
                /* This is for form demonstration purposes */
                sprintf((char *) pszUserName, "Sorry the password \"%s\" is too long.", pSess->ws_formlist->pairs[0].value);
                wi_printf(pSess, gpszNotifyString, "Password Set", pszUserName);
            }
            else
            {
                int8_t *pTemp;
                int8_t *ppArguments[4];
                int8_t pszPassword[32];

                ppArguments[0] = (int8_t *) "user";
                ppArguments[1] = pszUserName;
                ppArguments[3] = (int8_t *) pSess->ws_formlist->pairs[0].value;

                /* Copy the first user name on the list */
                strcpy((char *) pszUserName, gpszLoginsAndPasswords);
                pTemp = memchr(pszUserName, ':', sizeof(pszUserName));
                if (pTemp)
                {
                    *pTemp++ = '\0';
                    strcpy((char *) pszPassword, (const char *) pTemp);
                    *pTemp = '\0';
                    ppArguments[2] = pszPassword;
                    pTemp = pszPassword;
                    while ( *pTemp)
                    {
                        *pTemp = (int8_t) ( *pTemp ^ 129);
                        pTemp++;
                    }

                    /* Call the command handler */
                    cmdChangePassword(4, ppArguments, NULL);

                    /* Format the message */
                    sprintf((char *) pszUserName, "The password has been set to \"%s\".", pSess->ws_formlist->pairs[0].value);
                    wi_printf(pSess, gpszNotifyString, "Password Set", pszUserName);
                }
                else
                {
                    wi_printf(pSess, gpszNotifyString, "Password Set", "Failed to set password.");
                }
            }
        }
        else
        {
            wi_printf(pSess, gpszNotifyString, "Password Set", "Please enter the password in both fields.");
        }
    }

#endif
    return 0;
}
/******************************************************************************
 End of function  cgiSetPassword
 ******************************************************************************/

/*****************************************************************************
 Constant Data
 ******************************************************************************/

static const CGIFNASS gpCgiFnAss[] =
{
    {(int8_t *) "get_time.cgi", cgiGetTime},
    {(int8_t *) "led_ctrl.cgi", cgiLedCtrl},
    {(int8_t *) "sw1_ctrl.cgi", cgiSW1Ctrl},
    {(int8_t *) "sw2_ctrl.cgi", cgiSW2Ctrl},

//	{(int8_t *) "ms_explore.cgi", cgiMsExplore},
//	{(int8_t *) "ms_test.cgi", cgiMsTest},
//	{(int8_t *) "set_time.cgi", cgiSetTime},
//	{(int8_t *) "set_date.cgi", cgiSetDate},
//	{(int8_t *) "set_user.cgi", cgiSetUserName},
//	{(int8_t *) "set_password.cgi", cgiSetPassword}

/* TODO: Add more CGI file names and handling functions */
};

static const CGITAB gCgiTab =
{ (PCGIFNASS)gpCgiFnAss, sizeof(gpCgiFnAss) / sizeof(CGIFNASS) };

/******************************************************************************
 End  Of File
 ******************************************************************************/
