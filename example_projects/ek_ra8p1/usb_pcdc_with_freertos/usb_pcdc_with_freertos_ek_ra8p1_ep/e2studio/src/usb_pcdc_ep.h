/***********************************************************************************************************************
 * File Name    : usb_pcdc_ep.h
 * Description  : Contains macros, variables, data structures and functions used in usb_pcdc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PCDC_EP_H_
#define USB_PCDC_EP_H_

#include <stdbool.h>
#include "usb_pcdc_thread.h"

/* Macro for the reset value */
#define RESET_VALUE                                     (0x00)

/* Macros for application configuration */
#define INVALID_SIZE                                    (0U)
#define BAUD_ERROR_RATE                                 (5000U)
#define SCI_UART_INVALID_16BIT_PARAM                    (0xFFFFU)

/* Macro for USB CDC line coding length */
#define LINE_CODING_LENGTH                              (0x07U)

/* Macro for USB data buffer length */
#define DATA_LEN                                        (512U)

/* Macros based on the SCI version */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
typedef sci_b_baud_setting_t app_baud_setting_t;
typedef sci_b_uart_extended_cfg_t app_uart_extended_cfg_t;
#define SCI_UART_OPEN(ctrl, cfg)                        (R_SCI_B_UART_Open((ctrl), (cfg)))
#define SCI_UART_WRITE(ctrl, src, byte)                 (R_SCI_B_UART_Write((ctrl), (src), (byte)))
#define SCI_UART_BAUD_CAL(baud, bitrate, error, set)    (R_SCI_B_UART_BaudCalculate((baud), (bitrate), (error), (set)))
#define SCI_UART_CLOSE(ctrl)                            (R_SCI_B_UART_Close(ctrl))
#elif (BSP_PERIPHERAL_SCI_PRESENT)
typedef baud_setting_t app_baud_setting_t;
typedef sci_uart_extended_cfg_t app_uart_extended_cfg_t;
#define SCI_UART_OPEN(ctrl, cfg)                        (R_SCI_UART_Open((ctrl), (cfg)))
#define SCI_UART_WRITE(ctrl, src, byte)                 (R_SCI_UART_Write((ctrl), (src), (byte)))
#define SCI_UART_BAUD_CAL(baud, bitrate, error, set)    (R_SCI_UART_BaudCalculate((baud), (bitrate), (error), (set)))
#define SCI_UART_CLOSE(ctrl)                            (R_SCI_UART_Close(ctrl))
#endif /* SCI version */

/* Application error notifications passed from callback to the main application */
typedef enum
{
    APP_ERROR_NONE = 0,
    APP_ERROR_USB_TX_SEMAPHORE,
    APP_ERROR_USB_READ,
    APP_ERROR_USB_CONTROL_DATA_GET,
    APP_ERROR_USB_CONTROL_DATA_SET,
    APP_ERROR_USB_CONTROL_STATUS_SET,
    APP_ERROR_UART_BAUD_CALCULATE,
    APP_ERROR_UART_INIT,
    APP_ERROR_UART_RX_QUEUE,
    APP_ERROR_UART_TX_SEMAPHORE,
    APP_ERROR_UART_PARITY,
    APP_ERROR_UART_FRAMING,
    APP_ERROR_UART_OVERFLOW,
    APP_ERROR_UART_BREAK_DETECT
} app_error_t;

/* USB event notifications passed to the main application for status reporting */
typedef enum
{
    APP_USB_EVENT_NONE = 0,
    APP_USB_EVENT_READ_COMPLETE,
    APP_USB_EVENT_WRITE_COMPLETE,
    APP_USB_EVENT_CONFIGURED,
    APP_USB_EVENT_GET_LINE_CODING,
    APP_USB_EVENT_SET_LINE_CODING_REQUEST,
    APP_USB_EVENT_SET_LINE_CODING_COMPLETE,
    APP_USB_EVENT_SET_CONTROL_LINE_STATE,
    APP_USB_EVENT_DETACH,
    APP_USB_EVENT_SUSPEND,
    APP_USB_EVENT_RESUME
} app_usb_event_t;

/* Identifies the type of event stored in the application event queue */
typedef enum
{
    PERIPHERAL_NONE = 0,
    PERIPHERAL_USB,         /* USB data transfer event */
    PERIPHERAL_UART,        /* UART data transfer event */
    PERIPHERAL_ERROR,       /* USB or UART error notification */
    PERIPHERAL_USB_EVENT    /* USB status event used for application logging */
} peripheral_t;

/* Snapshot of USB PCDC line coding used for deferred status reporting */
typedef struct
{
    uint32_t bitrate;
    uint8_t char_format;
    uint8_t parity_type;
    uint8_t data_bits;
} app_line_coding_info_t;

/* USB event information passed from the USB callback to the main application for status reporting */
typedef struct
{
    app_usb_event_t event;
    app_line_coding_info_t line_coding;
    uint8_t bdtr;
    uint8_t brts;
} app_usb_event_info_t;

/* Event message exchanged between callback and the main application task */
typedef struct
{
    peripheral_t peripheral;
    union
    {
        uint32_t data_size;
        app_error_t error;
        app_usb_event_info_t usb;
    } u;
} queue_evt_t;

/* External variables - Defined in usb_pcdc_ep.c */
extern uint8_t g_user_data_buf[DATA_LEN];
extern volatile usb_pcdc_linecoding_t g_line_coding;
extern uart_cfg_t g_uart_test_cfg;
extern volatile bool g_usb_attach;
extern app_uart_extended_cfg_t g_sci_extend_cfg;

/* Function declarations */
fsp_err_t usb_init(void);
fsp_err_t usb_read(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size, uint8_t destination);
fsp_err_t usb_write(usb_ctrl_t * const p_api_ctrl, uint8_t const * const p_buf, uint32_t size, uint8_t destination);
fsp_err_t usb_peri_control_data_get(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size);
fsp_err_t usb_peri_control_data_set(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size);
fsp_err_t usb_peri_control_status_set(usb_ctrl_t * const p_api_ctrl, usb_setup_status_t status);
void usb_deinit(void);
fsp_err_t uart_init(uart_ctrl_t * const p_api_ctrl, uart_cfg_t const * const p_cfg);
fsp_err_t uart_write(uart_ctrl_t * const p_api_ctrl, uint8_t const * const p_src, uint32_t const bytes);
fsp_err_t uart_baud_calculate(uint32_t baudrate, bool bitrate_modulation, uint32_t baud_rate_error_x_1000,
                              app_baud_setting_t * const p_baud_setting);
void uart_deinit(void);

#endif /* USB_PCDC_EP_H_ */
