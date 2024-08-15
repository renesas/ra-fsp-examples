/***********************************************************************************************************************
* Copyright (c) 2010 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/******************************************************************************
* File Name    : fmtOut.c
* Version      : 1.00
* Description  : Formatted output writer.
*******************************************************************************
* History      : DD.MM.YYYY Ver. Description
*              : 18.11.2010 1.00 First Release
******************************************************************************/

/******************************************************************************
  WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
  OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
  SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <float.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "fmtout.h"

/******************************************************************************
Macro Definitions
******************************************************************************/

/* The format flag bit positions */
#define FMTOUT_TYPE_SHORT           (1 << 0)
#define FMTOUT_TYPE_LONG            (1 << 1)
#define FMTOUT_ALTERNATE_FORMAT     (1 << 2)
#define FMTOUT_LEADING_ZEROS        (1 << 3)
#define FMTOUT_LEFT_JUSTIFY         (1 << 4)
/* Define the buffer */
#ifndef FMTOUT_BUFFER_SIZE
#define FMTOUT_BUFFER_SIZE          32
#endif

#ifndef FMTOUT_EXP_INDEX
#define FMTOUT_EXP_INDEX            2
#endif

/****************************************************************************
 Function Macros
 ****************************************************************************/
#define FMTOUT_PUT_CHAR(ch)         if (pfnPutChar(ch, pvGenericPointer))\
                                    return iCharCount;\
                                    else iCharCount++

/******************************************************************************
Typedef definitions
******************************************************************************/
/* Define the structure of data used by the formatted writer */
typedef struct _FMTOUT
{
    /* The minimum field width after the . or the muber of character of a
       string */
    int32_t iPrecision;

    /* Number of leading or trailing digits */
    int32_t     iCount;

    /* The minimum field before the . */
    int32_t     iFieldWidth;

    /* Pointer to the start of the formatted string. Usually the buffer on the
       stack, otherwize pointer to the start of strings. */
    char   *   pchStart;

    /* Pointer to the end of the string */
    char   *   pchEnd;

    /* Pointer to the hex look-up table */
    char   *   pchHexTable;

    /* The format option flags */
    uint8_t    byFlags;

    /* The current format flag */
    char      chFmt;

    /* The sign char   or 0 if not use */
    char      chSign;
} FMTOUT,
*PFMTOUT;

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
Function Prototypes
******************************************************************************/
static int32_t  fmtoGetInteger(const char  **ppszASCII);
static void fmtoPutInteger(uint32_t ulValue, uint32_t ulDivisor, PFMTOUT pFmt);
static void fmtoParsModifiers(const char  **ppszFormat, PFMTOUT pFmt);
static void fmtoFormatFloat(long double ldValue,
                            PFMTOUT     pFmt,
                            _Bool       bfSupressTrailingZeros,
                            _Bool       bfAltModifier);
long double fmtoCalculateIntegralTenLog(long double ldValue, int16_t * psiTenPow);
_Bool fntoSetPointPosition(PFMTOUT   pFmt,
                          int16_t     siTenPow,
                          char   *     *ppchStart,
                          int16_t *    psiPointPosition);

/******************************************************************************
Constant Data
******************************************************************************/

/* The bad format error string */
const char   gpszBadFormat[] = "[fmtOut: Illegal format]";

/* The null pointer error string */
const char   gpszNullPointer[] = "[fmtOut: Null string pointer]";

/* Default build will be without floating point support */
#ifndef _FMTOUT_FLOAT_SUPPORT_

/* The no float support error string */
const char   gpszNoFloatSupport[] = "[fmtOut: No float support]";
#else

/* There are not numbers for these so we will let the user know that they
   probably divided by zero */
const char   gpszPlusInfinity[] = "[+inf]";
const char   gpszMinusInfinity[] = "[-inf]";
#endif

/******************************************************************************
Public Functions
******************************************************************************/

/******************************************************************************
Function Name: fmtOut
Description:   Function to perform ANSI formatted output
Arguments:     IN  pszFormat - Pointer to the format string
               IN  pfnPutChar - Pointer to a function to output a char
               IN  pvGenericPointer - Pointer passed to pfnPutChar
               IN  ap - The argument pointer
Return value:  The number of characters printed
******************************************************************************/
int32_t fmtOut(const char    *pszFormat,
               PFNPUTCHAR    pfnPutChar,
               void          *pvGenericPointer,
               va_list       ap)
{
    int32_t iCharCount = 0;
    FMTOUT  Fmt;
    char    pchBuffer[FMTOUT_BUFFER_SIZE];

/* Forever */
    while (true)
    {
        /* Put all non-formatted chars */
        while ('%' != (Fmt.chFmt = *pszFormat++))
        {
            /* Check for end of string */
            if (!Fmt.chFmt)
            {
                return iCharCount;
            }

            /* Put the char in the pchBufferfer */
            FMTOUT_PUT_CHAR(Fmt.chFmt);
        }

        /* %% for % character */
        if ('%' == *pszFormat)
        {
            /* Put the % sign */
            FMTOUT_PUT_CHAR(*pszFormat++);

            /* 3.7c: Continue keyword is depreciated */
            continue;
        }

        /* Initialise variables */
        Fmt.chSign = 0;
        Fmt.byFlags = 0;
        Fmt.pchEnd = &pchBuffer[0];
        Fmt.pchStart = &pchBuffer[0];
        Fmt.pchHexTable = "0123456789ABCDEF";

        /* Get the modifiers */
        fmtoParsModifiers(&pszFormat, &Fmt);

        /* Check for the field width option '*' is for field width on argument
           list */
        if ('*' == *pszFormat)
        {
            /* Get the field width */
            Fmt.iFieldWidth = va_arg(ap, int32_t);

            /* If it is negative */
            if (Fmt.iFieldWidth < 0)
            {
                /* Make it postive */
                Fmt.iFieldWidth = -Fmt.iFieldWidth;

                /* and switch left adjust on */
                Fmt.byFlags |= FMTOUT_LEFT_JUSTIFY;
            }

            /* Point at next format char   */
            pszFormat++;
        }
        else
        {
            /* Convert the field width */
            Fmt.iFieldWidth = fmtoGetInteger(&pszFormat);
        }

        /* 0 and - together are invalid */
        if (Fmt.byFlags & FMTOUT_LEFT_JUSTIFY)
        {
            Fmt.byFlags = (uint8_t) (Fmt.byFlags & ~FMTOUT_LEADING_ZEROS);
        }

        /* Test for precision point */
        if ('.' == *pszFormat)
        {
            /* Check for the precision option '*' is for precision on
               argument list */
            if ('*' == *++pszFormat)
            {
                /* Set precision */
                Fmt.iPrecision = va_arg(ap, int32_t);
                pszFormat++;
            }
            else
            {
                /* Convert precision */
                Fmt.iPrecision = fmtoGetInteger(&pszFormat);
            }
        }
        else
        {
            /* No precision specified */
            Fmt.iPrecision = -1;
        }

        /* Check for size modifiers */
        if (('l' == *pszFormat)
        ||  ('L' == *pszFormat))
        {
            /* Set long modifier flag */
            Fmt.byFlags |= FMTOUT_TYPE_LONG;
            pszFormat++;
        }
        else if ('h' == *pszFormat)
        {
            /* Set short modifier flag */
            Fmt.byFlags |= FMTOUT_TYPE_SHORT;
            pszFormat++;
        }

        /* Get the format specifier */
        Fmt.chFmt = *pszFormat++;

        /* Select the appropriate conversion */
        switch (Fmt.chFmt)
        {
            /* 3.7e: Register keyword to advise compiler for optimisation */
            register uint32_t   ulValue;
#ifdef _FMTOUT_FLOAT_SUPPORT_
            /* 3.7e: Register keyword to advise compiler for optimisation */
            register long double  ldValue;
#endif
            /* Single character */
            case 'c':
            {
                pchBuffer[0] = (char  )va_arg(ap, int32_t);
                Fmt.pchEnd++;
                break;
            }

            /* String */
            case 's':
            {
            /* Get pointer to the string */
                Fmt.pchStart = va_arg(ap, char   *);
                if (!Fmt.pchStart)
                {
                    Fmt.pchStart = (char   *)gpszNullPointer;
                }

                /* Reset the count variable */
                Fmt.iCount = 0;

                /* Count the length of the string up to the precision */
                while (   (*Fmt.pchStart++)
                       && ((uint32_t)Fmt.iCount < (uint32_t)Fmt.iPrecision))
                {
                    Fmt.iCount++;
                }

                /* Set the pchBufferfer pointers */
                Fmt.pchEnd = --Fmt.pchStart;
                Fmt.pchStart -= Fmt.iCount;
                break;
            }

            /* Octal */
            case 'o':
            {
                if ((Fmt.byFlags & FMTOUT_ALTERNATE_FORMAT) && !Fmt.iPrecision)
                {
                    Fmt.iPrecision++;
                }

                /* Fall through */
            }

            /* Hex */
            case 'x':
            {
                Fmt.pchHexTable = "0123456789abcdef";

                /* Fall through */
            }

            /* Unsigned decimal */
            case 'u':
            {
                /* Fall through */
            }

            /* Generic pointer */
            case 'p':
            {
                /* Fall through */
            }

            /* Hexadecimal */
            case 'X':
            {
                /* Get variable from the argument list */
            if ('p' == Fmt.chFmt)
                {
                    ulValue = (uint32_t) va_arg(ap,char   *);
                }
                else if (sizeof(int16_t) == sizeof(int32_t))
                {
                    ulValue = va_arg(ap, uint32_t);
                }
                else if (sizeof(int16_t) == sizeof(int32_t))
                {
                    ulValue = (uint32_t) ((Fmt.byFlags & FMTOUT_TYPE_LONG) ? (uint32_t) va_arg(ap, int32_t) : va_arg(ap, uint32_t));
                }
                else
                {
                    ulValue = (uint32_t) ((Fmt.byFlags & FMTOUT_TYPE_SHORT) ? (uint16_t) va_arg(ap, int32_t) : va_arg(ap, int32_t));
                }

                /* No sign is applicable to unsigned */
                Fmt.chSign = 0;

                /* Initialise the output buffer */
                Fmt.pchEnd = &pchBuffer[FMTOUT_BUFFER_SIZE - 1];
                Fmt.pchStart = &pchBuffer[FMTOUT_BUFFER_SIZE - 1];

                /* Perform the conversion */
                fmtoPutInteger( ulValue,
                                (('o' == Fmt.chFmt) ? 8UL :
                                (('u' == Fmt.chFmt) ? 10UL : 16UL)),
                                &Fmt);
                break;
            }

            /* Signed decimal / integer */
            case 'd':
            {
                /* Fall through */
            }
            case 'i':
            {
                /* Select the access size */
                if (sizeof(int16_t) == sizeof(int32_t))
                {
                    /* Get the variable */
                    ulValue = (uint32_t) va_arg(ap, int32_t);
                }
                else if (sizeof(int16_t) == sizeof(int32_t))
                {
                    /* Get the variable */
                    ulValue = (uint32_t) ((Fmt.byFlags & FMTOUT_TYPE_LONG) ? va_arg(ap, int32_t) : (int32_t) va_arg(ap, int32_t));
                }
                else
                {
                    /* Get the variable */
                    ulValue = (uint32_t)((Fmt.byFlags & FMTOUT_TYPE_SHORT)
                            ? (int16_t) va_arg(ap, int32_t) : va_arg(ap,int32_t));
                }

                /* Check the sign */
                if (((int32_t)ulValue) < 0)
                {
                    /* Set the sign to - */
                    Fmt.chSign = '-';

                    /* Make the ldValue positive */
                    ulValue = -ulValue;
                }

                /* Initialise the output buffer */
                Fmt.pchEnd = &pchBuffer[FMTOUT_BUFFER_SIZE - 1];
                Fmt.pchStart = &pchBuffer[FMTOUT_BUFFER_SIZE - 1];

                /* Format the integer */
                fmtoPutInteger(ulValue, 10UL, &Fmt);
                break;
            }

#ifdef _FMTOUT_FLOAT_SUPPORT_
            case 'g':

                /* Fall through */
            case 'G':
            {
                Fmt.iCount = 1;
                Fmt.chFmt = (char) (Fmt.chFmt - 2);
                if (! Fmt.iPrecision)
                {
                    Fmt.iPrecision = 1;
                }

                /* Set a default precision of 6 if not specified */
                if (Fmt.iPrecision < 0)
                {
                    Fmt.iPrecision = 6;
                }
                if (sizeof(double) != sizeof(long double))
                {
                    /* Get the ldValue */
                    ldValue = (Fmt.byFlags & FMTOUT_TYPE_LONG)
                           ? va_arg(ap, long double) : va_arg(ap, double);

                    /* Check the sign */
                    if (ldValue < 0)
                    {
                        /* Set the sign to - */
                        Fmt.chSign = '-';

                        /* Make the ldValue +ve */
                        ldValue = -ldValue;
                    }
                }
                else
                {
                    /* Get the ldValue */
                    ldValue = va_arg(ap, long double);

                    /* Check the sign */
                    if (ldValue < 0)
                    {
                        /* Set the sign to - */
                        Fmt.chSign = '-';

                        /* Make the ldValue +ve */
                        ldValue = -ldValue;
                    }
                }

                /* Format the float */
                fmtoFormatFloat(ldValue,
                                &Fmt,
                                (_Bool)Fmt.iCount,
                                (_Bool)(Fmt.byFlags & FMTOUT_ALTERNATE_FORMAT));

                /* Add leading zeros if required */
                if (Fmt.byFlags & FMTOUT_LEADING_ZEROS)
                {
                    Fmt.iPrecision = Fmt.iFieldWidth - (0 != Fmt.chSign);
                    while (Fmt.iPrecision > Fmt.pchEnd - Fmt.pchStart)
                    {
                        *--Fmt.pchStart = '0';
                    }
                }
                break;
            }

            case 'f':
            {
                Fmt.chFmt = 0;
            }
            case 'e':
            {
                /* Fall through */
            }
            case 'E':
            {
                Fmt.iCount = 0;

                /* Set a default precision of 6 if not specified */
                if (Fmt.iPrecision < 0)
                {
                    Fmt.iPrecision = 6;
                }
                if (sizeof(double) != sizeof(long double))
                {
                    /* Get the ldValue */
                    ldValue = (Fmt.byFlags & FMTOUT_TYPE_LONG)
                           ? va_arg(ap, long double) : va_arg(ap, double);

                    /* Check the sign */
                    if (ldValue < 0)
                    {
                        /* Set the sign to - */
                        Fmt.chSign = '-';

                        /* Make the ldValue +ve */
                        ldValue = -ldValue;
                    }
                }
                else
                {
                    /* Get the ldValue */
                    ldValue = va_arg(ap, long double);

                    /* Check the sign */
                    if (ldValue < 0)
                    {
                        /* Set the sign to - */
                        Fmt.chSign = '-';

                        /* Make the ldValue +ve */
                        ldValue = -ldValue;
                    }
                }

                /* Format the float */
                fmtoFormatFloat(ldValue,
                                &Fmt,
                                (_Bool)Fmt.iCount,
                                (_Bool)(Fmt.byFlags & FMTOUT_ALTERNATE_FORMAT));

                /* Add leading zeros if required */
                if (Fmt.byFlags & FMTOUT_LEADING_ZEROS)
                {
                Fmt.iPrecision = Fmt.iFieldWidth - (0 != Fmt.chSign);
                    while (Fmt.iPrecision > Fmt.pchEnd - Fmt.pchStart)
                    {
                        *--Fmt.pchStart = '0';
                    }
                }
                break;
            }
#else
            case 'g':
            {
                /* Fall through */
            }
            case 'G':
            {
                /* Fall through */
            }
            case 'f':
            {
                /* Fall through */
            }
            case 'e':
            {
                /* Fall through */
            }
            case 'E':
            {
                /* Get the value from the argument list so as not to
                   print rubish for all the other parameters */
                if (sizeof(double) != sizeof(long double))
                {
                    long double ldValue = (Fmt.byFlags & FMTOUT_TYPE_LONG)
                           ? va_arg(ap, long double) : va_arg(ap, double);
                }
                else
                {
                    /* Get the ldValue */
                    long double ldValue = va_arg(ap, long double);
                }
                Fmt.pchStart = gpszNoFloatSupport;
                Fmt.pchEnd = gpszNoFloatSupport + sizeof(gpszNoFloatSupport);
                break;
            }
#endif
            /* The number of characters output so far */
            case 'n':
            {
                /* Test for 32bit address bus */
                if (sizeof(int16_t) == sizeof(int32_t))
                {
                    /* Access as int32_t */
                    *va_arg(ap, int32_t *) = iCharCount;

                    /* Test for 32bit CPU type */
                }
                else if (sizeof(int16_t) != sizeof(int32_t))
                {
                    /* Check for 32/64bit register size */
                    if (sizeof(int32_t) != sizeof(int32_t))
                    {
                        /* Access accordingly */
                        if (Fmt.byFlags & FMTOUT_TYPE_SHORT)
                        {
                            *va_arg(ap, int16_t *) = (int16_t)iCharCount;
                        }
                        else if (Fmt.byFlags & FMTOUT_TYPE_LONG)
                        {
                            *va_arg(ap, int32_t *) = iCharCount;
                        }
                        else
                        {
                            *va_arg(ap, int32_t *) = iCharCount;
                        }
                    }
                    else
                    {
                        /* 16/32 bit access */
                        if (Fmt.byFlags & FMTOUT_TYPE_SHORT)
                        {
                            *va_arg(ap, int16_t *) = (int16_t)iCharCount;
                        }
                        else
                        {
                            *va_arg(ap, int32_t *) = iCharCount;
                        }
                    }

                    /* Assume 32bit address bus */
                }
                else
                {
                    if (Fmt.byFlags & FMTOUT_TYPE_LONG)
                    {
                        *va_arg(ap, int32_t *) = iCharCount;
                    }
                    else
                    {
                        *va_arg(ap, int32_t *) = iCharCount;
                    }
                }

                /* 3.7c: Continue keyword is depreciated */
                continue;
            }

            /* End of string in format argument */
            case '\0':
            {
                pszFormat--;

                /* Fall through */
            }

            /* Bad format argument */
            default:
            {
                Fmt.pchStart = (char   *)gpszBadFormat;
                Fmt.pchEnd = (char   *)gpszBadFormat + sizeof(gpszBadFormat);
                break;
            }
        }

        /* Calculate the length of the data */
        Fmt.iPrecision = (int32_t)(Fmt.pchEnd - Fmt.pchStart);

        /* Set the trailing space count */
        if (Fmt.iPrecision > Fmt.iFieldWidth)
        {
            Fmt.iCount = 0;
        }
        else
        {
        /* Subtract an extra one if there is a sign required */
            Fmt.iCount = (Fmt.iFieldWidth - Fmt.iPrecision) - (0 != Fmt.chSign);
        }

        /* Write out any leading pad characters */
        if ((Fmt.byFlags & FMTOUT_LEFT_JUSTIFY) == 0)
        {
            while ((--Fmt.iCount) >= 0)
            {
                FMTOUT_PUT_CHAR(' ');
            }
        }

        /* Write the sign char   */
        if (Fmt.chSign)
        {
            FMTOUT_PUT_CHAR(Fmt.chSign);
#ifdef FMTOUT_ALTEXP_SIGN_ALWAYS
        }
        else if ((Fmt.chFmt)
                &&  (Fmt.byFlags & FMTOUT_ALTERNATE_FORMAT))
        {
            FMTOUT_PUT_CHAR('+');
#endif
        }

        /* Write the formatted chars */
        while ((--Fmt.iPrecision) >= 0)
        {
            FMTOUT_PUT_CHAR(*Fmt.pchStart++);
        }

        /* Write traling spaces for left justification */
        if (Fmt.byFlags & FMTOUT_LEFT_JUSTIFY)
        {
            while ((--Fmt.iCount) >= 0)
            {
                FMTOUT_PUT_CHAR(' ');
            }
        }
    }
}
/******************************************************************************
End of function  fmtOut
******************************************************************************/

/******************************************************************************
Private Functions
******************************************************************************/

/******************************************************************************
Function Name: fmtoGetInteger
Description:   Function to convert ASCII to integer
Arguments:     IN/OUT - Pointer to the string pointer
Return Value:  The binary ldValue
******************************************************************************/
int32_t fmtoGetInteger(const char  **ppszASCII)
{
    int32_t iValue = 0;
    while (((*(*ppszASCII)) >= '0') && ((*(*ppszASCII)) <= '9'))
    {
        iValue = (iValue * 10) + ((*(*ppszASCII)++) - '0');
    }
    return iValue;
}
/******************************************************************************
End of function  fmtoGetInteger
******************************************************************************/

/******************************************************************************
Function Name: fmtoPutInteger
Description:   Function to perform the integer conversion
Arguments:     IN/OUT ulValue - The ldValue to convert
               IN     ulDivisor - The conversion divisor
               IN/OUT pFmt - Pointer to the format variables
Return Value:  N/A
******************************************************************************/
void fmtoPutInteger(uint32_t ulValue, uint32_t ulDivisor, PFMTOUT pFmt)
{
    _Bool bfNonZeroValue = (_Bool) (0 != ulValue);

    /* Nothing is printed if zero precision */
    if ((0 != pFmt->iPrecision)
           || (bfNonZeroValue))
    {
        /* Print the chars backwards */
        do
        {
            /* Look-up ASCII ldValue in table */
            *--pFmt->pchStart = pFmt->pchHexTable[(ulValue % ulDivisor)];

            /* While there is a significant digit */
        } while (ulValue /= ulDivisor);
    }

    /* Precision field size adjust */
    if (pFmt->iPrecision < 0)
    {
        if (pFmt->byFlags & FMTOUT_LEADING_ZEROS)
        {
            pFmt->iPrecision = pFmt->iFieldWidth - (0 != pFmt->chSign);
        }
    }

    /* Write leading zeros */
    while (pFmt->iPrecision > (pFmt->pchEnd - pFmt->pchStart))
    {
        *--pFmt->pchStart = '0';
    }

    /* Check for the laternate format flag */
    if (    (pFmt->byFlags & FMTOUT_ALTERNATE_FORMAT)
         && (bfNonZeroValue))
    {

        /* Test for hex */
        if ('x' == pFmt->chFmt || 'X' == pFmt->chFmt)
        {
            /* Write out the x */
            *--pFmt->pchStart = pFmt->chFmt;

            /* Write out the 0 */
            *--pFmt->pchStart = '0';

            /* Test for octal */
        }
        else if (('o' == pFmt->chFmt) && ('0' != *pFmt->pchStart))
        {
          /* Add leading 0 for octal */
          *--pFmt->pchStart = '0';
        }
    }
}
/*****************************************************************************
End of function  fmtoPutInteger
******************************************************************************/

/******************************************************************************
Function Name: fmtoParsModifiers
Description:   Function to pars the modifiers
Arguments:     IN/OUT ppszFormat - Pointer to the format pointer
               IN/OUT pFmt - Pointer to the format varaibles
Return Value:  N/A
******************************************************************************/
void fmtoParsModifiers(const char  **ppszFormat, PFMTOUT pFmt)
{
    /* Pars modifier flags */
    while (true)
    {
        /* Get the modifier */
        pFmt->chFmt = *(*ppszFormat);

        /* Check for sign padding */
        if (' ' == pFmt->chFmt)
        {
            /* Set padding character */
            if (!pFmt->chSign)
            {
                pFmt->chSign = pFmt->chFmt;
            }

            /* Check for + sign */
        }
        else if ('+' == pFmt->chFmt)
        {
            pFmt->chSign = pFmt->chFmt;

            /* Check for left justify */
        }
        else if ('-' == pFmt->chFmt)
        {
            pFmt->byFlags |= FMTOUT_LEFT_JUSTIFY;

            /* Check for alternate format */
        }
        else if ('#' == pFmt->chFmt)
        {
            pFmt->byFlags |= FMTOUT_ALTERNATE_FORMAT;

            /* Check for leading zeros */
        }
        else if ('0' == pFmt->chFmt)
        {
            pFmt->byFlags |= FMTOUT_LEADING_ZEROS;
        }
        else
        {
            /* End of modifiers */
            break;
        }

        /* Loop and check next flag */
        (*ppszFormat)++;
    }
}
/******************************************************************************
End of function  fmtoParsModifiers
******************************************************************************/

/******************************************************************************
Function Name: fmtoCalculateIntegralTenLog
Description:   Function to calculate the integral portion of the 10 powers
Arguments:     IN  ldValue - The value to convert
               OUT psiTenPow - Pointer to the 10 power portion
Return Value:  The converted value
******************************************************************************/
#ifdef _FMTOUT_FLOAT_SUPPORT_
long double fmtoCalculateIntegralTenLog(long double ldValue, int16_t * psiTenPow)
{
    int16_t   siTenPow = 0;

    /* if it is +ve */
    if (ldValue >= 1)
    {
        /* 10 * 10 check for speed */
        while (ldValue >= 1e11)
        {
            ldValue /= 1e10;
            siTenPow = (int16_t) (siTenPow + 10);
        }

        /* Divide and add */
        while (ldValue >= 10)
        {
            ldValue /= 10;
            siTenPow++;
        }

    /* Make sure that it is not 0.0 */
    }
    else if (ldValue != 0.0f)
    {
        /* 10 * 10 check for speed */
        while (ldValue <= 1e-10)
        {
            ldValue *= 1e10;
            siTenPow = (int16_t) (siTenPow - 10);
        }

        /* Divide and subtract */
        while (ldValue < 1)
        {
            ldValue *= 10;
            siTenPow--;
        }
    }

    /* Set the whole number */
    *psiTenPow = siTenPow;

    return ldValue;
}
/******************************************************************************
End of function  fmtoCalculateIntegralTenLog
******************************************************************************/

/******************************************************************************
Function Name: fntoSetPointPosition
Description:   Function to calclate the point position and write out the
               zeros, including trailing into the format buffer
Arguments:     IN  pFmt - Pointer to the format variables
               IN  siTenPow - The power of the number
               OUT ppchStart - Pointer to the output format buffer
               OUT psiPointPosition - Pointer to the point position
Return value:  true if there is more to do
 *
******************************************************************************/
_Bool fntoSetPointPosition(PFMTOUT   pFmt,
                          int16_t     siTenPow,
                          char   *     *ppchStart,
                          int16_t *    psiPointPosition)
{
    /* Check for %e or %E */
    if (pFmt->chFmt)
    {
        /* Set the point position */
        *psiPointPosition = 0;
    }
    else
    {
        int16_t siCount, siZeroCount = 0;

        /* Check for less than one */
        if (siTenPow < 0)
        {
            /* Write out the 0 portion */
            *(*ppchStart)++ = '0';
            siCount = (int16_t)pFmt->iPrecision;

            /* Write out the '.' if required */
            if ((siCount) || (pFmt->byFlags & FMTOUT_ALTERNATE_FORMAT))
            {
               *(*ppchStart)++ = '.';
            }

            /* Write out the zeros to the precision specified */
            while ((--siZeroCount > siTenPow)
                && (pFmt->iPrecision))
            {
                *(*ppchStart)++ = '0';
                pFmt->iPrecision--;
            }

            /* Check for further digits */
            if (siTenPow < (-siCount - 1))
            {
                /* No more to write out */
                return false;
            }

            /* Set the point position */
            *psiPointPosition = 1;
        }
        else
        {
            /* The point is at the end */
            *psiPointPosition = (int16_t)-siTenPow;
        }
    }

    /* More digits to go */
    return true;
}
/******************************************************************************
End of function  fntoSetPointPosition
******************************************************************************/

/******************************************************************************
Function Name: fmtoFormatFloat
Description:   Function to perform floting point formatting
Arguments:     IN  ldValue - The value to convert
               IN  pFmt - Pointer to the formatting variables
               IN  bfSupressTrailingZeros - true to supress zeros
               IN  bfAltModifier - true for the alternate format
Return value:  0 for success otherwise error code
******************************************************************************/
static void fmtoFormatFloat(long double  ldValue,
                            PFMTOUT pFmt,
                            _Bool    bfSupressTrailingZeros,
                            _Bool    bfAltModifier)
{
    char   *   pchStart, * pchBuffer;
    int16_t   siPointPosition, siTenPow;

    /* IEE754 64 bit +infinity */
    if (ldValue > DBL_MAX)
    {
        pFmt->pchStart = (char   *)gpszPlusInfinity;
        pFmt->pchEnd = (char   *)gpszPlusInfinity + sizeof(gpszPlusInfinity) - 1;
        return;
    }

    /* IEE754 64 bit -infinity */
    if ((ldValue < DBL_MIN) && (0 != ldValue))
    {
        pFmt->pchStart = (char   *)gpszMinusInfinity;
        pFmt->pchEnd = (char   *)gpszMinusInfinity + sizeof(gpszMinusInfinity) - 1;
        return;
    }

    /* Set the output buffer size */
    pFmt->pchStart += pFmt->iFieldWidth;
    pchBuffer = pFmt->pchStart;
    pchStart = pFmt->pchStart;
    siTenPow = 0;

    /* Multiply out */
    ldValue = fmtoCalculateIntegralTenLog(ldValue, &siTenPow);

    /* Test for zero suppression */
    if (bfSupressTrailingZeros)
    {
        /* Test for e format */
        if ((siTenPow < pFmt->iPrecision)
        &&  (siTenPow >= -4)) {

            /* Adjust format */
            pFmt->chFmt = 0;
            pFmt->iPrecision -= siTenPow;
        }
        pFmt->iPrecision--;

        /* Check for alternate format */
        if (bfAltModifier)
        {
            /* %#G always has trailing zeros */
            bfSupressTrailingZeros = false;
        }
    }

    /* Check for the requirement fo a decimal point */
    if (fntoSetPointPosition(pFmt,
                             siTenPow,
                             &pchStart,
                             &siPointPosition))
    {
        int32_t iPoint = (int32_t)siPointPosition;
        while (iPoint <= pFmt->iPrecision)
        {
            /* Get digit */
            int16_t iCount = (int16_t)ldValue;
            ldValue -= iCount;

            /* Multiply for next digit */
            ldValue *= 10;
            *pchStart++ = (char  )(iCount + '0');

            /* Test for point position */
            if ((!iPoint) && (pFmt->iPrecision || bfAltModifier))
            {
                /* Insert point */
                *pchStart++ = '.';
            }

            /* Count the digits until the point */
            iPoint++;
        }

        /* Test for rounding */
        if (ldValue >= 5)
        {
            _Bool    bfCarry = true;
            char   *   pchCarry = pchStart - 1;
            do
            {
                /* If the char   is not a '.' */
                if ('.' != *pchCarry)
                {

                    /* Add 1 */
                	if (bfCarry)
                	{
                        (*pchCarry)++;
                	}

                    /* Test for rollover to 0 */
                    if (('9' + 1) == *pchCarry)
                    {
                        *pchCarry = '0';

                        /* Set carry flag */
                        bfCarry = true;
                    } else {
                        bfCarry = false;
                    }
                }
            } while (pchCarry-- > pchBuffer);

            /* If the carry flag is set */
            if (bfCarry)
            {
                /* for formats %e or %E */
                if (pFmt->chFmt)
                {
                    /* Start at the top */
                    pchCarry = pchStart;

                    /* Undo the carry of the test loop */
                    while (pchCarry > pchBuffer)
                    {
                        if ('.' ==  *(pchCarry - 1))
                        {
                            *pchCarry = *(pchCarry - 2);
                            pchCarry--;
                        } else {
                            *pchCarry = *(pchCarry - 1);
                        }
                        pchCarry--;
                    }

                    /* Raise by power of 10 */
                    siTenPow++;

                }
                else
                {
                    /* Leave last digit rounded */
                    pchCarry = ++pchStart;

                    /* Undo the carry of the test loop */
                    while (pchCarry > pchBuffer)
                    {
                        *pchCarry = *(pchCarry - 1);
                        pchCarry--;
                    }
                }
                *pchBuffer = '1';
            }
        }
    }

    /* Check for trailing zero suppression */
    if (bfSupressTrailingZeros)
    {
        /* If trailing zeros are supressed then back the pointer until the
           first digit is found */
        while ('0' == *(pchStart - 1))
        {
            pchStart--;
        }

        /* If the next digit is a point then supress the point */
        if ('.' == *(pchStart - 1))
        {
            pchStart--;
        }
    }

    /* Check for formats %e or %E */
    if (pFmt->chFmt)
    {
        int32_t iCount,iCopy;

        /* Put in the e or E char   */
        *pchStart++ = pFmt->chFmt;

        /* Check for -ve */
        if (siTenPow < 0)
        {
            /* Insert the sign */
            *pchStart++ = '-';

            /* Make the power +ve */
            siTenPow = (int16_t)-siTenPow;
        }
        else
        {
            /* Insert the sign */
            *pchStart++ = '+';
        }
        iCount = 0;
        pchStart +=10;

        /* Write out the significant digits */
        do
        {
            iCount++;
            *pchStart++ = (char  )((siTenPow % 10) + '0');
            siTenPow /= 10;
        } while ((siTenPow) || (iCount < FMTOUT_EXP_INDEX));

        /* Copy into position */
        for (iCopy = iCount ; iCount > 0 ; iCount-- )
        {
          *(pchStart - 11 - iCopy + iCount) = *(pchStart - iCount);
        }

        /* Adjust the pointer */
        pchStart -= 10;
    }

    /* Update the pointer */
    pFmt->pchEnd = pchStart;
}
#endif
/******************************************************************************
End of function  fmtoFormatFloat
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
