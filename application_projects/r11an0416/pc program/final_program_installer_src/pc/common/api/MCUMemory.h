/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

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