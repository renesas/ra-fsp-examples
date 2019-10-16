
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
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
