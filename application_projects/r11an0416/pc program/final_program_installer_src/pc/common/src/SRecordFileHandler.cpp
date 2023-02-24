#include "SRecordFileHandler.h"
#include <fstream>

SRecordFileHandler::SRecordFileHandler()
{

}


bool SRecordFileHandler::Read(const char* file_name, MCUMemory& memory_handle)
{
	bool retval = false;
	std::ifstream srecord_file;
	std::string srecord(512, '\0');

	/*Open the file*/
	srecord_file.open(file_name, std::ios::in);

	retval = (false == srecord_file.fail());

	/*Validate the srecord and identify the memory blobs  */
	while (!GetRecord(srecord_file, srecord).eof() && retval)
	{
		if (ParseRecord(srecord))
		{
			/* Header record */
			if ('0' == srecord_type && file_header.empty())
			{
				file_header.assign(srecord_data.begin(), srecord_data.end());
			}
			/* Add data */
			else if (('1' == srecord_type) || ('2' == srecord_type) || ('3' == srecord_type))
			{
				retval = memory_handle.InsertData(srecord_address, srecord_data);
			}
			/* Add Start Address */
			else if (('7' == srecord_type) || ('8' == srecord_type) || ('9' == srecord_type))
			{
				start_address = srecord_address;
			}
			/* Ignore all other record types */
			else
			{
				/* Do nothing */
			}
		}

	}

	return true;
}

std::istream& SRecordFileHandler::GetRecord(std::istream& srecord_file, std::string& srecord)
{
	srecord.clear();

	std::istream::sentry sentry(srecord_file, true);
	std::streambuf* srec_sb = srecord_file.rdbuf();

	while (1)
	{
		int c = srec_sb->sbumpc();
		switch (c)
		{
		case '\n':
			return srecord_file;
		case '\r':
			if (srec_sb->sgetc() == '\n')
				srec_sb->sbumpc();
			return srecord_file;
		case std::streambuf::traits_type::eof():
			// Handles the case where last line has no line ending 
			if (srecord.empty())
				srecord_file.setstate(std::ios::eofbit);
			return srecord_file;
		default:
			srecord += (char)c;
		}
	}
}

bool SRecordFileHandler::ParseRecord(std::string& srecord)
{
	bool retval = false;
	uint8_t record_address_length = 0;
	uint8_t record_length = 0;
	uint8_t record_checksum = 0;
	std::string::iterator srecord_iterator = srecord.begin();
	int value = 0;
	srecord_data.clear();
	srecord_address = 0;
	srecord_type = 0;

	/* Validate length */
	if ((srecord.length() < 3) || (srecord.length() > 512))
	{
		return false;
	}

	/*validate header  */
	if ((*srecord_iterator++ != 'S'))
	{
		return false;
	}

	/* Get record type */
	srecord_type = *srecord_iterator++;

	/* Validate type and adjust the address lengths */
	switch (srecord_type)
	{
		case '0':
		case '1':
		case '9':
		{
			record_address_length = 2;
			break;
		}
		case '2':
		case '8':
		{
			record_address_length = 3;
			break;
		}

		case '3':
		case '7':
		{
			record_address_length = 4;
			break;
		}

		/* We dont parse any other types or invalid records */
		default:
		{
			return true;
		}
	}

	/*Get the record length */
	value = ReadRecordElement(srecord_iterator, srecord);
	if (value < 0)
	{
		return false;
	}
	record_length = (uint8_t)value;
	record_checksum = record_length;

	/* Get the address */
	while (record_address_length)
	{
		value = ReadRecordElement(srecord_iterator, srecord);
		if (value > -1)
		{
			srecord_address = srecord_address + ((uint8_t)value << (8 * (record_address_length - 1)));
			record_checksum += (uint8_t)value;
			record_address_length--;
			record_length--;
		}
		else
		{
			return false;
		}
	}

	/* Get Data */
	while (record_length > 1)
	{
		value = ReadRecordElement(srecord_iterator, srecord);
		if (value < 0)
		{
			return false;
		}
		srecord_data.push_back((uint8_t)value);
		record_checksum += (uint8_t)value;
		record_length--;
	}

	/* Validate checksum */
	record_checksum ^= 0xFF;
	value = ReadRecordElement(srecord_iterator, srecord);
	retval = ((uint8_t)value == record_checksum) && (srecord_iterator == srecord.end());

	return retval;
}

SRecordFileHandler::~SRecordFileHandler()
{
}

int SRecordFileHandler::ReadRecordElement(std::string::iterator& srec_it, std::string& srecord)
{
	if (srec_it == srecord.end())
	{
		return -1;
	}
	int high_nibble = AsciiHexToInt(*srec_it++);

	if (srec_it == srecord.end())
	{
		return -1;
	}
	int low_nibble = AsciiHexToInt(*srec_it++);

	if ((-1 == high_nibble) || (-1 == low_nibble))
	{
		return -1;
	}

	return (high_nibble << 4) + low_nibble;
}
int SRecordFileHandler::AsciiHexToInt(char c)
{
	if (!isxdigit(c))
	{
		return -1;
	}
	if (isdigit(c))
	{
		return c - '0';
	}
	if (isupper(c))
	{
		c = tolower(c);
	}
	return c - 'a' + 10;
}
