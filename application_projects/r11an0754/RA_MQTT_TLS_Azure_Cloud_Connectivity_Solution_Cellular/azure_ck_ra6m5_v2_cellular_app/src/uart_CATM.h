/***********************************************************************************************************************
 * File Name    : uart_CATM.h
 * Description  : Contains declaration of uart_CATM.c functions
 **********************************************************************************************************************/
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/


#ifndef UART_CATM_H_
#define UART_CATM_H_

#ifdef EXTERN       // have to undef before re-defining
#undef EXTERN
#endif

#ifdef UART_CATM_C
    #define EXTERN
#else
    #define EXTERN extern
#endif


/* Macro definition */
#define CARRIAGE_ASCII            (13u)     /* Carriage return */
#define LINEFEED_ASCII            (10u)     /* Line Feed */

#define ZERO_ASCII                (48u)     /* ASCII value of zero */
#define NINE_ASCII                (57u)     /* ASCII value for nine */
#define SPACE_CHAR                (32u)     /* ASCII value for a space */
#define TILDE_CHAR                (126u)    /* Tilde character is the largest ASCII value allowed */

#define UART_MAX_DATA_LENGTH      (80u)      /* Expected Input Data length */
#define UART_ERROR_EVENTS         (UART_EVENT_BREAK_DETECT | UART_EVENT_ERR_OVERFLOW | UART_EVENT_ERR_FRAMING | \
                                    UART_EVENT_ERR_PARITY)    /* UART Error event bits mapped in registers */

/* Function declaration */
fsp_err_t CATM_uart_print_user_msg(uint8_t *p_msg);
fsp_err_t CATM_uart_initialize(void);
fsp_err_t CATM_initialize(void);
fsp_err_t CATM_Ping(void);

EXTERN char g_strIMEI[UART_MAX_DATA_LENGTH]         /* Normally 15 digits */
#ifdef UART_CATM_C
= {0}
#endif
;

EXTERN char g_strICCID[UART_MAX_DATA_LENGTH]        /* Normally 19 digits */
#ifdef UART_CATM_C
= {RESET_VALUE}
#endif
;


#endif /* UART_CATM_H_ */
