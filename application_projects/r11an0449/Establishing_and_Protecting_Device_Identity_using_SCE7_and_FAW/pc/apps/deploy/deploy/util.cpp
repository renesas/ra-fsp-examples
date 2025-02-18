/**********************************************************************************************************************
* Copyright (c) 2017 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

/*******************************************************************************
* File Name    : util.cpp
* Description  : Misc utility functions
*
******************************************************************************/

/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*
*         : 25.04.2017 1.00     First Release
******************************************************************************/

/******************************************************************************
Includes
******************************************************************************/
//#include "StdAfx.h"
#include "util.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/


util::util(void)
{
}


util::~util(void)
{
}


/* 
   base64.cpp and base64.h

   Copyright (C) 2004-2008 Ren� Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   Ren� Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

char base64_chars[] = {
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/"
};


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

int getIndex(char ch)
{
	char *ptr = strchr(base64_chars, ch);
	if (ptr == NULL)
		return -1;
	else
		return ptr - base64_chars;


}

int util::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len, char *output)
{
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];
  char *originalOutput = output;



  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        *output++ =  base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      *output++ =  base64_chars[char_array_4[j]];

    while((i++ < 3))
     *output++ =  '=';

  }

  return output - originalOutput;	// Length

}


int util::base64_decode(char *encoded_string, unsigned char *output)
{
  int in_len = strlen(encoded_string);
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  unsigned char *originalOutput = output;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = getIndex(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        *output++ = char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = getIndex(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) *output++ = char_array_3[j];
  }

  return output - originalOutput;	// Length

}

#define	LF_REPLACEMENT '*'
void util::replaceLF(char *text)
{
	char *ptr = text;

	while (1)
	{
		ptr = strchr(ptr, '\n');
		if (ptr == NULL)
			return;
		*ptr = LF_REPLACEMENT;
	}

}

void util::restoreLF(char *text)
{
	char *ptr = text;

	while (1)
	{
		ptr = strchr(ptr, LF_REPLACEMENT);
		if (ptr == NULL)
			return;
		*ptr = '\n';
	}


}

// Decodes in place
void util::decodeHttp(char *text)
{
	int len = strlen(text);
	char *ptr;
	char *dest = text;

	for (ptr = text; ptr < text + len;)
	{
		if (*ptr == '%')
		{

			// Isolate text
			ptr++;
			char valText[3];
			strncpy(valText, ptr, 2);
			ptr += 2;
			valText[2] = 0;
		
			// Convert it
			int val;
			sscanf(valText, "%x", &val);

			// Copy it in
			*dest++ = val;
		}
		else
		{
			*dest++ = *ptr++;
		}

	}

	// Add the NULL terminator
	*dest++ = 0;

}

