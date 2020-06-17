/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "uart_stdout.h"

#include <assert.h>
#include <Driver_USART.h>
#include <stdint.h>
#include <string.h>
#include "hal_data.h"
//#include "target_cfg.h"


#define ASSERT_HIGH(X)  assert(X == ARM_DRIVER_OK)


/* Imports USART driver */
extern ARM_DRIVER_USART   Driver_USART;

int _write(int fd, char *str, int len);

static void uart_putc(unsigned char c)
{
    int32_t ret = ARM_DRIVER_OK;

    ret = Driver_USART.Send(&c, 1);
    ASSERT_HIGH(ret);
}

/* Redirects printf to TFM_DRIVER_STDIO in case of ARMCLANG*/
#if defined(__ARMCC_VERSION)
/* __ARMCC_VERSION is only defined starting from Arm compiler version 6 */
int fputc(int ch, FILE *f)
{
    /* Send byte to USART */
    uart_putc(ch);

    /* Return character written */
    return ch;
}
#elif defined(__GNUC__)
/* Redirects printf to TFM_DRIVER_STDIO in case of GNUARM */
int _write(int fd, char *str, int len)
{
    int i;
    (void)fd;   /* Not used, avoid warning */

    for (i = 0; i < len; i++) {
        /* Send byte to USART */
        uart_putc((unsigned char)str[i]);
    }

    /* Return the number of characters written */
    return len;
}
#endif

void stdio_init(void)
{
    int32_t ret = ARM_DRIVER_OK;

    ret = Driver_USART.Initialize(NULL);
    ASSERT_HIGH(ret);
}

void stdio_uninit(void)
{
    int32_t ret = ARM_DRIVER_OK;

    ret = Driver_USART.Uninitialize();
    ASSERT_HIGH(ret);
}
