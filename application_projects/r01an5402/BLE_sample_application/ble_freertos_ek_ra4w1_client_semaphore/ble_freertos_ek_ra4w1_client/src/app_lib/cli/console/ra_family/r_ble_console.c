/**********************************************************************************************************************
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
* Copyright (C) 2019-2020 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name: r_ble_console.c
 * Version : 1.0
 * Description : Command Line Interface.
 *********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "hal_data.h"
#include "r_ble_cli.h"

#if (BLE_CFG_CMD_LINE_EN == 1)

#include "cli/console/r_ble_console.h"

#if (BSP_CFG_RTOS == 1)
#include "tx_api.h"
extern void *g_ble_event_group_handle;
#elif (BSP_CFG_RTOS == 2)
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
extern EventGroupHandle_t   g_ble_event_group_handle;
#endif

/******************************************************************************
 Macro definitions
*******************************************************************************/

#define BLE_TX_BUFSIZ           (80)
#define BLE_UART_INSTANCE       (g_uart0)
#define FIFO_DEPTH              (64)

/******************************************************************************
 Function prototype declarations
*******************************************************************************/

void console_register_event_cb(ble_cli_event_cb_t cb);
static void check_uart(void);
static uint8_t read_fifo(void);
static int8_t write_fifo(uint8_t dat);
static int8_t check_fifo(void);
static void check_uart(void);

/******************************************************************************
 Global variables
*******************************************************************************/

typedef enum {
    BLE_CLI_STATE_NORMAL,
    BLE_CLI_STATE_ESC,
    BLE_CLI_STATE_ESC_SEQ,
} e_ble_cli_state_t;

typedef struct {
    uint8_t wp;
    bool wflg;
    uint8_t rp;
    bool rflg;
    uint8_t buf [FIFO_DEPTH];
} st_uart_fifo_t;

static volatile bool        g_cli_tx_flg;
static volatile bool        g_cli_rx_flg;
static volatile uint8_t     g_state;
static volatile uint8_t     g_UartDone;
static volatile uint8_t     g_UartReq;
static uint8_t              b[BLE_TX_BUFSIZ];
static ble_cli_event_cb_t   gs_cli_event_cb;
static st_uart_fifo_t       gs_uart_fifo;


/******************************************************************************
 * Function Name: user_uart_callback_ble_cli
 * Description  : Callback function for UART.
 *                The name of this function must be the same as the name
 *                specified FSP configuration.
 * Arguments    : uart_callback_args_t *p_args - Callback parameter
 * Return Value : none
 ******************************************************************************/
void user_uart_callback_ble_cli(uart_callback_args_t *p_args)
{
    switch(p_args->event)
    {
        case UART_EVENT_RX_CHAR:
            g_UartReq = 1U;
            g_cli_rx_flg = true;
            check_uart();
            break;

        case UART_EVENT_ERR_OVERFLOW:
            break;

        case UART_EVENT_TX_COMPLETE:
            g_UartDone = 1U;
            g_cli_tx_flg = false;
            break;

        case UART_EVENT_ERR_FRAMING:
            g_UartReq = 1U;
            g_cli_rx_flg = true;
            check_uart();
            break;

        default:
            break;
    }

#if (BSP_CFG_RTOS == 1)
    if( NULL != gs_cli_event_cb )
    {
        /* callback SCI event */
        gs_cli_event_cb((void *)g_ble_event_group_handle);
    }
#elif (BSP_CFG_RTOS == 2)
    if( NULL != gs_cli_event_cb )
    {
        /* callback SCI event */
        gs_cli_event_cb((void *)g_ble_event_group_handle);
    }
#endif
}

bool get_uart_reception(void)
{
    return g_cli_rx_flg;
}

void set_uart_reception(bool flag)
{
    g_cli_rx_flg = flag;
}

bool get_uart_transmission(void)
{
    return g_cli_tx_flg;
}

void set_uart_transmission(bool flag)
{
    g_cli_tx_flg = flag;
}

/*********************************************************************************************************************
 * Function Name: process_normal
 * Description  : Process an input character.
 * Arguments    : c - Input character code
 * Return Value : Character
 ********************************************************************************************************************/
static uint8_t process_normal(uint8_t c)
{
    uint8_t ch = 0x00;

    switch (c)
    {
        case KEY_ESC:
        {
            g_state = BLE_CLI_STATE_ESC;
        } break;

        default:
        {
            ch = c;
        } break;
    }

    return ch;
}

/*********************************************************************************************************************
 * Function Name: process_escape
 * Description  : Process an escape character.
 * Arguments    : c - Input escape character
 * Return Value : 0x00
 ********************************************************************************************************************/
static uint8_t process_escape(uint8_t c)
{
    g_state = BLE_CLI_STATE_NORMAL;

    if (c == '[')
    {
        g_state = BLE_CLI_STATE_ESC_SEQ;
    }

    return 0x00;
}

/*********************************************************************************************************************
 * Function Name: process_escape_sequence
 * Description  : Process an escape sequence.
 * Arguments    : c - Input escape sequence
 * Return Value : KEY_UP or KEY_DOWN or KEY_RIGHT or KEY_LEFT or 0x00
 ********************************************************************************************************************/
static uint8_t process_escape_sequence(uint8_t c)
{
    uint8_t ch = 0x00;

    g_state = BLE_CLI_STATE_NORMAL;

    switch (c)
    {
        case 'A':
        {
            ch = KEY_UP;
        } break;

        case 'B':
        {
            ch = KEY_DOWN;
        } break;

        case 'C':
        {
            ch = KEY_RIGHT;
        } break;

        case 'D':
        {
            ch = KEY_LEFT;
        } break;

        default:
            break;
    }

    return ch;
}

/*********************************************************************************************************************
 * Function Name: console_init
 * Description  : Initialize console.
 * Arguments    : none
 * Return Value : none
 ********************************************************************************************************************/
void console_init(void)
{
    g_cli_tx_flg = false;
    g_cli_rx_flg = false;

    g_UartDone = 1U;
    g_UartReq = 0U;
    BLE_UART_INSTANCE.p_api->open(BLE_UART_INSTANCE.p_ctrl, BLE_UART_INSTANCE.p_cfg);
    gs_uart_fifo = (st_uart_fifo_t){0};
}

/*********************************************************************************************************************
 * Function Name: console_register_cb
 * Description  : Register console callback function
 * Arguments    : cb       - callback function pointer
 * Return Value : none
 ********************************************************************************************************************/
void console_register_event_cb(ble_cli_event_cb_t cb)
{
    gs_cli_event_cb = cb;
}

/*********************************************************************************************************************
 * Function Name: console_terminate
 * Description  : Terminate console.
 * Arguments    : none
 * Return Value : none
 ********************************************************************************************************************/
void console_terminate(void)
{
    BLE_UART_INSTANCE.p_api->close(BLE_UART_INSTANCE.p_ctrl);
}

/*********************************************************************************************************************
 * Function Name: console_getc
 * Description  : Process a received character.
 * Arguments    : p_c      - Received character code
 *              : p_escape - Escape sequence state 
 * Return Value : Character processing state.
 ********************************************************************************************************************/
bool console_getc(uint8_t *p_c, bool *p_escape)
{
    uint8_t ch = 0x00;
    *p_c = 0x00;

    if (0 < check_fifo()){

        ch = read_fifo();

        switch (g_state)
        {
            case BLE_CLI_STATE_NORMAL:
            {
                *p_escape = false;
                *p_c      = process_normal(ch);
            } break;

            case BLE_CLI_STATE_ESC:
            {
                *p_escape = false;
                *p_c      = process_escape(ch);
            } break;

            case BLE_CLI_STATE_ESC_SEQ:
            {
                *p_escape = true;
                *p_c      = process_escape_sequence(ch);
            } break;

            default:
                break;

        }
    }

    return (bool)((*p_c) != 0x00);
}

/*********************************************************************************************************************
 * Function Name: console_putsf
 * Description  : Output debug message.
 * Arguments    : p_format      - Debug message
 * Return Value : none
 ********************************************************************************************************************/
void console_putsf(const char *p_format, ...)
{
    va_list va;
    fsp_err_t   ret;
    uint16_t    len;

    memset(b, 0x00, BLE_TX_BUFSIZ);
    va_start(va, p_format);
    vsnprintf((char *)b, BLE_TX_BUFSIZ, p_format, va);
    va_end(va);
    len = (uint16_t)strlen((const char *)b);

    /* TODO: Remove this after support UART DMA transmission. */
    if (len && (g_UartDone != 0))
    {
        do
        {
            g_UartDone = 0U;
            ret = BLE_UART_INSTANCE.p_api->write(BLE_UART_INSTANCE.p_ctrl, b, len);

            if( ret == FSP_SUCCESS )
                g_cli_tx_flg = true;
#if (BSP_CFG_RTOS == 1)
            tx_thread_sleep (1);
#elif (BSP_CFG_RTOS == 2)
           vTaskDelay (1 / portTICK_PERIOD_MS);
#endif
        } while ((ret == FSP_ERR_INSUFFICIENT_SPACE));

        while(0 == g_UartDone){
#if (BSP_CFG_RTOS == 1)
            tx_thread_sleep (1);
#elif (BSP_CFG_RTOS == 2)
            vTaskDelay (1 / portTICK_PERIOD_MS);
#endif
        }
    }
}

static uint8_t read_fifo(void)
{

    if ((gs_uart_fifo.wflg==gs_uart_fifo.rflg)&&(gs_uart_fifo.wp==gs_uart_fifo.rp)){
            return 0xFF;
        }
    uint8_t ret = gs_uart_fifo.buf[gs_uart_fifo.rp];

    if (gs_uart_fifo.rp==FIFO_DEPTH - 1) {
        gs_uart_fifo.rflg = !gs_uart_fifo.rflg;
        gs_uart_fifo.rp = 0;
    } else {
        gs_uart_fifo.rp++;
    }

    return ret;
}

static int8_t write_fifo(uint8_t dat)
{

    if ((gs_uart_fifo.wflg!=gs_uart_fifo.rflg)&&(gs_uart_fifo.wp==gs_uart_fifo.rp)){
        return -1;
    }

    gs_uart_fifo.buf[gs_uart_fifo.wp] = dat;
    if (gs_uart_fifo.wp==FIFO_DEPTH-1) {
        gs_uart_fifo.wflg = !gs_uart_fifo.wflg;
        gs_uart_fifo.wp = 0;
    } else {
        gs_uart_fifo.wp++;
    }

    return 0;
}

static int8_t check_fifo(void)
{
    if ((gs_uart_fifo.wflg==gs_uart_fifo.rflg)&&(gs_uart_fifo.wp>=gs_uart_fifo.rp)) {
        return (int8_t)(gs_uart_fifo.wp-gs_uart_fifo.rp);
    } else if ((gs_uart_fifo.wflg!=gs_uart_fifo.rflg)&&(gs_uart_fifo.wp<=gs_uart_fifo.rp)) {
        return (int8_t)((gs_uart_fifo.wp+FIFO_DEPTH)-gs_uart_fifo.rp);
    } else {
        return -1;
    }
}

static void check_uart(void)
{

    if (g_UartReq){
        g_UartReq = 0U;

        switch(BLE_UART_INSTANCE.p_cfg->channel)
        {
            case 0:
                write_fifo(R_SCI0->RDR);
                break;

            case 1:
                write_fifo(R_SCI1->RDR);
                break;

            case 4:
                write_fifo(R_SCI4->RDR);
                break;

            case 9:
                write_fifo(R_SCI9->RDR);
                break;

            default:
                /* Do nothing */
                break;

        }
    }
}

#else /* (BLE_CFG_CMD_LINE_EN == 1) */

/*********************************************************************************************************************
 * Function Name: console_putsf
 * Description  : Dummy function
 * Arguments    : p_format      - Debug message
 * Return Value : none
 ********************************************************************************************************************/
void console_putsf(const char *p_format, ...)
{
    (void)p_format;
}
#endif /* (BLE_CFG_CMD_LINE_EN == 1) */


