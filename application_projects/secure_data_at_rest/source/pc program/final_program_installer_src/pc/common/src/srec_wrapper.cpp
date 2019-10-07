#include "srec_wrapper.h"
#include "MCUMemory.h"
#include "SRecordFileHandler.h"
#include "flashR7FS5D97E.h"

static void readData(SrecRegion *pRegion, MCUMemory *pMcuMemory, e_memory_type memory_type);

bool srecFileRead(const char *pFilename, SrecImage* pImage)
{
	MCUMemory mcuMemory(&flashR7FS5D97E);
	SRecordFileHandler srec_handler;

	bool retval = srec_handler.Read(pFilename, mcuMemory);
	if (retval && NULL != pImage)
	{

		if (0 < pImage->u1NumRegions)
		{
			readData(&pImage->psRegions[code_memory], &mcuMemory, code_memory);
		}

		if (1 < pImage->u1NumRegions)
		{
			readData(&pImage->psRegions[data_memory], &mcuMemory, data_memory);
		}
	}

	return retval;
}

static void readData(SrecRegion *pRegion, MCUMemory *pMcuMemory, e_memory_type memory_type)
{
	uint32_t index = 0;
	uint8_t* pBuffer = NULL;
	uint32_t lowest_address = 0;
	uint32_t highest_address = 0;
	uint32_t size = pMcuMemory->GetPageAlignedSize(memory_type);

	if (size > 0)
	{
		pBuffer = (uint8_t*)malloc(size * sizeof(uint8_t));
		if (NULL != pBuffer)
		{
			memcpy(pBuffer, pMcuMemory->GetPageAlignedData(memory_type), size);
		}
		lowest_address = pMcuMemory->GetPageAlignedAddress(memory_type);
		highest_address = lowest_address + size - 1;
	}

	pRegion->u4Size = size;
	pRegion->pu1Data = pBuffer;
	pRegion->u4Address = lowest_address;
}
