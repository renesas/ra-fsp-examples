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

