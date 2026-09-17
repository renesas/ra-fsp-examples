/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef MODEL_WRAPPER_H
#define MODEL_WRAPPER_H

#include "model.h"

#include <stdint.h>
#include <stdbool.h>

static inline uint8_t* mera_input_ptr() {
    return (uint8_t*) GetModelInputPtr_serving_default_input_1_0();
}

static inline uint8_t* mera_output_ptr() {
    return (uint8_t*) GetModelOutputPtr_StatefulPartitionedCall_0_70098();
}

static inline uint64_t mera_input_size() {
    return 150528;
}

static inline uint64_t mera_output_size() {
    return 1000;
}


static inline void mera_invoke() {

    RunModel(false);
}

#endif // MODEL_WRAPPER_H
