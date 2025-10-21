/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : r_console.c
 * Version      : 1.0
 * Description  : Colour console, provides coloured text to attached console.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
 *********************************************************************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "r_typedefs.h"
#include "r_console.h"
#include "jlink_console.h"

#define MINIMUM_TOKEN_LENGTH                        (5)               // minimum length of a colour token
#define MAXIMUM_TOKEN_LENGTH                        (13)              // maximum length of a colour token

/* console_write */
void console_write(const char *buffer)
{
    unsigned length = strlen(buffer);

    if (length > 300)
    {
        length = 300;
    }

    print_to_console((char *) buffer); //length);
}

/*****************************************************************************
 * Function Name: get_colour
 *                Get the escape code string for the supplied colour tag
 * @param[in] char *string : the escape code
 * @param[out] bool_t *found : true if the tag was found, false if not
 * @retval the escape code for the colour tag, or the original string if there
 *         was no match
 ******************************************************************************/
static const char *get_colour(const char *string, bool *found)
{
    #define COLOURS     (13)
    const char *colour_codes[] = {"[BLACK]", "\x1B[30m", "[RED]", "\x1B[91m", "[GREEN]", "\x1B[92m", "[YELLOW]", "\x1B[93m",
                                  "[BLUE]", "\x1B[94m", "[MAGENTA]", "\x1B[95m", "[CYAN]", "\x1B[96m", "[WHITE]", "\x1B[97m",
                                  "[ORANGE]", "\x1B[38;5;208m", "[PINK]", "\x1B[38;5;212m", "[BROWN]", "\x1B[38;5;94m",
                                  "[PURPLE]", "\x1B[35m", "[DARKYELLOW]", "\x1B[38;5;220m"};
    uint8_t i;

    for (i = 0; i < COLOURS; i++)
    {
        if (0 == strcmp(string, colour_codes[i << 1]))
        {
            *found = true;
            return colour_codes[(i << 1) + 1];
        }
    }

    *found = false;
    return (string);
}
/******************************************************************************
 * End of function get_colour
 ******************************************************************************/

/* ************************************************************************** */
/* Function Name: detokenise                                                  */
/*                Replace colour tokens with terminal colour escape codes     */
/* @param[in] input : input string possibly containing colour tokens          */
/* @param[out] output : string with colour tokens replaced with escape codes  */
/* @retval none                                                               */
/* ************************************************************************** */
static void detokenise(const char * input, char *output)
{
    int16_t start_bracket_index;
    int16_t end_bracket_index;
    int16_t start_bracket_output_index;
    uint16_t i;
    uint16_t o;
    char token[MAXIMUM_TOKEN_LENGTH + 1];
    uint8_t token_length;
    bool token_found;
    const char *colour_code;
    bool token_replaced;

    start_bracket_index = -1;
    end_bracket_index = -1;
    start_bracket_output_index = 0;
    o = 0;

    /* scan the input string */
    for (i = 0; '\0' != input[i]; i++)
    {
        token_replaced = false;

        /* token start? */
        if ('[' == input[i])
        {
            start_bracket_index = (int16_t) i;
            start_bracket_output_index = (int16_t) o;
        }

        /* token end? */
        if (']' == input[i])
        {
            end_bracket_index = (int16_t) i;

            /* check to see if we have a token */
            if (start_bracket_index >= 0)
            {
                token_length = (uint8_t) (end_bracket_index - start_bracket_index + 1);

                if ((token_length >= MINIMUM_TOKEN_LENGTH) && (token_length <= MAXIMUM_TOKEN_LENGTH))
                {
                    /* copy the token to a buffer */
                    strncpy(token, &input[start_bracket_index], token_length);
                    token[token_length] = '\0';

                    /* check for a valid token */
                    colour_code = get_colour(token, &token_found);

                    /* if we have a colour token, then replace it in the output with the associated escape code */
                    if (token_found)
                    {
                        strcpy(&output[start_bracket_output_index], colour_code);
                        o = (uint16_t) (start_bracket_output_index + (int16_t) strlen(colour_code));
                        token_replaced = true;
                    }
                }
            }

            /* reset and start looking for another token */
            start_bracket_index = -1;
            end_bracket_index = -1;
        }

        /* if we didn't replace a token, then carry on copying input to output */
        if (!token_replaced)
        {
            output[o] = input[i];
            o++;
        }
    }

    /* terminate the output string */
    output[o] = '\0';
}
/******************************************************************************
 * End of function detokenise
 ******************************************************************************/

/*****************************************************************************
 * Function Name: printf_colour
 *                As printf_colour, but replaces colour tags with escape codes
 * @param[in] char *format : the format string
 * @param[in] ... : argument list, 0 or more parameters
 * @retval None
 ******************************************************************************/
void printf_colour(const char *format, ...)
{
    va_list arglist;
    static char colour_format[256];
    static char final_buffer[256];

    va_start(arglist, format);

    /* replace colour tokens with terminal colour escape codes */
    detokenise(format, colour_format);

    vsprintf(final_buffer, colour_format, arglist);
    va_end(arglist);

    console_write(final_buffer);
}
/******************************************************************************
 * End of function printf_colour
 ******************************************************************************/

void clear_console(void)
{
    console_write("\x1b[2J\x1b[H");
}
/******************************************************************************
 * End of function clear_console
 ******************************************************************************/
