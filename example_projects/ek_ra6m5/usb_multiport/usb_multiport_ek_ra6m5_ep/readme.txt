/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:
	This project demonstrates the basic functionality of USB Multiport on Renesas RA MCUs
	based on Renesas FSP using FreeRTOS. USB PCDC is configured for full speed with priority
	lower than USB HMSC configured as high speed. Jlink RTTViewer is used for the input and
	status display. Tera term is used to display the output and also to capture the data from
	it based on user input on Jlink RTT viewer. The project checks for the test.txt and
	reads the content and sends the data to usb_pcdc which is displayed on the Tera term. Data
	captured from Tera term(max 512 bytes) will be written to file.
	
Hardware Requirements:
	1) RA board (RA6M3/RA6M3G/RA6M5)
	2) Micro USB device cable - 2Nos.
	3) A PC with at least 2 USB port(1 for debug and 1 for com port)
	4) USB flash drive - 1Nos.
	5) USB OTG cable   - 1Nos
    
Software Requirements:: 
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	Terminal Console Application: Tera Term or a similar application
	USB Serial Drivers (included in Windows 10) 
	Windows® 10 operating system
	
Hardware Connections:
    *Note: User need to connect both USBFS and USBHS to run EP.
    High-Speed(For EK-RA6M3/EK-RA6M3G):
        Jumper j7: Connect pins 1-2
        Remove Jumper j17 pins
        Connect USB flash device to J6 connector with help of micro USB Host cable.

    High-Speed(For EK-RA6M5):
        Jumper j7: Connect pins 1-2
        Remove Jumper j17 pins
        Connect USB flash device to J31 connector with help of micro USB Host cable.

    Full-Speed:
        Jumper j12 placement is pins 2-3
        Connect Jumper j15 pins
        Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	port (J11) of the board.  Connect the other end of this cable to USB port of the 
	host PC. 

Note:
1. USB Mass Storage Device must be formatted with FAT file system.
2. Data written can also be manually verified in file.
3. If the respective board is not powered through micro-AB USB (DEBUG USB) port (J11)
   from the host PC, then the functionality cannot be demonstrated.﻿
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000138c
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
