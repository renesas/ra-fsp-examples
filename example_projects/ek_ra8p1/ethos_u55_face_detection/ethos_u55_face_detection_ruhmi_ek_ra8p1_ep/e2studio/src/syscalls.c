/***********************************************************************************************************************
 * File Name    : syscall.c
 * Description  : Contains system call implementations for standard I/O redirection.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include "common_utils.h"

static int putchar_wrapper(char c, FILE *file)
{
    (void) file;            /* Not used in this function */
    char string[2];
    sprintf(string,"%c",c);
    APP_PRINT(string);        /* Defined by underlying system */
    return c;
}

static FILE __stdio = FDEV_SETUP_STREAM(putchar_wrapper,
                                        NULL,
                                        NULL,
                                        _FDEV_SETUP_WRITE);
FILE *const stdin = &__stdio;
FILE *const stdout = &__stdio;
FILE *const stderr = &__stdio;

void _exit(int status) {
    (void)status;
    while (1);  // Trap here forever
}

