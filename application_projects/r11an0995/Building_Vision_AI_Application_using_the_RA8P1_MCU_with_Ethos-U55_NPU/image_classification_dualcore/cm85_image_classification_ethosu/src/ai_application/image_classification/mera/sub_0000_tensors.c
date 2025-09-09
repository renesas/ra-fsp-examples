#include "sub_0000_tensors.h"

const TensorInfo sub_0000_tensors[] = {
  { "_split_1_command_stream", 1, 5008, "COMMAND_STREAM", 0xffffffff },
  { "_split_1_flash", 2, 505776, "MODEL", 0xffffffff },
  { "_split_1_scratch", 3, 401408, "ARENA", 0x0 },
  { "_split_1_scratch_fast", 4, 401408, "FAST_SCRATCH", 0x0 },
  { "serving_default_input_1_0", 5, 150528, "INPUT_TENSOR", 0x31000 },
  { "StatefulPartitionedCall_0_70098", 0, 1000, "OUTPUT_TENSOR", 0x0 },
};

const size_t sub_0000_tensors_count = sizeof(sub_0000_tensors) / sizeof(sub_0000_tensors[0]);

// Addresses for each input and output buffer inside of the arena
const uint32_t sub_0000_address_serving_default_input_1_0 = 0x31000;
const uint32_t sub_0000_address_StatefulPartitionedCall_0_70098 = 0x0;

