/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

   	This project demonstrates the basic functionality of an RA4 MCU operating as a BLE Server board establish connection with 
	Renesas browser installed on the device client. An RA MCU using freeRTOS and the Renesas FSP was used to create this example 
	project. This EP "find me profile" is configured to establish the connection with the Renesas GATTBrowser installed on the 
	client device. Led 1 on the server board will provide visual feedback to indicate once the client has established connection 
	or when a disconnection occurs. LED1 is turned on when the connection is established. It will turn off if the client is 
	disconnected. The user can send alert levels, connect requests, or disconnect requests from the Renesas GATTBrowser on the 
	client device. LED0 blink speed is used to indicate which user alert level is selected. If user chooses alert level 0, the 
	LED0 does not blink. If user chooses alert level 1, LED0 blinks slower compared with if use chooses alert level 2. The 
	connection status and LEDs status messages will display on Jlink RTT viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.0.0
	e2 studio: Version 2025-04.1
	SEGGER J-Link RTT Viewer: Version 8.44a
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Device client (iphone or ipad,..v.v.) has Renesas GATT browser installed.

3. Hardware Requirement:

	i. 1x micro usb cable.
        ii. 1x board of EK-RA4W1.
        iii. 1x mobile device with Renesas GATTBrowser installed.

4. Hardware Connections:
 	Supported Board EK_RA4W1:		
	i. Connect RA board to Host machine using micro usb cable.

Note: After running EP "find me profile". User must use Renesas GATT brower to search and connect to Local Name "RA4W1_BLE" displayed on your
      device RA4W1 board server). Send the messages "Alert level" from your device GATTBrowser and the messages will be responded on RTTviewer.
