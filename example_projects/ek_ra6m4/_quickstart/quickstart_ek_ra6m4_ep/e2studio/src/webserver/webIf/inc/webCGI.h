/***********************************************************************************************************************
* Copyright (c) 2018 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/******************************************************************************
 * @headerfile     webCGI.h
 * @brief          Common Gateway Interface file handler functions
 * @version        1.00
 * @date           27.06.2018
 * H/W Platform    RZ/A1LU
 *****************************************************************************/
 /*****************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 30.06.2018 1.00 First Release
 *****************************************************************************/
/******************************************************************************
  WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
  OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
  SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
******************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef WEBCGI_H_INCLUDED
#define WEBCGI_H_INCLUDED

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "webio.h"
#include "webfs.h"

/**************************************************************************//**
 * @ingroup R_SW_PKG_93_WEBIF_API
 * @defgroup R_SW_PKG_93_WEBIF_CGI Common Gateway Interface
 * @brief Common Gateway Interface file handler functions
 *
 * @anchor R_SW_PKG_93_WEB_IF_API_SUMMARY
 * @par Summary
 *
 * The Interface allows the user to start and stop the WebIO service. 
 * 
 * @anchor R_SW_PKG_93_WEB_IF_API_INSTANCES
 * @par Known Implementations:
 * This driver is used in the RZA1LU Software Package.
 * @see RENESAS_APPLICATION_SOFTWARE_PACKAGE
 *
 * @see RENESAS_OS_ABSTRACTION  Renesas OS Abstraction interface
 * @{
 *****************************************************************************/
/******************************************************************************
Typedef definitions
******************************************************************************/

/* Define a structure to associate the command string with function */
typedef struct _CGIFNASS
{
    int8_t  *pszCgiFileName;
    PCSVRFN pCgiFunction;
} CGIFNASS,
*PCGIFNASS,
* const PCCGIFNASS;

/* Define a structure to describe a table of CGI handlers */
typedef struct _CGITAB
{
    PCGIFNASS   pCgiList;
    size_t      stNumber;
} CGITAB,
*PCGITAB;

/*****************************************************************************
Function Prototypes
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief         Function to perform a slash and case insensitive string compare
 *     
 * @param[in]     pszS1:      Pointer to the first string
 * @param[in]     pszS2:      Pointer to the second string
 * @param[in]     stLengthS1: The length of S1
 * 
 * @retval        0: If the two strings are equivalent.
 */
extern  int khanCompare(const int8_t *pszS1,
                        const int8_t *pszS2,
                        size_t        stLengthS1);

/**
 * @brief         Function to check the table of embedded CGI functions for the
 *                given file name
 *      
 * @param[in]     pszSsiFileName: Pointer to the CGI file name
 * 
 * @retval        p_func: Pointer to the function to handle the request.
 * @retval        NULL:   On error
 */
extern  PSVRFN cgiGetFunction(int8_t *pszSsiFileName);

/**
 * @brief         Function to get the arguments passed to the CGI function
 *     
 * @param[in]     pSess:   Pointer to the session data
 * @param[in]     bfFirst: Set true for the first argument and false for
 *                         the subsequent arguments.
 * 
 * @retval        p_arg: Pointer to the argument  
 * @retval        NULL:  If not found
 */
extern  char *cgiGetArgument(PSESS pSess, _Bool bfFirst);

/**
 * @brief         Function to check the argument
 *     
 * @param[in]     pszArgument:  Pointer to the argument
 * @param[in]     pszCommand:   Pointer to the command
 * 
 * @retval        TRUE: if the argment matches the command
 */
extern  _Bool cgiArgument(char *pszArgument, char *pszCommand);

/**
 * @brief         Function to split the argument string at the ','
 * 
 * @param[in]     pszArgument: Pointer to the string to search & terminate
 * 
 * @retval        p_arg: The pointer to the argument after the ','
 */
extern char *cgiSplitArgument(char *pszArgument);

/**
 * @brief         Function to decode the string encoded by the browser
 *     
 * @param[in/out] pszString: Pointer to the string to decode
 * 
 * @return  None.
 */
extern  void cgiDecodeString(char *pszString);

/**
 * @brief         Function to encode a string to remove any characters that are
 *                not safe in a URL
 *    
 * @param[out]    pszDest:  Pointer to the destination string
 * @param[in]     stLength: The length of the destination string
 * @param[in]     szSrc:    Pointer to the string to encode
 * 
 * @return        None.
 */
extern  void cgiEncodeString(char *pszDest, size_t stLength, char *szSrc);

/**
 * @brief         Function to replace unprintable chars
 * 
 * @param[in/out] pszString: Pointer to the string to search
 * @param[in]     chReplace: The character to replace them with
 * 
 * @Return        None.
 */
extern void cgiReplaceUnprintableChars(char *pszString, char chReplace);

/**
 * @brief         Function to format the file size 39.124k, 987.323M
 *   
 * @param[out]    pszDest: Pointer to the destination string
 * @param[in]     stFileSize: The size of the file
 * 
 * @Return        None.
 */
extern void cgiFileSize(char *pszDest, float fFileSize);

/**
 * @brief         Function to find a keyword in the session header
 *
 * @param[in]     pSess:       Pointer to the session data
 * @param[in]     pszKeyWord:  Pointer to the key word to find
 *
 * @retval        p_start: Pointer to the start of the keyword
 * @retval        NULL:    If not found
 */
extern char *cgiFindKeyword (PSESS pSess, char *pszKeyWord);

/**
 * @brief         Function to find a keyword in the session header
 *  
 * @param[in]     pSearch:    Pointer to the start of the search. 
 * @param[in]     pszEnd:     Pointer to the end of memory to search
 * @param[in]     pszKeyWord: Pointer to the key word to find
 *
 * @retval        p_start: Pointer to the start of the keyword 
 * @retval        NULL:    If not found
 */
extern char *cgiSearch (char *pszSearch, char *pszEnd, char *pszKeyWord);

#ifdef __cplusplus
}
#endif

#endif /* WEBCGI_H_INCLUDED */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
/******************************************************************************
End  Of File
******************************************************************************/
