#include "sub_0001_tensors.h"

const TensorInfo sub_0001_tensors[] = {
  { "_split_1_command_stream", 6, 4748, "COMMAND_STREAM", 0xffffffff },
  { "_split_1_flash", 7, 117440, "MODEL", 0xffffffff },
  { "_split_1_scratch", 8, 1024, "ARENA", 0x0 },
  { "_split_1_scratch_fast", 9, 1024, "FAST_SCRATCH", 0x0 },
  { "denoise_gru_prev_state_int8", 10, 96, "INPUT_TENSOR", 0x0 },
  { "vad_gru_prev_state_int8", 13, 24, "INPUT_TENSOR", 0x180 },
  { "Tensordot_Reshape_model_1_fc1_StatefulPartitionedCall_Tensordot_Reshape_70039", 5, 42, "INPUT_TENSOR", 0x250 },
  { "noise_gru_prev_state_int8", 12, 48, "INPUT_TENSOR", 0x1d0 },
  { "main_input_int8", 11, 42, "INPUT_TENSOR", 0x1a0 },
  { "Identity_3_int8_70089", 2, 24, "OUTPUT_TENSOR", 0x180 },
  { "Identity_2_int8_70118", 1, 48, "OUTPUT_TENSOR", 0x1d0 },
  { "Identity_int8_70148", 4, 96, "OUTPUT_TENSOR", 0x0 },
  { "Identity_4_int8_70156", 3, 1, "OUTPUT_TENSOR", 0x60 },
  { "Identity_1_int8_70152", 0, 22, "OUTPUT_TENSOR", 0xd0 },
};

const size_t sub_0001_tensors_count = sizeof(sub_0001_tensors) / sizeof(sub_0001_tensors[0]);

// Addresses for each input and output buffer inside of the arena
const uint32_t sub_0001_address_denoise_gru_prev_state_int8 = 0x0;
const uint32_t sub_0001_address_vad_gru_prev_state_int8 = 0x180;
const uint32_t sub_0001_address_Tensordot_Reshape_model_1_fc1_StatefulPartitionedCall_Tensordot_Reshape_70039 = 0x250;
const uint32_t sub_0001_address_noise_gru_prev_state_int8 = 0x1d0;
const uint32_t sub_0001_address_main_input_int8 = 0x1a0;
const uint32_t sub_0001_address_Identity_3_int8_70089 = 0x180;
const uint32_t sub_0001_address_Identity_2_int8_70118 = 0x1d0;
const uint32_t sub_0001_address_Identity_int8_70148 = 0x0;
const uint32_t sub_0001_address_Identity_4_int8_70156 = 0x60;
const uint32_t sub_0001_address_Identity_1_int8_70152 = 0xd0;

