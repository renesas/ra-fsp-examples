#ifndef __ETHOSU_COMMON_H__
#define __ETHOSU_COMMON_H__

#include <stddef.h>
#include <stdint.h>

typedef struct {
  const char* name;
  int32_t tensor_index;
  size_t size;
  const char* region;
  uint32_t address;
} TensorInfo;

#endif // __ETHOSU_COMMON_H__
