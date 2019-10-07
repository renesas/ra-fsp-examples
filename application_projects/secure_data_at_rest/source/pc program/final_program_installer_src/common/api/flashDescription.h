

/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED ÅeAS ISÅf WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
**********************************************************************************************************************/


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
