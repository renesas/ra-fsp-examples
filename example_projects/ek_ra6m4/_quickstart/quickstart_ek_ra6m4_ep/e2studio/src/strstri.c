/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Technology Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Technology Corp. and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* TECHNOLOGY CORP. NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THE THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************
* Copyright (C) 2011. Renesas Technology Corp., All Rights Reserved.
*******************************************************************************    
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
