/***********************************************************************************************************************
 * File Name    : audio_data.h
 * Description  : Contains macros, data buffers used in audio_data.h
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef AUDIO_DATA_H_
#define AUDIO_DATA_H_

#define BUFF_SIZE         (0x1000U)     //Sample audio buffer size
#define OPERATION_DELAY   (500)         //Time delay between each WriteRead and compare operation.

/* Destination buffer to receive the sample audio data */
extern uint8_t g_dest_buff[BUFF_SIZE];

/* Source buffer to transmit the sample audio data */
extern const uint8_t g_src_buff[BUFF_SIZE];


#endif /* AUDIO_DATA_H_ */
