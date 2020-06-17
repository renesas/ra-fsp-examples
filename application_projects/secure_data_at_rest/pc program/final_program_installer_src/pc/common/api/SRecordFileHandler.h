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

