/******************************************************************************
* DISCLAIMER
* Please refer to http://www.renesas.com/disclaimer
******************************************************************************
  Copyright (C) 2011. Renesas Technology Corp., All Rights Reserved.
*******************************************************************************
* File Name    : strstri
* Version      : 1.00
* Description  : Case insensitive version of the C library function strstr
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.01.2011 1.00    First Release
******************************************************************************/

#ifndef STRSTRI_FILENAME_H
#define STRSTRI_FILENAME_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

/**
 * @brief Function to locate case insensitive substring pszS2 in pszS1
 * @param pszS1 - Pointer to the string to search
 * @param pszS2 - Pointer to the substring
 * @return Pointer to the first occurrence of pszS1 within pszS2
 */
extern  const char * strstri (const char * pszS1, const char * pszS2);

#ifdef __cplusplus
}
#endif

#endif /* STRSTRI_FILENAME_H */
