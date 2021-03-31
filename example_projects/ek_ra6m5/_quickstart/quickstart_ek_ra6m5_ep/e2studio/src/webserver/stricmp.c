/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. 
* This software is owned by Renesas Electronics Corporation and is protected 
* under all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************
* File Name    : stricmp.c
* Version      : 1.00
* Description  : The standard C library function stricmp is missing
*                from the CRT!
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.02.2011 1.00    First Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include <string.h>

/******************************************************************************
Public Functions
******************************************************************************/
int strnicmp(const char * s1, const char * s2, size_t count);
int stricmp(const char *s1, const char *s2);

/*****************************************************************************
Function Name: stricmp
Description:   Function to comppare two strings with case insensitivity
Parameters:    IN  s1 - Pointer to the first string
               IN  s2 - Pointer to the second string
Return value:  The numerical comparison of the two strings
*****************************************************************************/
int stricmp(const char *s1, const char *s2)
{
    char f, l;
    do
    {
        f = (char)((((*s1) <= 'Z') && ((*s1) >= 'A')) ? (((*s1) + 'a') - 'A') : (*s1));
        l = (char)((((*s2) <= 'Z') && ((*s2) >= 'A')) ? (((*s2) + 'a') - 'A') : (*s2));
        s1++;
        s2++;
    } while ((f) && (f == l));
    return (int) (f - l);
}
/*****************************************************************************
End of function  stricmp
******************************************************************************/

/*****************************************************************************
Function Name: strnicmp
Description:   Function to compare case insensitive two strings and return a
               numeric result
Arguments:     IN  s1 - Pointer to the first string
               IN  s2 - Pointer to the second string
               IN  count - The length of the string to compare
Return value:  If the two strings are equivalent the return is zero.
*****************************************************************************/
int strnicmp(const char *s1, const char *s2, size_t count)
{
    int f, l;
    do
    {
        if (((f = (unsigned char)(*(s1++))) >= 'A') && (f <= 'Z'))
        {
            f -= ('A' - 'a');
        }
        if (((l = (unsigned char)(*(s2++))) >= 'A') && (l <= 'Z'))
        {
            l -= ('A' - 'a');
        }
    } while ((--count) && f && (f == l));
    return f - l;
}
/*****************************************************************************
End of function  strnicmp
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
