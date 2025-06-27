/***********************************************************************************************************************
 * File Name    : audio_data.h
 * Description  : Contains macros, data buffers used in audio_data.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef AUDIO_DATA_H_
#define AUDIO_DATA_H_

#define BUF_ROW_SIZE     (4)
#define BUF_FIFO_STAGE   (64)
#define INDEX            (0)
extern uint32_t g_buf_row_index;

/* Destination buffer to receive the sample audio data */
extern uint8_t g_dest_buff[BUF_ROW_SIZE][BUF_FIFO_STAGE];

/* Source buffer to transmit the sample audio data */
extern const uint8_t g_src_buff[BUF_ROW_SIZE][BUF_FIFO_STAGE];
extern const uint8_t g_src_buff1[BUF_ROW_SIZE][BUF_FIFO_STAGE];

#endif /* AUDIO_DATA_H_ */
