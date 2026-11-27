/***********************************************************************************************************************
 * File Name    : ssi_ep.h
 * Description  : Contains declarations of data structures and functions used in ssi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SSI_EP_H_
#define SSI_EP_H_

#include <stdbool.h>
#include "hal_data.h"

#define BUFF_SIZE                                   (0x800U)
#define SSI_STREAMING_EXAMPLE_TONE_FREQUENCY_HZ     (800)
#define SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK     (1024)
#define MAX_TIME                                    (0xffffU)
#define RESET_VALUE                                 (0x00)

#ifndef M_PI
#define M_PI                                        (3.14159265358979323846)
#endif

/* Extern variables — defined in ssi_ep.c */
extern volatile i2s_event_t g_i2s_event;
extern volatile bool g_send_data_in_main_loop;
extern volatile uint8_t g_buffer_index;
extern uint8_t g_dest_buff[BUFF_SIZE];
extern int16_t g_src_buff[2][SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK];

/* Function declarations */
fsp_err_t ssi_init(void);
fsp_err_t gpt_init(void);
void ssi_example_calculate_samples(uint32_t buffer_index);
fsp_err_t ssi_example_write(void);
void deinit_ssi(void);
void deinit_gpt(void);

#endif /* SSI_EP_H_ */
