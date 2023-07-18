/***********************************************************************************************************************
 * File Name    : audio_data.h
 * Description  : Contains macros, data buffers used in audio_data.h
 ***********************************************************************************************************************/
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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
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
