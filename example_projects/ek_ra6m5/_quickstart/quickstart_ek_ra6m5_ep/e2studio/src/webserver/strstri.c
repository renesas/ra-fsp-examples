/***********************************************************************************************************************
* Copyright (c) 2011 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/******************************************************************************    
* File Name    : strstri.c
* Version      : 1.00
* Description  : Case insensitive version of the C library function strstr
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.01.2011 1.00    First Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include <string.h>
#include "strstri.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
* Function Name: strstri
* Description  : Function to locate case insensitive substring pszS2 in pszS1
* Arguments    : IN  pszS1 - Pointer to the string to search
*                IN  pszS2 - Pointer to the substring
* Return Value : Pointer to the first occurrence of pszS1 within pszS2
******************************************************************************/
const char * strstri(const char *pszS1, const char *pszS2)
{
    int iIndex;
    if (*pszS2)
    {
        while (*pszS1)
        {
            for (iIndex = 0;
                    (*(pszS1 + iIndex) | 0x20) == (*(pszS2 + iIndex) | 0x20);
                    iIndex++)
            {
                if (!*(pszS2 + iIndex + 1))
                {
                    return pszS1;
                }
            }
            pszS1++;
        }
        return NULL;
    }
    else
    {
        return pszS1;
    }
}
/******************************************************************************
End of function strstri
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
