
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ecc.h"
#include "crypto.h"
#include "cryptoHelpers.h"


#if defined (_MSC_VER)
#include <windows.h>
#include <wincrypt.h>
#endif

// KDF2 is the KDF defined in ISO-18033-2
// (see http://www.di-mgt.com.au/cryptoKDFs.html#kdf2  for an explanation)
// we use SHA256 as the hash algorithm
 bool KDF2(const uint8_t secret[uECC_BYTES], uint32_t requiredLengthInBytes, uint8_t *pOutput)
{
    uint8_t     hvalue[uECC_BYTES + sizeof(uint32_t)];
    uint8_t    *counter = hvalue + uECC_BYTES + 3;
    uint32_t    needed = requiredLengthInBytes;
    uint8_t     hv[SHA256_SIZE];

    memset(hvalue, 0, sizeof(hvalue));
    memcpy(hvalue, secret, uECC_BYTES);
    *counter = 1;
    while (needed >= SHA256_SIZE)
    {
        sha256_calc_hash(hvalue, uECC_BYTES + sizeof(uint32_t), pOutput + ((*counter - 1) * SHA256_SIZE));
        *counter = (uint8_t) (*counter + 1);
        needed -= SHA256_SIZE;
    }
    if (needed != 0)
    {
        sha256_calc_hash(hvalue, uECC_BYTES + sizeof(uint32_t), hv);
        memcpy(pOutput + ((*counter - 1) * SHA256_SIZE), hv, needed);
        *counter = (uint8_t) (*counter + 1);
    }
    return true;
}



#ifndef ENABLE_HW_RNG

 // ECC sign needs a random number....
 // Until we get TSIP a pseudo is needed. This one is supposed to be good, but
 // given we have NOT got anything to seed it from we will always get the same sequence.
 //
 //http://www0.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf
 //
 /* Implementation of a 32-bit KISS generator which uses no multiply instructions */
 static unsigned int x = 123456789, y = 234567891, z = 345678912, w = 456789123, c = 0;
 static unsigned int JKISS32(void)
 {
     int t;
     y ^= (y << 5);
     y ^= (y >> 7);
     y ^= (y << 22);
     t = (int) (z + w + c);
     z = w;
     c = t < 0;
     w = t & 2147483647;
     x += 1411392427;
     return x + y + w;
 }


 int our_RNG(uint8_t *dest, unsigned size)
 {
#if defined (_MSC_VER)
     HCRYPTPROV prov;
     if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
     {
         return false;
     }

     CryptGenRandom(prov, size, (BYTE *)dest);
     CryptReleaseContext(prov, 0);
     return true;
#else
     char *ptr = (char *)dest;
     size_t left = size;
     size_t amount;
     uint32_t rnd;

     while (left > 0) {
         rnd = JKISS32();
         amount = left>4 ? 4 : left;

         memcpy(ptr, (void*)&rnd, amount);
         left -= amount;
         ptr += amount;
     }
     return true;
#endif
 }
#endif //ENABLE_HW_RNG

#ifndef ENABLE_HW_ECC
 void cryptoPrepareRandomGenerator(void)
 {
#if !defined (_MSC_VER)
     uECC_set_rng(our_RNG);
#endif
 }

#endif

 /********************************************************************************************
  *
  * Function: crypto_rand()
  *
  * Description:
  *   Wrapper function for the HW or SW based random number generation function.
  *
  * Parameters: uint8_t *dest_buffer   - pointer to output buffer
  *             unsigned buffer_bytes  - number of bytes of random data to generate
  *
  * Return value: Number of random bytes generated.
  *
  **********************************************************************************************/
 int crypto_rand(uint8_t *dest_buffer, unsigned buffer_bytes)
 {
      return (our_RNG(dest_buffer, buffer_bytes));
 }


