#include "MCUMemory.h"
#include <algorithm>

MCUMemory::MCUMemory(FlashDescription * pMemoryMap) :
	m_regions {{UINT32_MAX, 0, UINT32_MAX, 0, 0, NULL},{ UINT32_MAX, 0, UINT32_MAX, 0, 0, NULL}}
{
	region_create(code_memory, pMemoryMap);
	region_create(data_memory, pMemoryMap);
}

MCUMemory::~MCUMemory()
{
	if (NULL != m_regions[code_memory].p_data)
	{
		delete[] m_regions[code_memory].p_data;
	}

	if (NULL != m_regions[data_memory].p_data)
	{
		delete[] m_regions[data_memory].p_data;
	}
}

void MCUMemory::region_create(e_memory_type region_type, FlashDescription *pMemoryMap)
{
	uint32_t num_records = pMemoryMap->numFlashRecords;
	uint32_t page_size = 0;
	FlashDescriptionRecord *pRecords = pMemoryMap->pFlashRecords;
	MCUMemory::memory_region_t *pRegion = &m_regions[region_type];

	/* Setup the memory region */
	for (uint32_t index = 0; index < num_records; index++)
	{
		if (region_type == pRecords[index].flash_type)
		{
			pRegion->region_start_address = std::min(pRegion->region_start_address, pRecords[index].startAddr);
			pRegion->region_end_address = std::max(pRegion->region_end_address, pRecords[index].endAddr);
			pRegion->region_page_size = std::max(pRegion->region_page_size, pRecords[index].writeBlockSize);
		}
	}

	if ((0 != pRegion->region_end_address) &&
		(UINT32_MAX != pRegion->region_start_address))
	{
		uint32_t size = pRegion->region_end_address - pRegion->region_start_address + 1;
		pRegion->p_data = new uint8_t[size];
	}
}

bool MCUMemory::InsertData(uint32_t start_address, const std::vector<uint8_t>& data_to_insert)
{
	/* try inserting in the code memory region */
	bool retval = region_insert(code_memory, start_address, data_to_insert);

	/* Not code, try inserting in the data memory region */
	if (!retval)
	{
		retval = region_insert(data_memory, start_address, data_to_insert);
	}

	return retval;
}

bool MCUMemory::region_insert(e_memory_type region_type, uint32_t start_address, const std::vector<uint8_t>& data_to_insert)
{
	bool retval = false;
	MCUMemory::memory_region_t *pRegion = &m_regions[region_type];
	uint32_t end_address = start_address + data_to_insert.size() - 1;

	if ((start_address >= pRegion->region_start_address) &&
		(end_address <= pRegion->region_end_address) && (NULL != pRegion->p_data))
	{
		for (uint32_t i = 0; i < data_to_insert.size(); i++)
		{
			pRegion->p_data[(start_address - pRegion->region_start_address) + i] = data_to_insert[i];
		}

		pRegion->data_start_address = std::min(start_address, pRegion->data_start_address);
		pRegion->data_end_address = std::max(end_address, pRegion->data_end_address);

		retval = true;
	}

	return retval;
}

const uint8_t*  MCUMemory::GetPageAlignedData(e_memory_type region_type)
{
	MCUMemory::memory_region_t *pRegion = &m_regions[region_type];
	uint8_t* retval = NULL;
	if ((NULL != pRegion->p_data) && 
		(pRegion->data_end_address > pRegion->data_start_address))
	{
		uint32_t start_address_aligned = pRegion->data_start_address & ~(pRegion->region_page_size - 1);
		retval = &pRegion->p_data[start_address_aligned - pRegion->region_start_address];
	}

	return retval;
}

uint32_t MCUMemory::GetPageAlignedSize(e_memory_type region_type)
{
	MCUMemory::memory_region_t *pRegion = &m_regions[region_type];
	uint32_t retval = 0;

	if (pRegion->data_end_address > pRegion->data_start_address)
	{
		uint32_t page_size = pRegion->region_page_size;
		uint32_t start_address_aligned = pRegion->data_start_address & ~(page_size - 1);
		uint32_t length = pRegion->data_end_address - start_address_aligned + 1;
		retval = (length + page_size - 1) & ~(page_size - 1);
	}
	return retval;
}
uint32_t MCUMemory::GetPageAlignedAddress(e_memory_type region_type)
{
	MCUMemory::memory_region_t *pRegion = &m_regions[region_type];
	uint32_t retval = 0;

	if (pRegion->data_end_address > pRegion->data_start_address)
	{
		retval = pRegion->data_start_address & ~(pRegion->region_page_size - 1);
	}
	return retval;
}