#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "common_data.h"

#include "sub_0001_tensors.h"
#include "sub_0001_command_stream.h"
#include "sub_0001_model_data.h"

#include "sub_0001_invoke.h"

// Include Ethos-U driver headers (Assumed to be available)
#include "ethosu_driver.h"

// Define arenas with allocation and 16-byte alignment
__attribute__((aligned(16))) uint8_t sub_0001_arena[1024];
// Fast scratch arena not used for Ethos-U55
//  We will not create it for now and reuse the address of the other arena
// __attribute__((aligned(16))) static uint8_t sub_0001_fast_scratch[1024];
uint8_t* sub_0001_fast_scratch = sub_0001_arena;

int sub_0001_invoke(bool clean_outputs) {
  // Initialize base addresses and sizes
  uint64_t base_addrs[13] = {0};
  size_t base_addrs_size[13] = {0};
  int num_base_addrs = 13;

  // Variables for command stream
  uint8_t* cms_data = NULL;
  int cms_size = 0;

  // Prepare base_addrs and base_addrs_size arrays
  // Buffer sub_0001_model with size 117440 and address: 4294967295
  base_addrs[0] = (uint64_t)(uintptr_t)sub_0001_model_data;
  base_addrs_size[0] = sub_0001_model_data_size;
  // Buffer sub_0001_arena with size 1024 and address: 0
  base_addrs[1] = (uint64_t)(uintptr_t) (sub_0001_arena+0);
  base_addrs_size[1] = 1024;

  // Buffer sub_0001_fast_scratch with size 1024 and address: 0
  base_addrs[2] = (uint64_t)(uintptr_t) (sub_0001_arena+0);
  base_addrs_size[2] = 1024;

  // Buffer input_tensor_0 with size 96 and address: 0
  base_addrs[3] = (uint64_t)(uintptr_t) (sub_0001_arena+0);
  base_addrs_size[3] = 96;

  // Buffer input_tensor_1 with size 24 and address: 384
  base_addrs[4] = (uint64_t)(uintptr_t) (sub_0001_arena+384);
  base_addrs_size[4] = 24;

  // Buffer input_tensor_2 with size 42 and address: 592
  base_addrs[5] = (uint64_t)(uintptr_t) (sub_0001_arena+592);
  base_addrs_size[5] = 42;

  // Buffer input_tensor_3 with size 48 and address: 464
  base_addrs[6] = (uint64_t)(uintptr_t) (sub_0001_arena+464);
  base_addrs_size[6] = 48;

  // Buffer input_tensor_4 with size 42 and address: 416
  base_addrs[7] = (uint64_t)(uintptr_t) (sub_0001_arena+416);
  base_addrs_size[7] = 42;

  // Buffer output_tensor_0 with size 24 and address: 384
  if (clean_outputs) {
    memset(sub_0001_arena + 384, 0, 24);
  }
  base_addrs[8] = (uint64_t)(uintptr_t) (sub_0001_arena+384);
  base_addrs_size[8] = 24;

  // Buffer output_tensor_1 with size 48 and address: 464
  if (clean_outputs) {
    memset(sub_0001_arena + 464, 0, 48);
  }
  base_addrs[9] = (uint64_t)(uintptr_t) (sub_0001_arena+464);
  base_addrs_size[9] = 48;

  // Buffer output_tensor_2 with size 96 and address: 0
  if (clean_outputs) {
    memset(sub_0001_arena + 0, 0, 96);
  }
  base_addrs[10] = (uint64_t)(uintptr_t) (sub_0001_arena+0);
  base_addrs_size[10] = 96;

  // Buffer output_tensor_3 with size 1 and address: 96
  if (clean_outputs) {
    memset(sub_0001_arena + 96, 0, 1);
  }
  base_addrs[11] = (uint64_t)(uintptr_t) (sub_0001_arena+96);
  base_addrs_size[11] = 1;

  // Buffer output_tensor_4 with size 22 and address: 208
  if (clean_outputs) {
    memset(sub_0001_arena + 208, 0, 22);
  }
  base_addrs[12] = (uint64_t)(uintptr_t) (sub_0001_arena+208);
  base_addrs_size[12] = 22;

  // Command stream data
  cms_data = (uint8_t*)sub_0001_command_stream;
  cms_size = (int) sub_0001_command_stream_size;

  // Invoke the Ethos-U driver
  if (num_base_addrs > 8) {
    num_base_addrs = 8;
  }
  int result = ethosu_invoke_v3(&g_ethosu0, cms_data, cms_size, base_addrs, base_addrs_size, num_base_addrs, NULL);

  if (result == -1) {
    // Ethos-U invocation failed
    return -1;
  }

  return 0;
}
