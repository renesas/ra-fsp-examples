/*
    base64.c - by Joe DF (joedf@ahkscript.org)
    Released under the MIT License

    See "base64.h", for more information.

    Thank you for inspiration:
    http://www.codeproject.com/Tips/813146/Fast-base-functions-for-encode-decode
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "base64.h"

/* Base64 char table - used internally for encoding */

unsigned int base64_int(unsigned int ch)
{

    /* ASCII to base64_int
       65-90  Upper Case  >>  0-25
       97-122 Lower Case  >>  26-51
       48-57  Numbers     >>  52-61
       43     Plus (+)    >>  62
       47     Slash (/)   >>  63
       61     Equal (=)   >>  64~
    */
    if (ch==43)
    return 62;
    if (ch==47)
    return 63;
    if (ch==61)
    return 64;
    if ((ch>47) && (ch<58))
    return ch + 4;
    if ((ch>64) && (ch<91))
    return ch - 'A';
    if ((ch>96) && (ch<123))
    return (ch - 'a') + 26;
    return 0;
}

/*******************************************************************************************************************//**
 * @brief       Generate hex data from BASE64 data
 * @param[in]   in      Buffer of base64 string to be decoded
 * @param[in]   in_len  Number of bytes to be decoded
 * @param       out     Pointer to buffer with enough memory, user is responsible for memory allocation
 * @retval      k       Return hex data len
 ***********************************************************************************************************************/
unsigned int base64_decode(const unsigned char* in, unsigned int in_len, unsigned char* out) {

    unsigned int i=0, j=0, k=0, s[4] = {0,0,0,0};

    for (i=0;i<in_len;i++)
	{
        s[j++]=base64_int(*(in+i));
        if (j==4)
		{
            out[k+0] = ((s[0]&255)<<2)+((s[1]&0x30)>>4);
            if (s[2]!=64)
			{
                out[k+1] = ((s[1]&0x0F)<<4)+((s[2]&0x3C)>>2);
                if ((s[3]!=64))
				{
                    out[k+2] = ((s[2]&0x03)<<6)+(s[3]); k+=3;
                }
				else
				{
                    k+=2;
                }
            }
			else
			{
                k+=1;
            }
            j=0;
        }
    }

    return k;
}