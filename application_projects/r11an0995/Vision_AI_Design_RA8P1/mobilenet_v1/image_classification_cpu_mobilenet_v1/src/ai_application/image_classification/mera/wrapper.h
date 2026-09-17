/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
#ifndef MODEL_WRAPPER_H
#define MODEL_WRAPPER_H

#include <compute_sub_0000_net1.h>
#include <stdint.h>
#include <stdbool.h>



int8_t output_buffer[1000];

static inline uint64_t mera_input_size() {
    return 150528;
}

static inline uint64_t mera_output_size() {
    return 1000;
}


#endif // MODEL_WRAPPER_H
