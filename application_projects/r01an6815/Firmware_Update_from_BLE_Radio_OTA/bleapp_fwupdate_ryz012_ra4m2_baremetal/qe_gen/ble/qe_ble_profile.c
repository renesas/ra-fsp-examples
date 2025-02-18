/**********************************************************************************************************************
* Copyright (c) 2021 - 2024 Renesas Electronics Corporation and/or its affiliates
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
		.encapsulated_attributes = QE_ATTRIBUTE_HANDLE_PROFILE_END,    // added OTA Service
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
	},
    // 16 :   OTA Service and Characteristic
    //!< OTA Service : required to perform RYZ012 FW Update with TelinkOTA mobile app, choose this Service on mobile
   [QE_ATTRIBUTE_HANDLE_SERVICE_DECL_OTA_SERVICE] =
   {
       .handle = QE_ATTRIBUTE_HANDLE_SERVICE_DECL_OTA_SERVICE,
       .encapsulated_attributes = 4,
       .permissions = 0x01,
       .uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
       .value_length = QE_BLE_PROFILE_UUID_SIZE_CUSTOM,
       .uuid = (uint8_t []){0x00,0x28},
       .value = (uint8_t []){0x12,0x19,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00},
       .notify_write = 0,
       .notify_read = 0
   },
   //!< OTA Service : character value
   [QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_DECL_OTA_CHARVAL] =
   {
       .handle = QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_DECL_OTA_CHARVAL,
       .encapsulated_attributes = 0,
       .permissions = 0x01,
       .uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
       .value_length = 0x0013,
       .uuid = (uint8_t []){0x03,0x28},
       .value = (uint8_t []){ 0x02|0x04,0x48,0x00,0x12,0x2B,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00},
       .notify_write = 0,
       .notify_read = 0
   },
   //!< OTA UUID : SPP data required to perform RYZ012 FW Update with TelinkOTA mobile app, choose this Characteristic on mobile
   [QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_VALUE_OTA_UUID] =
   {
       .handle = QE_ATTRIBUTE_HANDLE_CHARACTERISTIC_VALUE_OTA_UUID,
       .encapsulated_attributes = 0,
       .permissions = 0x01 | 0x02,
       .uuid_length = QE_BLE_PROFILE_UUID_SIZE_CUSTOM,
       .value_length = 0x0001,
       .uuid = (uint8_t []){0x12,0x2B,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00},
       .value = (uint8_t []){0x00},
       .notify_write = 0x02,
       .notify_read = 0x02
   },
   //!< OTA Name
   [QE_ATTRIBUTE_HANDLE_DESCRIPTOR_OTA_NAME] =
   {
       .handle = QE_ATTRIBUTE_HANDLE_DESCRIPTOR_OTA_NAME,
       .encapsulated_attributes = 0,
       .permissions = 0x01,
       .uuid_length = QE_BLE_PROFILE_UUID_SIZE_ADOPTED,
       .value_length = 0x0003,
       .uuid = (uint8_t []){0x01, 0x29},
       .value = (uint8_t []){'O', 'T', 'A'},
       .notify_write = 0,
       .notify_read = 0
    }
};
