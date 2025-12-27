/***********************************************************************************************************************
 * File Name    : uart_CATM.h
 * Description  : Contains declaration of uart_CATM.c functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef UART_CATM_H_
#define UART_CATM_H_

#ifdef EXTERN       // have to undef before re-defining
#undef EXTERN
#endif

#ifdef UART_CATM_C
    #define EXTERN
#else
    #define EXTERN extern // @suppress("Macro expansion")
#endif


/* Macro definition */
#define CARRIAGE_ASCII            (13u)     /* Carriage return */
#define LINEFEED_ASCII            (10u)     /* Line Feed */

#define ZERO_ASCII                (48u)     /* ASCII value of zero */
#define NINE_ASCII                (57u)     /* ASCII value for nine */
#define SPACE_CHAR                (32u)     /* ASCII value for a space */
#define TILDE_CHAR                (126u)    // Tilde character is the largest ASCII value allowed */

#define UART_MAX_DATA_LENGTH      (80u)      /* Expected Input Data length */
#define UART_ERROR_EVENTS         (UART_EVENT_BREAK_DETECT | UART_EVENT_ERR_OVERFLOW | UART_EVENT_ERR_FRAMING | \
                                    UART_EVENT_ERR_PARITY)    /* UART Error event bits mapped in registers */

/* Function declaration */
fsp_err_t CATM_uart_print_user_msg(uint8_t *p_msg);
fsp_err_t CATM_uart_initialize(void);
fsp_err_t CATM_initialize(void);
fsp_err_t CATM_Ping(void);

void deinit_uart(void);
bool isExpectedResponse(char *p2ExpectedStr);

EXTERN char g_strIMEI[UART_MAX_DATA_LENGTH]         // normally 15 digits
#ifdef UART_CATM_C
= {0}
#endif
;

EXTERN char g_strICCID[UART_MAX_DATA_LENGTH]        // normally 19 digits
#ifdef UART_CATM_C
= {RESET_VALUE}
#endif
;


#endif /* UART_CATM_H_ */
