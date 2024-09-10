/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
* File Name    : SRecordFile.h
* Description  : Class to represent SRecord File
**********************************************************************************************************************/
#ifndef _SRECORD_FILE_H_
#define _SRECORD_FILE_H_

#include <stdint.h>
#include <string>
#include "McuMemory.h"
class SRecordFileHandler
{
public:
	SRecordFileHandler();
	bool Read(const char* file_name, MCUMemory& memory_handle);
	~SRecordFileHandler();
	
	uint32_t GetStartAddress() const
	{
		return start_address;
	}

	std::string GetHeader() const
	{
		return file_header;
	}

private:
	bool ParseRecord(std::string& srecord);
	std::istream& GetRecord(std::istream& srecord_file, std::string& srecord);
	int ReadRecordElement(std::string::iterator& srec_it, std::string& srecord);
	int AsciiHexToInt(char c);
	std::vector <uint8_t> srecord_data;
	uint32_t srecord_address;
	uint8_t srecord_type;
	std::string file_header;
	uint32_t start_address;
};

#endif //_SRECORD_FILE_H_

