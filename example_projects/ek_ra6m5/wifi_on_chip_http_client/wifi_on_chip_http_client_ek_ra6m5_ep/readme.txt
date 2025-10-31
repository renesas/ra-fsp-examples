/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the functionalities of the HTTP Client within the DA16200 module, employing the FSP Integrated Onchip
	DA16XXX Middleware driver, and leveraging FreeRTOS on Renesas RA MCUs. EP uses the US159-DA16200EVZ Ultra Low Power Wi-Fi Pmod™ 
	module to connect to an access point and communicate with a web server using HTTP methods (GET, PUT, POST). After sending each 
	request, EP waits for the HTTP response and prints it to the RTT Viewer. Additionally, status and failure messages are displayed
	on the RTT Viewer.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

3. Hardware Requirements:
	Board supported: EK-RA6M5
	1 x Renesas board
	1 x Micro USB cable for programming, debugging and observing EP operations on RTT viewer.
	1 x Wi-Fi router Access Point(AP) with internet access. Note: WiFi router AP's credentials should be known.
	1 x US159-DA16200EVZ Ultra Low Power Wi-Fi Pmod™ (Part Number: US159-DA16200MEVZ) (SDK v3.2.8.1).
		**Note: Please ensure the Pmod™ SDK is up-to-date before executing the project. 
				For SDK update instructions, please refer to Section 5, 6, and 7 in the provided link:
					https://www.renesas.com/en/document/apn/da16200da16600-sdk-update-guide

4. Hardware Connections:
	Plug in the US159-DA16200MEVZ to the Pmod2 connector on the MCU evaluation kit and be careful to align Pin 1 on the module to Pin 1 on the MCU kit.
	Connect the USB Debug port on EK-RA6M5 to the PC using a micro USB cable.
	
Note :	  
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000111c
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).


