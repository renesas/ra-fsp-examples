/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionality of USB Multiport on Renesas RA MCUs based on Renesas FSP
	using FreeRTOS. USB PCDC is configured for full speed with priority lower than USB HMSC configured as high
	speed. J-Link RTT Viewer is used for the input and status display. Tera Term is used to display the output
	and also to capture the data from it based on user input on J-Link RTT Viewer. The project checks for the
	test.txt and reads the content and sends the data to USB PCDC which is displayed on the Tera Term. Data
	captured from Tera Term (max 512 bytes) will be written to file.
	
2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.1.0
	e2 studio: Version 2025-07
	SEGGER J-Link RTT Viewer: Version 8.58
	Terminal Console Application: Tera Term or a similar application
	USB Serial Drivers (included in Windows 10) 
	Windows 10 operating system

3. Hardware Requirements:
	Supported RA boards: EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1, EK-RA8P1
	1 x Renesas RA board.
	2 x Type-C USB cables.
	1 x Host PC with at least 2 USB ports (1 for debug and 1 for COM port).
	1 x USB flash drive.
	1 x Type-C OTG cable: USB-C to USB-A female host cable.
	
4. Hardware Connections:
	The user needs to connect both USBFS and USBHS to run EP.

	For EK-RA6M3, EK-RA6M3G (High-Speed):
        	Jumper J7: Connect pins 1-2.
          	Remove the jumper J17 pins.
          	Connect USB flash drive to J6 connector via a micro USB host cable.

	For EK-RA6M5, EK-RA8M1, EK-RA8D1 (High-Speed):
	  	Note: For EK-RA8D1, the user needs to turn OFF SW1-6 to use USBHS.
          	Jumper J7: Connect pins 1-2.
          	Remove the jumper J17 pins.
          	Connect USB flash drive to J31 connector via a micro USB host cable.

	For EK-RA8P1 (High-Speed):
		Connect USB flash drive to J7 connector via a Type-C USB Host cable.

    	For EK-RA6M3, EK-RA6M3G, EK-RA6M5, EK-RA8M1, EK-RA8D1 (Full-Speed):
          	Jumper J12: Connect pins 2-3.
          	Connect the jumper J15 pins.
          	Connect micro-AB USB Full Speed port (J11) of the board to USB port of the host PC
                via a micro USB cable.

	For EK-RA8P1 (Full-Speed):
          	Connect Type-C USB Full Speed port (J11) of the board to USB port of the host PC
                via a Type-C USB cable.

Note:
1) USB Mass Storage Device must be formatted with FAT file system.

2) Data written can also be manually verified in file.

3) If the respective board is not powered through Type-C USB debug port (J10) from the host PC,
   the functionality cannot be demonstrated.﻿

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000710
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
