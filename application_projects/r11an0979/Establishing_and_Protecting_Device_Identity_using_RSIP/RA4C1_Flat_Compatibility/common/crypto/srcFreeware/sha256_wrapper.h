

#include <stdint.h>
#if defined (_MSC_VER)
#else
#include <stdbool.h>
#endif

#ifndef COMMON_API_SHA256_WRAPPER_H_
#define COMMON_API_SHA256_WRAPPER_H_

extern bool sha256_calc_hash(const uint8_t *pData, uint32_t length, uint8_t *pHash);

#endif /* COMMON_API_SHA256_WRAPPER_H_ */
