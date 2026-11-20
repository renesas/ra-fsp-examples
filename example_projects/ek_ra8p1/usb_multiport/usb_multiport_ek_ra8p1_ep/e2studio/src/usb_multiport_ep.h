/***********************************************************************************************************************
 * File Name    : usb_multiport_ep.h
 * Description  : Contains macros, variables, and functions used in usb_multiport_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_MULTIPORT_EP_H_
#define USB_MULTIPORT_EP_H_

#include <stdbool.h>
#include "usb_hmsc_thread.h"
#include "usb_pcdc_thread.h"

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macro for the console input buffer size */
#define BUFFER_SIZE             (16U)

/* Macro for the USB media inserted state */
#define MEDIA_INSERTED          (1U << 1)

/* Macro for the USB media removed state */
#define MEDIA_REMOVED           (1U << 0)

/* Macros for the console input selection */
#define USB_READ                (1U)
#define USB_WRITE               (2U)

/* Macro for the file name */
#define FILE_NAME               "test.txt"

/* Macro for the file write mode */
#define WRITE_MODE              "w"

/* Macro for the file read mode */
#define READ_MODE               "r"

/* Macro for the number of elements to read/write */
#define WRITE_ITEM_SIZE         (512U)

/* Macro for the read buffer size - Also the item size of the data queues */
#define READ_BUF_SIZE           (512U)

/* Macros for operation results */
#define READ_FAILURE            (0)
#define FF_FS_ADD_FAILURE       (0)

/* Macro for the USB detection wait time in milliseconds */
#define WAIT_TIME               (4000)

/* Macro for the carriage return character */
#define CARRIAGE_RETURN         ('\r')

/* Macro for the line coding length */
#define LINE_CODING_LENGTH      (0x07U)

/* External variables - Defined in usb_multiport_ep.c */
extern volatile bool g_write_to_usb;
extern uint16_t g_bytes_to_write;
extern volatile usb_event_info_t * gp_usb_pcdc_event;

/* Function declarations */
fsp_err_t fat_init(void);
fsp_err_t fat_deinit(void);
fsp_err_t fat_disk_init(void);
fsp_err_t fat_disk_deinit(void);
fsp_err_t usb_init(void);
fsp_err_t usb_read(usb_ctrl_t * const p_api_ctrl, uint8_t * p_buf, uint32_t size, uint8_t destination);
fsp_err_t usb_write(usb_ctrl_t * const p_api_ctrl, uint8_t const * const p_buf, uint32_t size, uint8_t destination);
fsp_err_t usb_pcdc_class_request(void);
void usb_deinit(void);
bool wait_till_usb_detected(void);
bool check_usb_connected(void);
void fill_data_buffer_and_write_to_file(uint8_t const * const p_buf, uint32_t size);

#endif /* USB_MULTIPORT_EP_H_ */
