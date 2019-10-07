/***********************************************************************************************************************
* Copyright [2015-2018] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* This file is part of Renesas SynergyTM Software Package (SSP)
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
* Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
* or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
* MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
* "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
* CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
* CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
* included in this file may be subject to different terms.
**********************************************************************************************************************/

/**********************************************************************************************************************
* File Name    : MCUMemory.h
* Description  : Class to represent MCU Memory map
**********************************************************************************************************************/
#ifndef _MCU_MEMORY_H_
#define _MCU_MEMORY_H_

#include <stdint.h>
#include <vector>
#include "flashDescription.h"

#define NUM_REGIONS 2

class MCUMemory
{
public:
	MCUMemory(FlashDescription * pMemoryMap);
	uint32_t GetPageAlignedSize(e_memory_type region_type);
	uint32_t GetPageAlignedAddress(e_memory_type region_type);
	const uint8_t* GetPageAlignedData(e_memory_type region_type);
	bool InsertData(uint32_t start_address, const std::vector<uint8_t>& data_to_insert);
	~MCUMemory();

private:
	typedef struct
	{
		uint32_t region_start_address;
		uint32_t region_end_address;
		uint32_t data_start_address;
		uint32_t data_end_address;
		uint32_t region_page_size;
		uint8_t* p_data;
	}memory_region_t;

	memory_region_t m_regions[NUM_REGIONS];

	void region_create(e_memory_type region_type, FlashDescription *pMemoryMap);
	bool region_insert(e_memory_type region_type, uint32_t start_address, const std::vector<uint8_t>& data_to_insert);
};

#endif //_MCU_MEMORY_H_