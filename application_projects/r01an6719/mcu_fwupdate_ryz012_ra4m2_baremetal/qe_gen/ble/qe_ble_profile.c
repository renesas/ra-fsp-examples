/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : qe_ble_profile.c
* Description  : This file includes definitions.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : MM/DD/YYYY Version Description
*              : 08/31/2021 1.00    First Release
***********************************************************************************************************************/

#include "qe_ble_profile.h"

const attribute_t qe_ble_profile[QE_ATTRIBUTE_HANDLE_PROFILE_END] =
{

	// Profile Declaration
	[0] =
	{
		.handle = 0,
		.encapsulated_attributes = 15,
		.permissions = 0x00, 
		.uuid_length = 0x00,
		.value_length = 0x00,
		.notify_write = 0,
		.notify_read = 0
	},
	// Service Declaration: GAP Service
	[1] =
	{
		.handle = 1,
		.encapsulated_attributes = 11,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x02,
		.uuid = (uint8_t []){0x00,0x28},
		.value = (uint8_t []){0x00,0x18},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Declaration: Device Name
	[2] =
	{
		.handle = 2,
		.encapsulated_attributes = 0,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x05,
		.uuid = (uint8_t []){0x03,0x28},
		.value = (uint8_t []){0x0A,0x03,0x00,0x00,0x2A},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Value: Device Name
	[3] =
	{
		.handle = 3,
		.encapsulated_attributes = 0,
		.permissions = 0x03,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x80,
		.uuid = (uint8_t []){0x00,0x2A},
		.value = (uint8_t []){0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Declaration: Appearance
	[4] =
	{
		.handle = 4,
		.encapsulated_attributes = 0,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x05,
		.uuid = (uint8_t []){0x03,0x28},
		.value = (uint8_t []){0x02,0x05,0x00,0x01,0x2A},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Value: Appearance
	[5] =
	{
		.handle = 5,
		.encapsulated_attributes = 0,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x02,
		.uuid = (uint8_t []){0x01,0x2A},
		.value = (uint8_t []){0x00,0x00},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Declaration: Peripheral Preferred Connection Parameters
	[6] =
	{
		.handle = 6,
		.encapsulated_attributes = 0,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x05,
		.uuid = (uint8_t []){0x03,0x28},
		.value = (uint8_t []){0x02,0x07,0x00,0x04,0x2A},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Value: Peripheral Preferred Connection Parameters
	[7] =
	{
		.handle = 7,
		.encapsulated_attributes = 0,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x08,
		.uuid = (uint8_t []){0x04,0x2A},
		.value = (uint8_t []){0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Declaration: Central Address Resolution
	[8] =
	{
		.handle = 8,
		.encapsulated_attributes = 0,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x05,
		.uuid = (uint8_t []){0x03,0x28},
		.value = (uint8_t []){0x02,0x09,0x00,0xA6,0x2A},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Value: Central Address Resolution
	[9] =
	{
		.handle = 9,
		.encapsulated_attributes = 0,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x01,
		.uuid = (uint8_t []){0xA6,0x2A},
		.value = (uint8_t []){0x00},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Declaration: Resolvable Private Address Only
	[10] =
	{
		.handle = 10,
		.encapsulated_attributes = 0,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x05,
		.uuid = (uint8_t []){0x03,0x28},
		.value = (uint8_t []){0x02,0x0B,0x00,0xC9,0x2A},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Value: Resolvable Private Address Only
	[11] =
	{
		.handle = 11,
		.encapsulated_attributes = 0,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x01,
		.uuid = (uint8_t []){0xC9,0x2A},
		.value = (uint8_t []){0x00},
		.notify_write = 0,
		.notify_read = 0
	},
	// Service Declaration: GATT Service
	[12] =
	{
		.handle = 12,
		.encapsulated_attributes = 4,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x02,
		.uuid = (uint8_t []){0x00,0x28},
		.value = (uint8_t []){0x01,0x18},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Declaration: Service Changed
	[13] =
	{
		.handle = 13,
		.encapsulated_attributes = 0,
		.permissions = 0x01,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x05,
		.uuid = (uint8_t []){0x03,0x28},
		.value = (uint8_t []){0x20,0x0E,0x00,0x05,0x2A},
		.notify_write = 0,
		.notify_read = 0
	},
	// Characteristic Value: Service Changed
	[14] =
	{
		.handle = 14,
		.encapsulated_attributes = 0,
		.permissions = 0x00,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x04,
		.uuid = (uint8_t []){0x05,0x2A},
		.value = (uint8_t []){0x00,0x00,0x00,0x00},
		.notify_write = 0,
		.notify_read = 0
	},
	// Descriptor: Client Characteristic Configuration
	[15] =
	{
		.handle = 15,
		.encapsulated_attributes = 0,
		.permissions = 0x03,
		.uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
		.value_length = 0x02,
		.uuid = (uint8_t []){0x02,0x29},
		.value = (uint8_t []){0x00,0x00},
		.notify_write = 0,
		.notify_read = 0
	}

};
