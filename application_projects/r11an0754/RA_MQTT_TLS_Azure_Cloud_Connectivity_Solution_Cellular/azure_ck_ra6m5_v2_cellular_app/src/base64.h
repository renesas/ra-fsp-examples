/*
    base64.h - by Joe DF (joedf@ahkscript.org)
    Released under the MIT License

    See "base64.h", for more information.

    Thank you for inspiration:
    http://www.codeproject.com/Tips/813146/Fast-base-functions-for-encode-decode
*/

#ifndef BASE64_H_
#define BASE64_H_

#include <stdio.h>

unsigned int base64_int(unsigned int ch);
/* in : buffer of base64 string to be decoded.
   in_len : number of bytes to be decoded.
   out : pointer to buffer with enough memory, user is responsible for memory allocation, receives "raw" binary
   returns size of output excluding null byte
 */
unsigned int base64_decode(const unsigned char* in, unsigned int in_len, unsigned char* out);

#endif /* BASE64_H_ */
