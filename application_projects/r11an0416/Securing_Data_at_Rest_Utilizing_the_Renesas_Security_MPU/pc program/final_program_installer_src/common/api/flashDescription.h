
/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FLASH_DESCRIPTION_H_
#define FLASH_DESCRIPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdint.h>

	typedef enum
	{
		code_memory,
		data_memory
	}e_memory_type;

    // Record for a flash region
    typedef struct FlashDescriptionRecord_tag {

		// Flash type
		e_memory_type flash_type;

        // start address
        uint32_t startAddr;

        // end address
        uint32_t endAddr;

        // erase block size
        uint32_t eraseBlockSize;

        // write block size
        uint32_t writeBlockSize;

    } FlashDescriptionRecord;


    // The flash description structure
    typedef struct FlashDescription_tag {

        // Number of flash description records
        uint8_t numFlashRecords;

        // Pointer to array of flash description records
        // N.B: these records MUST be in ascending address order for
        //      correct operation of the flash protocol code.
        FlashDescriptionRecord *pFlashRecords;

    } FlashDescription;

#ifdef __cplusplus
}
#endif


#endif /* FLASH_DESCRIPTION_H_ */
