/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "hal_data.h"

#define SEND_ONLY_AVE						(0x1)
#define SEND_ALL							(0x2)
#define SEND_CURRENT_ANA					(0x3)
#define SEND_WAKEUP_COMPLETE				(0x4)
#define SEND_COMMAND_ERR					(0x5)

#define COMMAND_SIZE						(0x1)

#define COMMAND_ONLY_AVE					(0x31) // '1'
#define COMMAND_ALL							(0x32) // '2'
#define COMMAND_RECV_ANA_LEVEL				(0x33) // '3'

#define START_CODE							(0x5E) // '^'
#define END_CODE							(0x24) // '$'

#define SEND_DATA_SPACE						(0x20) // ' '

#define ATTRIBUTE_ONLY_AVE					(0x31) // '1'
#define ATTRIBUTE_ALL						(0x32) // '2'
#define ATTRIBUTE_CURRENT_ANA				(0x33) // '3'
#define ATTRIBUTE_WAKEUP_COMPLETE			(0x34) // '4'
#define ATTRIBUTE_COMMAND_ERR				(0x35) // '5'

#define PACKET_FORMAT_SIZE_START_CODE		(1u)
#define PACKET_FORMAT_SIZE_ATTRIBUTE_CODE	(1u)
#define PACKET_FORMAT_SIZE_DATA_LENGTH_CODE	(4u)
#define PACKET_FORMAT_SIZE_END_CODE			(1u)
#define SEND_DATA_SIZE_SPACE				(1u)

#define SEND_BUFF_SIZE						(2059u)

extern void data_output(void);
