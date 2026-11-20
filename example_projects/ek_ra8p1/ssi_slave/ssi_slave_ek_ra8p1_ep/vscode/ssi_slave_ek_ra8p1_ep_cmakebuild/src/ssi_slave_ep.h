/***********************************************************************************************************************
 * File Name    : ssi_slave_ep.h
 * Description  : Contains macros, variables, and functions used in ssi_slave_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SSI_SLAVE_EP_H_
#define SSI_SLAVE_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the null character */
#define NULL_CHAR                   ('\0')

/* Macro for the reset value */
#define RESET_VALUE                 (0U)

/* Macros for the operation mode selected by the user */
#define MASTER_WRITE_SLAVE_READ     (1U)
#define MASTER_READ_SLAVE_WRITE     (2U)

/* Macros for the number of read/write transactions to complete an operation */
#define WRITE_COUNTS                (10U)
#define READ_COUNTS                 (10U)

/* Macro for the delay between successive read/write polls, in microseconds */
#define WAIT_TIME                   (200U)

/* Macro for the size of the command input buffer */
#define BUFFER_SIZE                 (16U)

/* Macro for the number of buffer rows */
#define BUF_ROW_SIZE                (4U)

/* Macro for the number of FIFO entries per row */
#define BUF_FIFO_STAGE              (64U)

/* Macro for the initial buffer row index */
#define INDEX                       (0U)

/* Extern variables - defined in ssi_slave_ep.c */
extern volatile i2s_event_t g_i2s_event;
extern uint32_t g_buf_row_index;
extern uint8_t g_selection_value;
extern uint16_t g_read_counter;
extern uint16_t g_write_counter;
extern volatile bool g_read_in_cb;
extern volatile bool g_send_data_in_main_loop;
extern uint8_t g_dest_buff[BUF_ROW_SIZE][BUF_FIFO_STAGE];

/* Function declarations */
fsp_err_t ssi_init(void);
void ssi_deinit(void);
fsp_err_t ssi_read(uint32_t buf_row_index);
fsp_err_t ssi_write(uint32_t buf_row_index);

#endif /* SSI_SLAVE_EP_H_ */
