#include "sub_0000_tensors.h"

const TensorInfo sub_0000_tensors[] = {
  { "_split_1_command_stream", 2, 11252, "COMMAND_STREAM", 0xffffffff },
  { "_split_1_flash", 3, 422048, "MODEL", 0xffffffff },
  { "_split_1_scratch", 4, 442368, "ARENA", 0x0 },
  { "_split_1_scratch_fast", 5, 442368, "FAST_SCRATCH", 0x0 },
  { "image_input", 6, 36864, "INPUT_TENSOR", 0x24000 },
  { "Identity_1_70284", 0, 2592, "OUTPUT_TENSOR", 0x5580 },
  { "Identity_70275", 1, 648, "OUTPUT_TENSOR", 0xd80 },
};

const size_t sub_0000_tensors_count = sizeof(sub_0000_tensors) / sizeof(sub_0000_tensors[0]);

// Addresses for each input and output buffer inside of the arena
const uint32_t sub_0000_address_image_input = 0x24000;
const uint32_t sub_0000_address_Identity_1_70284 = 0x5580;
const uint32_t sub_0000_address_Identity_70275 = 0xd80;

