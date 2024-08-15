/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates basic functionality of USBX Peripheral Human Interface Device Class module on Renesas RA MCUs 
	based on Renesas FSP using Azure RTOS. User will be able to use RA board as a keyboard(HID Device) by connecting it to 
	the Host PC. Once the board is connected, user can provide any input from RTT Viewer to send character from a - z & numbers 
	0 - 9 from RA board to host PC. USB status and any error messages are displayed on RTT Viewer.

2. Hardware Requirement:
	Micro USB Cable : 2x
	
3. Hardware connection :
	EK-RA6M1,EK-RA6M2:(support only full speed)
	  Connect the micro USB end of the micro USB cable to micro-AB USB Full Speed
	  port (J9) of the board.  Connect the other end of this cable to USB port of the
	  host PC.
	
	EK-RA6M3,EK-RA6M4,EK-RA6M5,EK-RA4M3,EK-RA4M2, EK-RA8M1, EK-RA8D1:(support only full speed)
	  Jumper J12 placement is pins 2-3.
	  Connect jumper J15 pins.
	  Connect the micro USB end of the micro USB cable to micro-AB USB Full Speed
	  port (J11) of the board.  Connect the other end of this cable to USB port of the
	  host PC. 

Note:
1) Known limitation: the USBX_phid USB full speed configuration example project does not work on PCs with Crowdstrike
   software installed.

2) Open Device Manager, and check 'Human Interface Devices', to see ' USB Input Device' has been detected.

3) Lower case a - z and numbers 0 - 9 are sent from MCU to the host PC.﻿

4) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x220009f0
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
5) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
