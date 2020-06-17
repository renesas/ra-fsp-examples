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