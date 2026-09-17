#include "sub_0000_tensors.h"

const TensorInfo sub_0000_tensors[] = {
  { "_split_1_command_stream", 1, 8376, "COMMAND_STREAM", 0xffffffff },
  { "_split_1_flash", 2, 4672224, "MODEL", 0xffffffff },
  { "_split_1_scratch", 3, 1505280, "ARENA", 0x0 },
  { "_split_1_scratch_fast", 4, 1505280, "FAST_SCRATCH", 0x0 },
  { "images", 5, 150528, "INPUT_TENSOR", 0xc4000 },
  { "Softmax_70164", 0, 1000, "OUTPUT_TENSOR", 0x3f0 },
};

const size_t sub_0000_tensors_count = sizeof(sub_0000_tensors) / sizeof(sub_0000_tensors[0]);

// Addresses for each input and output buffer inside of the arena
const uint32_t sub_0000_address_images = 0xc4000;
const uint32_t sub_0000_address_Softmax_70164 = 0x3f0;

