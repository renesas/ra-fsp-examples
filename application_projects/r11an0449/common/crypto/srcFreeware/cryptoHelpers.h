
#include <stdint.h>
#include <stdbool.h>

#ifndef CRYPTO_HELPERS_H_
#define CRYPTO_HELPERS_H_

#include "ecc.h"

extern bool KDF2(const uint8_t secret[uECC_BYTES], uint32_t requiredLengthInBytes, uint8_t *pOutput);

extern int our_RNG(uint8_t *dest, unsigned size);
extern int crypto_rand(uint8_t *dest_buffer, unsigned buffer_bytes);

extern void cryptoPrepareRandomGenerator(void);

#endif
