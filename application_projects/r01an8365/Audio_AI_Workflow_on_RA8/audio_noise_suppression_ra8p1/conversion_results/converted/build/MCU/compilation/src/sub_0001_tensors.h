#ifndef __SUB_0001_TENSORS_H__
#define __SUB_0001_TENSORS_H__

#include <stddef.h>
#include <stdint.h>
#include "ethosu_common.h"

extern const TensorInfo sub_0001_tensors[];
extern const size_t sub_0001_tensors_count;

#define kArenaSize_sub_0001 1024

// Addresses for each input and output buffer inside of the arena
extern const uint32_t sub_0001_address_denoise_gru_prev_state_int8;
extern const uint32_t sub_0001_address_vad_gru_prev_state_int8;
extern const uint32_t sub_0001_address_Tensordot_Reshape_model_1_fc1_StatefulPartitionedCall_Tensordot_Reshape_70039;
extern const uint32_t sub_0001_address_noise_gru_prev_state_int8;
extern const uint32_t sub_0001_address_main_input_int8;
extern const uint32_t sub_0001_address_Identity_3_int8_70089;
extern const uint32_t sub_0001_address_Identity_2_int8_70118;
extern const uint32_t sub_0001_address_Identity_int8_70148;
extern const uint32_t sub_0001_address_Identity_4_int8_70156;
extern const uint32_t sub_0001_address_Identity_1_int8_70152;


#endif // __SUB_0001_TENSORS_H__
