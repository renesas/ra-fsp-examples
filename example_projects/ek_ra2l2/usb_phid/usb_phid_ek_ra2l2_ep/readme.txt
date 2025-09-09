/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of USB PHID driver on Renesas RA MCUs based on Renesas FSP.
	The project configures RA board as a Keyboard. On pressing any key from RTT Viewer, MCU sends characters from 
	a - z & numbers 0 - 9 to the host PC.

2. Hardware Requirements:
	Supported RA boards: EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA4M3, 
			     EK-RA4M2, EK-RA8M1, EK-RA8D1, EK-RA4E2, EK-RA6E2, MCK-RA8T1, EK-RA4L1, EK-RA8E2, EK-RA8P1,
			     EK-RA2L2.
	1 x Renesas RA board.
	1 x Micro USB cable for debugging and programming.
	1 x Type-C USB cable.
	1 x PC with at least 2 USB ports (1 for debug and 1 for COM port).

3. Hardware Connections:
	For EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2 (Full Speed):
		- Connect the RA board USB device port (J9) to the PC via a micro USB cable.
		- Connect the RA board USB debug port (J11) to the PC via a micro USB cable for EP debugging.
	
	For EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA4M3, EK-RA4M2, EK-RA8M1, EK-RA8D1 (Full Speed):
		- Jumper J12: Connect pins 2-3.
		- Connect Jumper J15 pins.
		- Connect the RA board USB FS port (J11) to the PC via a micro USB cable.
		- Connect the RA board USB debug port (J10) to the PC via a micro USB cable for EP debugging.

	For EK-RA4E2, EK-RA6E2 (Full Speed): 
		- Connect the RA board USB FS port (J11) to the PC via a micro USB cable.
		- Connect the RA board USB debug port (J10) to the PC via a micro USB cable for EP debugging.

	For MCK-RA8T1 (Full Speed):
		- Jumper JP9: Connect pins 2-3.
		- Connect Jumper JP10 pins.
		- Connect the RA board USB FS port (CN14) to the PC via a Type-C USB cable.
		- Connect the RA board USB debug port (CN11) to the PC via a Type-C USB cable for EP debugging.

	For EK-RA4L1 (Full Speed):
		- Turn ON S4-4 to select USB device mode.
		- Set J17 jumper to pins 2-3, Set J7 jumper to use P407 for USBFS VBUS.
		- Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.
		- Connect the RA board USB debug port (J10) to the PC via a Type-C USB cable for EP debugging.

	For EK-RA8E2, EK-RA8P1 (Full Speed):
		- Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.
		- Connect the RA board USB debug port (J10) to the PC via a Type-C USB cable for EP debugging.

	For EK-RA2L2 (Full Speed):
		- Connect the RA board USB FS port (J11) to the PC via a Type-C USB cable.
		- Connect the RA board USB debug port (J10) to the PC via a micro USB cable for EP debugging.

Note:
1) Open "Device Manager", and check "Human Interface Devices", to see "USB Input Device" has been detected.

2) Lowercase letters a - z and numbers 0 - 9 are sent from the MCU to the host PC.﻿

3) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200049a8
   b. Keil: 	Not Available
   c. IAR: 	Not Available

4) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
