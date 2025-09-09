
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "sha256_wrapper.h"
#include "sha256.h"


bool sha256_calc_hash(const uint8_t *pData, uint32_t length, uint8_t *pHash)
{
	SHA256Context 	shaContext;
	int 			shaStatus;

	if (shaSuccess != (shaStatus = SHA256Reset(&shaContext)))
	{
		printf("[e] SHA256Reset failed %d\n", shaStatus);
	}
	else if (shaSuccess != (shaStatus = SHA256Input(&shaContext, pData, length)))
	{
		printf("[e] SHA256Input failed %d\n", shaStatus);
	}
	else if (shaSuccess != (shaStatus = SHA256FinalBits(&shaContext, 0, 0)))
	{
		printf("[e] SHA256FinalBits failed %d\n", shaStatus);
	}
	else if (shaSuccess != (shaStatus = SHA256Result(&shaContext, pHash)))
	{
		printf("[e] SHA256Result failed %d\n", shaStatus);
	}
	else
	{
		return true;
	}
	return false;
}

